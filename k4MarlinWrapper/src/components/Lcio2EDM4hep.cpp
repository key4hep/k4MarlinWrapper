#include "k4MarlinWrapper/converters/Lcio2EDM4hep.h"

#include <EVENT/LCCollection.h>
#include <Exceptions.h>

#include <edm4hep/ClusterCollection.h>
#include <edm4hep/EventHeaderCollection.h>
#include <edm4hep/MCParticleCollection.h>
#include <edm4hep/ParticleIDCollection.h>
#include <edm4hep/RawCalorimeterHitCollection.h>
#include <edm4hep/RawTimeSeriesCollection.h>
#include <edm4hep/ReconstructedParticleCollection.h>
#include <edm4hep/SimCalorimeterHitCollection.h>
#include <edm4hep/SimTrackerHitCollection.h>
#include <edm4hep/TrackCollection.h>
#include <edm4hep/TrackerHitPlaneCollection.h>
#include <edm4hep/VertexCollection.h>

#include "k4EDM4hep2LcioConv/k4Lcio2EDM4hepConv.h"

DECLARE_COMPONENT(Lcio2EDM4hepTool);

Lcio2EDM4hepTool::Lcio2EDM4hepTool(const std::string& type, const std::string& name, const IInterface* parent)
    : GaudiTool(type, name, parent), m_eds("EventDataSvc", "Lcio2EDM4hepTool") {
  declareInterface<IEDMConverter>(this);

  StatusCode sc = m_eds.retrieve();
}

Lcio2EDM4hepTool::~Lcio2EDM4hepTool() { ; }

StatusCode Lcio2EDM4hepTool::initialize() {
  m_podioDataSvc = dynamic_cast<PodioLegacyDataSvc*>(m_eds.get());
  if (nullptr == m_podioDataSvc)
    return StatusCode::FAILURE;

  return GaudiTool::initialize();
}

StatusCode Lcio2EDM4hepTool::finalize() { return GaudiTool::finalize(); }

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

void Lcio2EDM4hepTool::registerCollection(
    std::tuple<const std::string&, std::unique_ptr<podio::CollectionBase>> namedColl, EVENT::LCCollection* lcioColl) {
  auto& [name, e4hColl] = namedColl;
  if (e4hColl == nullptr) {
    error() << "Could not convert collection " << name << endmsg;
    return;
  }

  auto wrapper = new DataWrapper<podio::CollectionBase>();
  wrapper->setData(e4hColl.release());

  // No need to check for pre-existing collections, since we only ever end up
  // here if that is not the case
  auto sc = m_podioDataSvc->registerObject("/Event", "/" + std::string(name), wrapper);
  if (sc == StatusCode::FAILURE) {
    error() << "Could not register collection " << name << endmsg;
  }

  auto handle = DataHandle<podio::CollectionBase>{name, Gaudi::DataHandle::Reader, this};

  // Convert metadata
  if (lcioColl != nullptr) {
    const auto acollid = handle.get()->getID();

    std::vector<std::string> string_keys = {};
    lcioColl->getParameters().getStringKeys(string_keys);

    auto& e4h_coll_md = m_podioDataSvc->getProvider().getCollectionMetaData(acollid);

    for (auto& elem : string_keys) {
      if (elem == "CellIDEncoding") {
        std::string lcio_coll_cellid_str = lcioColl->getParameters().getStringVal(lcio::LCIO::CellIDEncoding);
        e4h_coll_md.setValue("CellIDEncodingString", lcio_coll_cellid_str);
      } else {
        std::string lcio_coll_value = lcioColl->getParameters().getStringVal(elem);
        e4h_coll_md.setValue(elem, lcio_coll_value);
      }
    }
  }
}

StatusCode Lcio2EDM4hepTool::convertCollections(lcio::LCEventImpl* the_event) {
  // Convert Event Header outside the collections loop
  if (!collectionExist("EventHeader")) {
    registerCollection("EventHeader", LCIO2EDM4hepConv::createEventHeader(the_event));
  }

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

  auto lcio2edm4hepMaps = LCIO2EDM4hepConv::LcioEdmTypeMapping{};

  std::vector<std::pair<std::string, EVENT::LCCollection*>>               lcRelationColls{};
  std::vector<std::tuple<std::string, EVENT::LCCollection*, std::string>> subsetColls{};

  for (const auto& [lcioName, edm4hepName] : collsToConvert) {
    try {
      auto* lcio_coll = the_event->getCollection(lcioName);
      debug() << "Converting collection " << lcioName << " (storing it as " << edm4hepName << "). ";
      if (collectionExist(edm4hepName)) {
        debug() << "Collection already exists, skipping." << endmsg;
        continue;  // No need to convert again
      }
      const auto& lcio_coll_type_str = lcio_coll->getTypeName();
      debug() << "LCIO type of the relation is " << lcio_coll_type_str << endmsg;

      // We deal with subset collections and LCRelations once we have all data
      // converted
      if (lcio_coll->isSubset()) {
        subsetColls.emplace_back(std::make_tuple(edm4hepName, lcio_coll, lcio_coll_type_str));
        continue;
      }
      if (lcio_coll_type_str == "LCRelation") {
        lcRelationColls.emplace_back(std::make_pair(edm4hepName, lcio_coll));
      }

      for (auto&& e4hColl : LCIO2EDM4hepConv::convertCollection(edm4hepName, lcio_coll, lcio2edm4hepMaps)) {
        if (std::get<1>(e4hColl)) {
          registerCollection(std::move(e4hColl));
        } else {
          error() << "Could not convert collection " << lcioName << " (type: " << lcio_coll_type_str << ")" << endmsg;
        }
      }
    } catch (const lcio::DataNotAvailableException& ex) {
      warning() << "LCIO Collection " << lcioName << " not found in the event, skipping conversion to EDM4hep"
                << endmsg;
      continue;
    }
  }

  // Now we can resolve relations, subset collections and LCRelations
  LCIO2EDM4hepConv::resolveRelations(lcio2edm4hepMaps);

  for (const auto& [name, coll, type] : subsetColls) {
    registerCollection(name, LCIO2EDM4hepConv::fillSubset(coll, lcio2edm4hepMaps, type));
  }

  for (auto&& assocColl : LCIO2EDM4hepConv::createAssociations(lcio2edm4hepMaps, lcRelationColls)) {
    registerCollection(std::move(assocColl));
  }

  return StatusCode::SUCCESS;
}
