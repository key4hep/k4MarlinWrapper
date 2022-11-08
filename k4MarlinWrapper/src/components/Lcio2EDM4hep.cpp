#include "k4MarlinWrapper/converters/Lcio2EDM4hep.h"

DECLARE_COMPONENT(Lcio2EDM4hepTool);

Lcio2EDM4hepTool::Lcio2EDM4hepTool(const std::string& type, const std::string& name, const IInterface* parent)
    : GaudiTool(type, name, parent), m_eds("EventDataSvc", "Lcio2EDM4hepTool") {
  declareInterface<IEDMConverter>(this);

  StatusCode sc = m_eds.retrieve();
}

Lcio2EDM4hepTool::~Lcio2EDM4hepTool() { ; }

StatusCode Lcio2EDM4hepTool::initialize() {
  m_podioDataSvc = dynamic_cast<PodioDataSvc*>(m_eds.get());
  if (nullptr == m_podioDataSvc)
    return StatusCode::FAILURE;

  return GaudiTool::initialize();
}

StatusCode Lcio2EDM4hepTool::finalize() {
  for (const auto& [key, val] : m_dataHandlesMap) {
    delete val;
  }

  return GaudiTool::finalize();
}

// **********************************
// Convert EDM4hep collection using k4LCIOConverter
// Register converted collection with Podio Data Service to Event Store
// **********************************
template <typename T>
void Lcio2EDM4hepTool::convertRegister(const std::string& edm_name, const std::string& lcio_name,
                                       std::unique_ptr<k4LCIOConverter>& lcio_converter,
                                       const lcio::LCCollection* const lcio_coll, const bool cnv_metadata) {
  debug() << "Converting collection: " << lcio_name << " from LCIO to EDM4hep " << edm_name << endmsg;

  // Convert and get EDM4hep collection
  auto* e4h_generic_coll = lcio_converter->getCollection(lcio_name);

  if (e4h_generic_coll == nullptr) {
    error() << "LCIO Collection " << lcio_name << " failed to convert to EDM4hep in k4LCIOConverter." << endmsg;
    return;
  }

  // Cast to specific type based on typename
  T* mycoll = dynamic_cast<T*>(e4h_generic_coll);
  if (mycoll == nullptr) {
    error() << "LCIO Collection " << lcio_name
            << " did not get correct type to be registered. Type was: " << typeid(T).name() << endmsg;
    return;
  }

  // Manually register object instead of using DataHandle
  DataWrapper<T>* wrapper = new DataWrapper<T>();
  wrapper->setData(mycoll);

  // Check if collection was already registered
  auto collections   = m_podioDataSvc->getCollections();
  bool is_registered = false;
  for (auto& coll : collections) {
    if (coll.first == edm_name) {
      is_registered = true;
      return;
    }
  }

  if (is_registered == false) {
    StatusCode sc = m_podioDataSvc->registerObject("/Event", "/" + std::string(edm_name), wrapper);
    if (sc == StatusCode::FAILURE) {
      error() << "Error registering collection " << edm_name << endmsg;
      return;
    }
  } else {
    debug() << "Collection " << edm_name << " was already registered" << endmsg;
  }

  // Convert Metadata
  if (cnv_metadata) {
    // int params_size = lcio_coll->getParameters().size();

    DataHandle<T> ahandle{edm_name, Gaudi::DataHandle::Reader, this};
    const auto    acoll   = ahandle.get();
    auto          acollid = acoll->getID();

    std::vector<std::string> string_keys = {};
    lcio_coll->getParameters().getStringKeys(string_keys);

    auto& e4h_coll_md = m_podioDataSvc->getProvider().getCollectionMetaData(acollid);

    for (auto& elem : string_keys) {
      if (elem == "CellIDEncoding") {
        std::string lcio_coll_cellid_str = lcio_coll->getParameters().getStringVal(lcio::LCIO::CellIDEncoding);
        e4h_coll_md.setValue("CellIDEncodingString", lcio_coll_cellid_str);
      } else {
        std::string lcio_coll_value = lcio_coll->getParameters().getStringVal(elem);
        e4h_coll_md.setValue(elem, lcio_coll_value);
      }
    }
  }
}

// **********************************
// Check if a collection was already registered to skip it
// **********************************
bool Lcio2EDM4hepTool::collectionExist(const std::string& collection_name) {
  auto collections = m_podioDataSvc->getCollections();
  for (const auto& [name, coll] : collections) {
    if (collection_name == name) {
      debug() << "Collection named " << name << " already registered, skipping conversion." << endmsg;
      return true;
    }
  }
  return false;
}

// **********************************
// - Convert all collections indicated in Tool parameters
// - Some collections implicitly convert associated collections, as for
// key4hep/k4LCIOReader
// - Convert associated collections from LCRelation for existing EDM4hep relations
// - Converted collections are put into TES
// **********************************
StatusCode Lcio2EDM4hepTool::convertCollections(lcio::LCEventImpl* the_event) {
  // Set the event to the converter
  podio::CollectionIDTable*        id_table       = m_podioDataSvc->getCollectionIDs();
  std::unique_ptr<k4LCIOConverter> lcio_converter = std::make_unique<k4LCIOConverter>(id_table);
  lcio_converter->set(the_event);

  // Start off with the pre-defined collection name mappings
  auto collsToConvert{m_collNames.value()};
  if (m_convertAll) {
    const auto* collections = the_event->getCollectionNames();
    for (const auto& collName : *collections) {
      // And simply add the rest, exploiting the fact that emplace will not
      // replace existing entries with the same key
      collsToConvert.emplace(collName, collName);
    }
  }

  // Convert Event Header outside the collections loop
  if (!collectionExist("EventHeader")) {
    convertRegister<edm4hep::EventHeaderCollection>("EventHeader", "EventHeader", lcio_converter, nullptr);
  }

  // Convert collections indicated in the parameters
  for (const auto& [lcioName, edm4hepName] : collsToConvert) {
    if (!collectionExist(edm4hepName)) {
      std::string         lcio_coll_type_str = "";
      lcio::LCCollection* lcio_coll          = nullptr;
      try {
        // Get type string from collection name
        lcio_coll          = the_event->getCollection(lcioName);
        lcio_coll_type_str = lcio_coll->getTypeName();
      } catch (const lcio::DataNotAvailableException& ex) {
        warning() << "LCIO Collection " << lcioName << " not found in the event, skipping conversion to EDM4hep"
                  << endmsg;
        continue;
      }

      if (lcio_coll_type_str == "ReconstructedParticle") {
        convertRegister<edm4hep::ReconstructedParticleCollection>(edm4hepName, lcioName, lcio_converter, lcio_coll);
        // Get associated collection. Name hardcoded in k4LCIOConverter
        convertRegister<edm4hep::ParticleIDCollection>("ParticleID_EXT", "ParticleID_EXT", lcio_converter, nullptr);
        convertRegister<edm4hep::VertexCollection>("Vertex_EXT", "Vertex_EXT", lcio_converter, nullptr);
      } else if (lcio_coll_type_str == "ParticleID") {
        convertRegister<edm4hep::ParticleIDCollection>(edm4hepName, lcioName, lcio_converter, lcio_coll);
      } else if (lcio_coll_type_str == "MCParticle") {
        convertRegister<edm4hep::MCParticleCollection>(edm4hepName, lcioName, lcio_converter, lcio_coll);
      } else if (lcio_coll_type_str == "Vertex") {
        convertRegister<edm4hep::VertexCollection>(edm4hepName, lcioName, lcio_converter, lcio_coll);
      } else if (lcio_coll_type_str == "Track") {
        convertRegister<edm4hep::TrackCollection>(edm4hepName, lcioName, lcio_converter, lcio_coll);
      } else if (lcio_coll_type_str == "TrackerHit") {
        convertRegister<edm4hep::TrackerHitCollection>(edm4hepName, lcioName, lcio_converter, lcio_coll, true);
      } else if (lcio_coll_type_str == "TrackerHitPlane") {
        convertRegister<edm4hep::TrackerHitPlaneCollection>(edm4hepName, lcioName, lcio_converter, lcio_coll, true);
      } else if (lcio_coll_type_str == "SimTrackerHit") {
        convertRegister<edm4hep::SimTrackerHitCollection>(edm4hepName, lcioName, lcio_converter, lcio_coll, true);
      } else if (lcio_coll_type_str == "CalorimeterHit") {
        convertRegister<edm4hep::CalorimeterHitCollection>(edm4hepName, lcioName, lcio_converter, lcio_coll, true);
      } else if (lcio_coll_type_str == "SimCalorimeterHit") {
        convertRegister<edm4hep::SimCalorimeterHitCollection>(edm4hepName, lcioName, lcio_converter, lcio_coll, true);
        // Get associated collections
        // This collection name is hardcoded in k4LCIOConverter
        convertRegister<edm4hep::CaloHitContributionCollection>("CaloHitContribution_EXT", "CaloHitContribution_EXT",
                                                                lcio_converter, nullptr);
      } else if (lcio_coll_type_str == "RawCalorimeterHit") {
        convertRegister<edm4hep::RawCalorimeterHitCollection>(edm4hepName, lcioName, lcio_converter, lcio_coll);
      } else if (lcio_coll_type_str == "TPCHit") {
        convertRegister<edm4hep::TPCHitCollection>(edm4hepName, lcioName, lcio_converter, lcio_coll);
      } else if (lcio_coll_type_str == "Cluster") {
        convertRegister<edm4hep::ClusterCollection>(edm4hepName, lcioName, lcio_converter, lcio_coll);
        // Get associated collection. Name hardcoded in k4LCIOConverter
        convertRegister<edm4hep::ParticleIDCollection>("ParticleID_EXT", "ParticleID_EXT", lcio_converter, nullptr);
      } else if (lcio_coll_type_str == "LCRelation") {
        // Get specific relation type from converted
        auto* conv_collection = lcio_converter->getCollection(lcioName);
        if (conv_collection == nullptr) {
          error() << "Unable to convert collection " << lcioName << std::endl;
          continue;
        }
        auto e4h_coll_type_str = conv_collection->getValueTypeName();

        if (e4h_coll_type_str == "edm4hep::MCRecoCaloAssociation") {
          convertRegister<edm4hep::MCRecoCaloAssociationCollection>(edm4hepName, lcioName, lcio_converter, lcio_coll);
        } else if (e4h_coll_type_str == "edm4hep::MCRecoTrackerAssociation") {
          convertRegister<edm4hep::MCRecoTrackerAssociationCollection>(edm4hepName, lcioName, lcio_converter,
                                                                       lcio_coll);
        } else if (e4h_coll_type_str == "edm4hep::MCRecoParticleAssociation") {
          convertRegister<edm4hep::MCRecoParticleAssociationCollection>(edm4hepName, lcioName, lcio_converter,
                                                                        lcio_coll);
        } else if (e4h_coll_type_str == "edm4hep::MCRecoCaloParticleAssociation") {
          convertRegister<edm4hep::MCRecoCaloParticleAssociationCollection>(edm4hepName, lcioName, lcio_converter,
                                                                            lcio_coll);
        } else if (e4h_coll_type_str == "edm4hep::MCRecoTrackParticleAssociation") {
          convertRegister<edm4hep::MCRecoTrackParticleAssociationCollection>(edm4hepName, lcioName, lcio_converter,
                                                                             lcio_coll);
        } else if (e4h_coll_type_str == "edm4hep::RecoParticleVertexAssociation") {
          convertRegister<edm4hep::RecoParticleVertexAssociationCollection>(edm4hepName, lcioName, lcio_converter,
                                                                            lcio_coll);
        } else {
          error() << "Unsuported LCRelation for collection " << lcioName << std::endl;
        }

      } else {
        error() << lcio_coll_type_str << ": conversion type not supported." << endmsg;
      }
    } else {
      debug() << "EDM4hep Collection " << edm4hepName << " already in place, skipping conversion." << endmsg;
    }
  }

  return StatusCode::SUCCESS;
}
