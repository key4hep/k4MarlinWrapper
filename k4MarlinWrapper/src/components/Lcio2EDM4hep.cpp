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

template <typename T>
void Lcio2EDM4hepTool::registerCollection(
    std::tuple<const std::string&, std::unique_ptr<podio::CollectionBase>> namedColl, EVENT::LCCollection* lcioColl) {
  auto& [name, e4hColl] = namedColl;
  if (e4hColl == nullptr) {
    error() << "Could not convert collection " << name << endmsg;
    return;
  }
  DataHandle<T> handle{name, Gaudi::DataHandle::Writer, this};
  handle.put(static_cast<T*>(e4hColl.release()));

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
    registerCollection<edm4hep::EventHeaderCollection>("EventHeader", LCIO2EDM4hepConv::createEventHeader(the_event));
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

  auto [lcio2edm4hepMaps, lcRelationColls, subsetColls] = convertCollectionData(collsToConvert, the_event);

  // Now we can resolve relations, subset collections and LCRelations
  LCIO2EDM4hepConv::resolveRelations(lcio2edm4hepMaps);
  createSubsetColls(subsetColls, lcio2edm4hepMaps);

  return StatusCode::SUCCESS;
}

std::tuple<LCIO2EDM4hepConv::LcioEdmTypeMapping, std::vector<std::tuple<std::string, EVENT::LCCollection*>>,
           std::vector<std::tuple<std::string, EVENT::LCCollection*, std::string>>>
Lcio2EDM4hepTool::convertCollectionData(const std::map<std::string, std::string>& collsToConvert,
                                        lcio::LCEventImpl*                        the_event) {
  using namespace LCIO2EDM4hepConv;

  auto lcio2edm4hepMaps = LcioEdmTypeMapping{};

  std::vector<std::tuple<std::string, EVENT::LCCollection*>>              lcRelationColls{};
  std::vector<std::tuple<std::string, EVENT::LCCollection*, std::string>> subsetColls{};

  for (const auto& [lcioName, edm4hepName] : collsToConvert) {
    try {
      auto*       lcio_coll          = the_event->getCollection(lcioName);
      const auto& lcio_coll_type_str = lcio_coll->getTypeName();

      // We deal with subset collections and LCRelations once we have all data
      // converted
      if (lcio_coll->isSubset()) {
        subsetColls.emplace_back(std::make_tuple(edm4hepName, lcio_coll, lcio_coll_type_str));
        continue;
      }
      if (lcio_coll_type_str == "LCRelation") {
        lcRelationColls.emplace_back(std::make_tuple(edm4hepName, lcio_coll));
      }

      if (lcio_coll_type_str == "ReconstructedParticle") {
        auto e4hColls = convertReconstructedParticles(edm4hepName, lcio_coll, lcio2edm4hepMaps.recoParticles,
                                                      lcio2edm4hepMaps.particleIDs);
        registerCollection<edm4hep::ReconstructedParticleCollection>(std::move(e4hColls[0]));
        registerCollection<edm4hep::ParticleIDCollection>(std::move(e4hColls[1]));
      } else if (lcio_coll_type_str == "MCParticle") {
        registerCollection(edm4hepName, convertMCParticles(edm4hepName, lcio_coll, lcio2edm4hepMaps.mcParticles));
      } else if (lcio_coll_type_str == "Vertex") {
        registerCollection(edm4hepName, convertVertices(edm4hepName, lcio_coll, lcio2edm4hepMaps.vertices));
      } else if (lcio_coll_type_str == "Track") {
        registerCollection(edm4hepName, convertTracks(edm4hepName, lcio_coll, lcio2edm4hepMaps.tracks));
      } else if (lcio_coll_type_str == "TrackerHit") {
        registerCollection(edm4hepName, convertTrackerHits(edm4hepName, lcio_coll, lcio2edm4hepMaps.trackerHits),
                           lcio_coll);
      } else if (lcio_coll_type_str == "TrackerHitPlane") {
        registerCollection(
            edm4hepName, convertTrackerHitPlanes(edm4hepName, lcio_coll, lcio2edm4hepMaps.trackerHitPlanes), lcio_coll);
      } else if (lcio_coll_type_str == "SimTrackerHit") {
        registerCollection(edm4hepName, convertSimTrackerHits(edm4hepName, lcio_coll, lcio2edm4hepMaps.simTrackerHits),
                           lcio_coll);
      } else if (lcio_coll_type_str == "SimCalorimeterHit") {
        registerCollection(edm4hepName, convertSimCalorimeterHits(edm4hepName, lcio_coll, lcio2edm4hepMaps.simCaloHits),
                           lcio_coll);
      } else if (lcio_coll_type_str == "RawCalorimeterHit") {
        registerCollection(edm4hepName,
                           convertRawCalorimeterHits(edm4hepName, lcio_coll, lcio2edm4hepMaps.rawCaloHits));
      } else if (lcio_coll_type_str == "TPCHit") {
        registerCollection(edm4hepName, convertTPCHits(edm4hepName, lcio_coll, lcio2edm4hepMaps.tpcHits));
      } else if (lcio_coll_type_str == "Cluster") {
        registerCollection(edm4hepName, convertClusters(edm4hepName, lcio_coll, lcio2edm4hepMaps.clusters));
        // TODO: Particle IDs related to Clusters. Needs converter support!
      } else {
        error() << lcio_coll_type_str << ": conversion type not supported." << endmsg;
      }
    } catch (const lcio::DataNotAvailableException& ex) {
      warning() << "LCIO Collection " << lcioName << " not found in the event, skipping conversion to EDM4hep"
                << endmsg;
      continue;
    }
  }

  return std::make_tuple(std::move(lcio2edm4hepMaps), std::move(lcRelationColls), std::move(subsetColls));
}

void Lcio2EDM4hepTool::createSubsetColls(
    const std::vector<std::tuple<std::string, EVENT::LCCollection*, std::string>>& subsetColls,
    const LCIO2EDM4hepConv::LcioEdmTypeMapping&                                    lcio2edm4hepMaps) {
  using namespace LCIO2EDM4hepConv;

  for (const auto& [name, coll, type] : subsetColls) {
    if (type == "MCParticle") {
      registerCollection(name, handleSubsetColl<edm4hep::MCParticleCollection>(coll, lcio2edm4hepMaps.mcParticles));
    } else if (type == "ReconstructedParticle") {
      registerCollection(
          name, handleSubsetColl<edm4hep::ReconstructedParticleCollection>(coll, lcio2edm4hepMaps.recoParticles));
    } else if (type == "Vertex") {
      registerCollection(name, handleSubsetColl<edm4hep::VertexCollection>(coll, lcio2edm4hepMaps.vertices));
    } else if (type == "Track") {
      registerCollection(name, handleSubsetColl<edm4hep::TrackCollection>(coll, lcio2edm4hepMaps.tracks));
    } else if (type == "Cluster") {
      registerCollection(name, handleSubsetColl<edm4hep::ClusterCollection>(coll, lcio2edm4hepMaps.clusters));
    } else if (type == "SimCalorimeterHit") {
      registerCollection(name,
                         handleSubsetColl<edm4hep::SimCalorimeterHitCollection>(coll, lcio2edm4hepMaps.simCaloHits));
    } else if (type == "RawCalorimeterHit") {
      registerCollection(name,
                         handleSubsetColl<edm4hep::RawCalorimeterHitCollection>(coll, lcio2edm4hepMaps.rawCaloHits));
    } else if (type == "CalorimeterHit") {
      registerCollection(name, handleSubsetColl<edm4hep::CalorimeterHitCollection>(coll, lcio2edm4hepMaps.caloHits));
    } else if (type == "SimTrackerHit") {
      registerCollection(name,
                         handleSubsetColl<edm4hep::SimTrackerHitCollection>(coll, lcio2edm4hepMaps.simTrackerHits));
    } else if (type == "TPCHit") {
      registerCollection(name, handleSubsetColl<edm4hep::RawTimeSeriesCollection>(coll, lcio2edm4hepMaps.tpcHits));
    } else if (type == "TrackerHit") {
      registerCollection(name, handleSubsetColl<edm4hep::TrackerHitCollection>(coll, lcio2edm4hepMaps.trackerHits));
    } else if (type == "TrackerHitPlane") {
      registerCollection(name,
                         handleSubsetColl<edm4hep::TrackerHitPlaneCollection>(coll, lcio2edm4hepMaps.trackerHitPlanes));
    } else {
      error() << type << ": conversion type not supported." << endmsg;
    }
  }
}

void Lcio2EDM4hepTool::createAssociations(
    const std::vector<std::tuple<std::string, EVENT::LCCollection*>>& lcRelationColls,
    const LCIO2EDM4hepConv::LcioEdmTypeMapping&                       lcio2edm4hepMaps) {
  using namespace LCIO2EDM4hepConv;

  for (const auto& [name, lcioColl] : lcRelationColls) {
    const auto& params   = lcioColl->getParameters();
    const auto& fromType = params.getStringVal("FromType");
    const auto& toType   = params.getStringVal("ToType");

    if (fromType.empty() || toType.empty()) {
      error() << "LCRelation collection " << name << " has missing FromType or ToType parameters. "
              << "Cannot convert it without this information." << endmsg;
      continue;
    }

    using namespace LCIO2EDM4hepConv;

    if (fromType == "MCParticle" && toType == "ReconstructedParticle") {
      registerCollection(name, createAssociationCollection<edm4hep::MCRecoParticleAssociationCollection, false>(
                                   lcioColl, lcio2edm4hepMaps.mcParticles, lcio2edm4hepMaps.recoParticles));
    } else if (fromType == "ReconstructedParticle" && toType == "MCParticle") {
      registerCollection(name, createAssociationCollection<edm4hep::MCRecoParticleAssociationCollection, true>(
                                   lcioColl, lcio2edm4hepMaps.recoParticles, lcio2edm4hepMaps.mcParticles));
    } else if (fromType == "CalorimeterHit" && toType == "SimCalorimeterHit") {
      registerCollection(name, createAssociationCollection<edm4hep::MCRecoCaloAssociationCollection, true>(
                                   lcioColl, lcio2edm4hepMaps.caloHits, lcio2edm4hepMaps.simCaloHits));
    } else if (fromType == "SimCalorimeterHit" && toType == "CalorimeterHit") {
      registerCollection(name, createAssociationCollection<edm4hep::MCRecoCaloAssociationCollection, false>(
                                   lcioColl, lcio2edm4hepMaps.simCaloHits, lcio2edm4hepMaps.caloHits));
    } else if (fromType == "Cluster" && toType == "MCParticle") {
      registerCollection(name, createAssociationCollection<edm4hep::MCRecoClusterParticleAssociationCollection, true>(
                                   lcioColl, lcio2edm4hepMaps.clusters, lcio2edm4hepMaps.mcParticles));
    } else if (fromType == "MCParticle" && toType == "Cluster") {
      registerCollection(name, createAssociationCollection<edm4hep::MCRecoClusterParticleAssociationCollection, false>(
                                   lcioColl, lcio2edm4hepMaps.mcParticles, lcio2edm4hepMaps.clusters));
    } else if (fromType == "MCParticle" && toType == "Track") {
      registerCollection(name, createAssociationCollection<edm4hep::MCRecoTrackParticleAssociationCollection, false>(
                                   lcioColl, lcio2edm4hepMaps.mcParticles, lcio2edm4hepMaps.tracks));
    } else if (fromType == "Track" && toType == "MCParticle") {
      registerCollection(name, createAssociationCollection<edm4hep::MCRecoTrackParticleAssociationCollection, true>(
                                   lcioColl, lcio2edm4hepMaps.tracks, lcio2edm4hepMaps.mcParticles));
    } else if (fromType == "TrackerHit" && toType == "SimTrackerHit") {
      registerCollection(name, createAssociationCollection<edm4hep::MCRecoTrackerAssociationCollection, true>(
                                   lcioColl, lcio2edm4hepMaps.trackerHits, lcio2edm4hepMaps.simTrackerHits));
    } else if (fromType == "SimTrackerHit" && toType == "TrackerHit") {
      registerCollection(name, createAssociationCollection<edm4hep::MCRecoTrackerAssociationCollection, false>(
                                   lcioColl, lcio2edm4hepMaps.simTrackerHits, lcio2edm4hepMaps.trackerHits));
    } else if (fromType == "SimTrackerHit" && toType == "TrackerHitPlane") {
      registerCollection(name, createAssociationCollection<edm4hep::MCRecoTrackerHitPlaneAssociationCollection, false>(
                                   lcioColl, lcio2edm4hepMaps.simTrackerHits, lcio2edm4hepMaps.trackerHitPlanes));
    } else if (fromType == "TrackerHitPlane" && toType == "SimTrackerHit") {
      registerCollection(name, createAssociationCollection<edm4hep::MCRecoTrackerHitPlaneAssociationCollection, true>(
                                   lcioColl, lcio2edm4hepMaps.trackerHitPlanes, lcio2edm4hepMaps.simTrackerHits));
    } else if (fromType == "ReconstructedParticle" && toType == "Vertex") {
      registerCollection(name, createAssociationCollection<edm4hep::RecoParticleVertexAssociationCollection, true>(
                                   lcioColl, lcio2edm4hepMaps.recoParticles, lcio2edm4hepMaps.vertices));
    } else if (fromType == "Vertex" && toType == "ReconstructedParticle") {
      registerCollection(name, createAssociationCollection<edm4hep::RecoParticleVertexAssociationCollection, false>(
                                   lcioColl, lcio2edm4hepMaps.vertices, lcio2edm4hepMaps.recoParticles));
    } else {
      error() << "Relation from: " << fromType << " to: " << toType << " (" << name
              << ") is not beeing handled during creation of associations" << endmsg;
    }
  }
}
