#include "k4MarlinWrapper/converters/Lcio2EDM4hep.h"


DECLARE_COMPONENT(Lcio2EDM4hepTool);

Lcio2EDM4hepTool::Lcio2EDM4hepTool(const std::string& type, const std::string& name, const IInterface* parent)
    : GaudiTool(type, name, parent), m_eds("EventDataSvc", "Lcio2EDM4hepTool") {
  declareInterface<IEDMConverter>(this);

  StatusCode sc = m_eds.retrieve();
}

Lcio2EDM4hepTool::~Lcio2EDM4hepTool() { ; }


StatusCode Lcio2EDM4hepTool::initialize() {

  if (m_lcio2edm_params.size() % 2 != 0) {
    error() << " Error processing conversion parameters. 2 arguments (LCIO name, EDM4hep name) per collection expected. " << endmsg;
    return StatusCode::FAILURE;
  }

  m_podioDataSvc = dynamic_cast<PodioDataSvc*>(m_eds.get());
  if (nullptr == m_podioDataSvc) return StatusCode::FAILURE;

  return GaudiTool::initialize();
}

StatusCode Lcio2EDM4hepTool::finalize() {

  for (const auto& [key, val] : m_dataHandlesMap) {
    delete val;
  }

  return GaudiTool::finalize();
}

template <typename T>
void Lcio2EDM4hepTool::convertPut(
  const std::string& edm_name,
  const std::string& lcio_name,
  podio::CollectionBase* generic_coll,
  podio::CollectionIDTable* id_table)
{
  T* mycoll = dynamic_cast<T*>(generic_coll);

  if (mycoll == nullptr) {
    error() << "Collection named " << lcio_name << " was null" << endmsg;
    return;
  }

  // Manually register object instead of using DataHandle
  DataWrapper<T>* wrapper = new DataWrapper<T>();
  wrapper->setData(mycoll);
  StatusCode sc = m_podioDataSvc->registerObject("/Event", "/" + std::string(edm_name), wrapper);
  if (sc == StatusCode::FAILURE) {
    error() << "Error registering collection " << edm_name << endmsg;
  }
}


bool Lcio2EDM4hepTool::collectionExist(
  const std::string& collection_name)
{
  auto collections = m_podioDataSvc->getCollections();
  for (const auto& [name, coll] : collections) {
    if (collection_name == name){
      std::cout << name << std::endl;;
      return true;
    }
  }
  return false;
}


StatusCode Lcio2EDM4hepTool::convertCollections(
  lcio::LCEventImpl* the_event)
{
  // Set the event to the converter
  podio::CollectionIDTable* id_table = new podio::CollectionIDTable();
  k4LCIOConverter* lcio_converter = new k4LCIOConverter(id_table);
  lcio_converter->set(the_event);

  if (! collectionExist("EventHeader")) {
    auto e4h_generic_coll = lcio_converter->getCollection("EventHeader");
    convertPut<edm4hep::EventHeaderCollection>(
      "EventHeader", "EventHeader", e4h_generic_coll, id_table);
  }

  // Convert based on parameters
  for (int i = 0; i < m_lcio2edm_params.size(); i=i+2) {
    if (! collectionExist(m_lcio2edm_params[i+1])) {

      std::string lcio_coll_type_str = "";
      try {
        // Get type string from collection name
        auto lcio_coll = the_event->getCollection(m_lcio2edm_params[i]);
        lcio_coll_type_str = lcio_coll->getTypeName();
      } catch (const lcio::DataNotAvailableException& ex) {
        warning() << "Collection " << m_lcio2edm_params[i] << " not found, skipping conversion to EDM4hep" << endmsg;
        continue;
      }

      // Get EDM4hep converted type
      auto e4h_generic_coll = lcio_converter->getCollection(m_lcio2edm_params[i]);

      if (e4h_generic_coll == nullptr) {
        error() << "Collection " << m_lcio2edm_params[i] << " failed to convert; returned null" << endmsg;
      }

      if (lcio_coll_type_str == "ReconstructedParticle") {
        convertPut<edm4hep::ReconstructedParticleCollection>(
          m_lcio2edm_params[i+1], m_lcio2edm_params[i], e4h_generic_coll, id_table);
        // Get associated collection. Name hardcoded in k4LCIOConverter
        convertPut<edm4hep::ParticleIDCollection>(
          "ParticleID_EXT", "ParticleID_EXT", e4h_generic_coll, id_table);
        convertPut<edm4hep::VertexCollection>(
          "Vertex_EXT", "Vertex_EXT", e4h_generic_coll, id_table);
      } else if (lcio_coll_type_str == "ParticleID") {
        convertPut<edm4hep::ParticleIDCollection>(
          m_lcio2edm_params[i+1], m_lcio2edm_params[i], e4h_generic_coll, id_table);
      } else if (lcio_coll_type_str == "MCParticle") {
        convertPut<edm4hep::MCParticleCollection>(
          m_lcio2edm_params[i+1], m_lcio2edm_params[i], e4h_generic_coll, id_table);
      } else if (lcio_coll_type_str == "Vertex") {
        convertPut<edm4hep::VertexCollection>(
          m_lcio2edm_params[i+1], m_lcio2edm_params[i], e4h_generic_coll, id_table);
      } else if (lcio_coll_type_str == "Track") {
        convertPut<edm4hep::TrackCollection>(
          m_lcio2edm_params[i+1], m_lcio2edm_params[i], e4h_generic_coll, id_table);
      } else if (lcio_coll_type_str == "TrackerHit") {
        convertPut<edm4hep::TrackerHitCollection>(
          m_lcio2edm_params[i+1], m_lcio2edm_params[i], e4h_generic_coll, id_table);
      } else if (lcio_coll_type_str == "TrackerHitPlane") {
        convertPut<edm4hep::TrackerHitPlaneCollection>(
          m_lcio2edm_params[i+1], m_lcio2edm_params[i], e4h_generic_coll, id_table);
      } else if (lcio_coll_type_str == "SimTrackerHit") {
        convertPut<edm4hep::SimTrackerHitCollection>(
          m_lcio2edm_params[i+1], m_lcio2edm_params[i], e4h_generic_coll, id_table);
      } else if (lcio_coll_type_str == "CalorimeterHit") {
        convertPut<edm4hep::CalorimeterHitCollection>(
          m_lcio2edm_params[i+1], m_lcio2edm_params[i], e4h_generic_coll, id_table);
      } else if (lcio_coll_type_str == "SimCalorimeterHit") {
        convertPut<edm4hep::SimCalorimeterHitCollection>(
          m_lcio2edm_params[i+1], m_lcio2edm_params[i], e4h_generic_coll, id_table);
        // Get associated collections
        // This collection name is hardcoded in k4LCIOConverter
        convertPut<edm4hep::CaloHitContributionCollection>(
          "CaloHitContribution_EXT", "CaloHitContribution_EXT", e4h_generic_coll, id_table);
      } else if (lcio_coll_type_str == "RawCalorimeterHit") {
        convertPut<edm4hep::RawCalorimeterHitCollection>(
          m_lcio2edm_params[i+1], m_lcio2edm_params[i], e4h_generic_coll, id_table);
      } else if (lcio_coll_type_str == "TPCHit") {
        convertPut<edm4hep::TPCHitCollection>(
          m_lcio2edm_params[i+1], m_lcio2edm_params[i], e4h_generic_coll, id_table);
      } else if (lcio_coll_type_str == "Cluster") {
        convertPut<edm4hep::ClusterCollection>(
          m_lcio2edm_params[i+1], m_lcio2edm_params[i], e4h_generic_coll, id_table);
        // Get associated collection. Name hardcoded in k4LCIOConverter
        convertPut<edm4hep::ParticleIDCollection>(
          "ParticleID_EXT", "ParticleID_EXT", lcio_converter, id_table);
      } else if (coll_type_str == "LCRelation") {
        convertPut<edm4hep::MCRecoTrackerAssociationCollection>(
          m_lcio2edm_params[i+1], m_lcio2edm_params[i], e4h_generic_coll, id_table);
      } else if (lcio_coll_type_str == "LCRelation") {

        // Get specific relation type from converted
        auto e4h_coll_type_str = e4h_generic_coll->getValueTypeName();

        if (e4h_coll_type_str == "edm4hep::MCRecoCaloAssociation") {
          convertPut<edm4hep::MCRecoCaloAssociationCollection>(
            m_lcio2edm_params[i+1], m_lcio2edm_params[i], e4h_generic_coll, id_table);
        } else if (e4h_coll_type_str == "edm4hep::MCRecoTrackerAssociation") {
          convertPut<edm4hep::MCRecoTrackerAssociationCollection>(
            m_lcio2edm_params[i+1], m_lcio2edm_params[i], e4h_generic_coll, id_table);
        } else if (e4h_coll_type_str == "edm4hep::MCRecoParticleAssociation") {
          convertPut<edm4hep::MCRecoParticleAssociationCollection>(
            m_lcio2edm_params[i+1], m_lcio2edm_params[i], e4h_generic_coll, id_table);
        } else if (e4h_coll_type_str == "edm4hep::MCRecoCaloParticleAssociation") {
          convertPut<edm4hep::MCRecoCaloParticleAssociationCollection>(
            m_lcio2edm_params[i+1], m_lcio2edm_params[i], e4h_generic_coll, id_table);
        }

      } else {
        error() << lcio_coll_type_str << ": conversion type not supported." << endmsg;
      }
    } else {
      debug() << " Collection " << m_lcio2edm_params[i+1] << " already in place, skipping conversion. " << endmsg;
    }
  }

  return StatusCode::SUCCESS;
}
