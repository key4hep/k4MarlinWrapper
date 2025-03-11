/*
 * Copyright (c) 2019-2024 Key4hep-Project.
 *
 * This file is part of Key4hep.
 * See https://key4hep.github.io/key4hep-doc/ for further info.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "k4MarlinWrapper/converters/EDM4hep2Lcio.h"
#include <GaudiKernel/StatusCode.h>
#include "GlobalConvertedObjectsMap.h"
#include "StoreUtils.h"

#include "UTIL/PIDHandler.h"

#include "edm4hep/Constants.h"
#include "edm4hep/utils/ParticleIDUtils.h"

#include "k4FWCore/DataHandle.h"
#include "k4FWCore/FunctionalUtils.h"
#include "k4FWCore/MetaDataHandle.h"
#include "k4FWCore/PodioDataSvc.h"

#include "GaudiKernel/AnyDataWrapper.h"
#include "GaudiKernel/IDataManagerSvc.h"
#include "GaudiKernel/IDataProviderSvc.h"

#include <functional>
#include <memory>

DECLARE_COMPONENT(EDM4hep2LcioTool);

using namespace k4MarlinWrapper;

struct CollectionPairMappings {
  TrackMap           tracks{};
  TrackerHitMap      trackerHits{};
  TrackerHitPlaneMap trackerHitPlanes{};
  SimTrackerHitMap   simTrackerHits{};
  CaloHitMap         caloHits{};
  RawCaloHitMap      rawCaloHits{};
  SimCaloHitMap      simCaloHits{};
  TPCHitMap          tpcHits{};
  ClusterMap         clusters{};
  VertexMap          vertices{};
  RecoParticleMap    recoParticles{};
  MCParticleMap      mcParticles{};
  ParticleIDMap      particleIDs{};
};

EDM4hep2LcioTool::EDM4hep2LcioTool(const std::string& type, const std::string& name, const IInterface* parent)
    : AlgTool(type, name, parent), m_eventDataSvc("EventDataSvc", "EDM4hep2LcioTool") {
  declareInterface<IEDMConverter>(this);
}

StatusCode EDM4hep2LcioTool::initialize() {
  StatusCode sc = m_eventDataSvc.retrieve();
  if (sc == StatusCode::FAILURE) {
    error() << "Could not retrieve the EventDataSvc;" << endmsg;
    return StatusCode::FAILURE;
  }

  m_podioDataSvc = dynamic_cast<PodioDataSvc*>(m_eventDataSvc.get());

  if (!m_podioDataSvc) {
    m_metadataSvc = service("MetadataSvc", false);
    if (!m_metadataSvc) {
      error() << "Could not retrieve MetadataSvc" << endmsg;
      return StatusCode::FAILURE;
    }
  }

  return AlgTool::initialize();
}

StatusCode EDM4hep2LcioTool::finalize() { return AlgTool::finalize(); }

// Convert EDM4hep Tracks to LCIO
// Add converted LCIO ptr and original EDM4hep collection to vector of pairs
// Add LCIO Collection Vector to LCIO event
void EDM4hep2LcioTool::convertTracks(TrackMap& tracks_vec, const std::string& e4h_coll_name,
                                     const std::string& lcio_coll_name, lcio::LCEventImpl* lcio_event) {
  DataHandle<edm4hep::TrackCollection> tracks_handle{e4h_coll_name, Gaudi::DataHandle::Reader, this};
  const auto*                          tracks_coll = tracks_handle.get();

  auto conv_tracks = EDM4hep2LCIOConv::convertTracks(tracks_coll, tracks_vec);

  // Add all tracks to event
  lcio_event->addCollection(conv_tracks.release(), lcio_coll_name);
}

// Convert EDM4hep TrackerHits to LCIO
// Add converted LCIO ptr and original EDM4hep collection to vector of pairs
// Add LCIO Collection Vector to LCIO event
void EDM4hep2LcioTool::convertTrackerHits(TrackerHitMap& trackerhits_vec, const std::string& e4h_coll_name,
                                          const std::string& lcio_coll_name, lcio::LCEventImpl* lcio_event) {
  DataHandle<edm4hep::TrackerHit3DCollection> trackerhits_handle{e4h_coll_name, Gaudi::DataHandle::Reader, this};
  const auto                                  trackerhits_coll = trackerhits_handle.get();

  MetaDataHandle<std::string> cellIDStrHandle{trackerhits_handle, edm4hep::labels::CellIDEncoding,
                                              Gaudi::DataHandle::Reader};

  auto conv_trackerhits =
      EDM4hep2LCIOConv::convertTrackerHits(trackerhits_coll, cellIDStrHandle.get(""), trackerhits_vec);

  // Add all trackerhits to event
  lcio_event->addCollection(conv_trackerhits.release(), lcio_coll_name);
}

void EDM4hep2LcioTool::convertParticleIDs(ParticleIDMap& pidMap, const std::string& e4h_coll_name, int32_t algoId) {
  DataHandle<edm4hep::ParticleIDCollection> pidHandle{e4h_coll_name, Gaudi::DataHandle::Reader, this};

  EDM4hep2LCIOConv::convertParticleIDs(pidHandle.get(), pidMap, algoId);
}

void EDM4hep2LcioTool::convertTrackerHitPlanes(TrackerHitPlaneMap& trackerhits_vec, const std::string& e4h_coll_name,
                                               const std::string& lcio_coll_name, lcio::LCEventImpl* lcio_event) {
  DataHandle<edm4hep::TrackerHitPlaneCollection> trackerhits_handle{e4h_coll_name, Gaudi::DataHandle::Reader, this};
  const auto                                     trackerhits_coll = trackerhits_handle.get();

  MetaDataHandle<std::string> cellIDStrHandle{trackerhits_handle, edm4hep::labels::CellIDEncoding,
                                              Gaudi::DataHandle::Reader};

  auto conv_trackerhits =
      EDM4hep2LCIOConv::convertTrackerHitPlanes(trackerhits_coll, cellIDStrHandle.get(""), trackerhits_vec);

  // Add all trackerhits to event
  lcio_event->addCollection(conv_trackerhits.release(), lcio_coll_name);
}

// Convert EDM4hep SimTrackerHits to LCIO
// Add converted LCIO ptr and original EDM4hep collection to vector of pairs
// Add LCIO Collection Vector to LCIO event
void EDM4hep2LcioTool::convertSimTrackerHits(SimTrackerHitMap& simtrackerhits_vec, const std::string& e4h_coll_name,
                                             const std::string& lcio_coll_name, lcio::LCEventImpl* lcio_event) {
  DataHandle<edm4hep::SimTrackerHitCollection> simtrackerhits_handle{e4h_coll_name, Gaudi::DataHandle::Reader, this};
  const auto                                   simtrackerhits_coll = simtrackerhits_handle.get();

  MetaDataHandle<std::string> cellIDHandle{simtrackerhits_handle, edm4hep::labels::CellIDEncoding,
                                           Gaudi::DataHandle::Reader};
  const auto                  cellIDstr = cellIDHandle.get();

  auto conv_simtrackerhits =
      EDM4hep2LCIOConv::convertSimTrackerHits(simtrackerhits_coll, cellIDstr, simtrackerhits_vec);

  // Add all simtrackerhits to event
  lcio_event->addCollection(conv_simtrackerhits.release(), lcio_coll_name);
}

// Convert EDM4hep Calorimeter Hits to LCIO
// Add converted LCIO ptr and original EDM4hep collection to vector of pairs
// Add converted LCIO Collection Vector to LCIO event
void EDM4hep2LcioTool::convertCalorimeterHits(CaloHitMap& calo_hits_vec, const std::string& e4h_coll_name,
                                              const std::string& lcio_coll_name, lcio::LCEventImpl* lcio_event) {
  DataHandle<edm4hep::CalorimeterHitCollection> calohit_handle{e4h_coll_name, Gaudi::DataHandle::Reader, this};
  const auto                                    calohit_coll = calohit_handle.get();

  MetaDataHandle<std::string> cellIDHandle{calohit_handle, edm4hep::labels::CellIDEncoding, Gaudi::DataHandle::Reader};
  const auto                  cellIDstr = cellIDHandle.get();

  auto conv_calohits = EDM4hep2LCIOConv::convertCalorimeterHits(calohit_coll, cellIDstr, calo_hits_vec);

  // Add all Calorimeter Hits to event
  lcio_event->addCollection(conv_calohits.release(), lcio_coll_name);
}

// Convert EDM4hep RAW Calorimeter Hits to LCIO
// Add converted LCIO ptr and original EDM4hep collection to vector of pairs
// Add converted LCIO Collection Vector to LCIO event
void EDM4hep2LcioTool::convertRawCalorimeterHits(RawCaloHitMap& raw_calo_hits_vec, const std::string& e4h_coll_name,
                                                 const std::string& lcio_coll_name, lcio::LCEventImpl* lcio_event) {
  DataHandle<edm4hep::RawCalorimeterHitCollection> raw_calohit_handle{e4h_coll_name, Gaudi::DataHandle::Reader, this};
  const auto                                       rawcalohit_coll = raw_calohit_handle.get();

  auto conv_rawcalohits = EDM4hep2LCIOConv::convertRawCalorimeterHits(rawcalohit_coll, raw_calo_hits_vec);

  // Add all Raw Calorimeter Hits to event
  lcio_event->addCollection(conv_rawcalohits.release(), lcio_coll_name);
}

// Convert EDM4hep Sim Calorimeter Hits to LCIO
// Add converted LCIO ptr and original EDM4hep collection to vector of pairs
// Add converted LCIO Collection Vector to LCIO event
void EDM4hep2LcioTool::convertSimCalorimeterHits(SimCaloHitMap& sim_calo_hits_vec, const std::string& e4h_coll_name,
                                                 const std::string& lcio_coll_name, lcio::LCEventImpl* lcio_event) {
  DataHandle<edm4hep::SimCalorimeterHitCollection> sim_calohit_handle{e4h_coll_name, Gaudi::DataHandle::Reader, this};
  const auto                                       simcalohit_coll = sim_calohit_handle.get();

  MetaDataHandle<std::string> cellIDHandle{sim_calohit_handle, edm4hep::labels::CellIDEncoding,
                                           Gaudi::DataHandle::Reader};
  const auto                  cellIDstr = cellIDHandle.get();

  // TODO mcparticles_vdc
  auto conv_simcalohits = EDM4hep2LCIOConv::convertSimCalorimeterHits(simcalohit_coll, cellIDstr, sim_calo_hits_vec);

  // Add all Sim Calorimeter Hits to event
  lcio_event->addCollection(conv_simcalohits.release(), lcio_coll_name);
}

// Convert EDM4hep TPC Hits to LCIO
// Add converted LCIO ptr and original EDM4hep collection to vector of pairs
// Add converted LCIO Collection Vector to LCIO event
void EDM4hep2LcioTool::convertTPCHits(TPCHitMap& tpc_hits_vec, const std::string& e4h_coll_name,
                                      const std::string& lcio_coll_name, lcio::LCEventImpl* lcio_event) {
  DataHandle<edm4hep::RawTimeSeriesCollection> tpchit_handle{e4h_coll_name, Gaudi::DataHandle::Reader, this};
  const auto                                   tpchit_coll = tpchit_handle.get();

  auto conv_tpchits = EDM4hep2LCIOConv::convertTPCHits(tpchit_coll, tpc_hits_vec);

  // Add all TPC Hits to event
  lcio_event->addCollection(conv_tpchits.release(), lcio_coll_name);
}

// Convert EDM4hep Clusters to LCIO
// Add converted LCIO ptr and original EDM4hep collection to vector of pairs
// Add converted LCIO Collection Vector to LCIO event
void EDM4hep2LcioTool::convertClusters(ClusterMap& cluster_vec, const std::string& e4h_coll_name,
                                       const std::string& lcio_coll_name, lcio::LCEventImpl* lcio_event) {
  DataHandle<edm4hep::ClusterCollection> cluster_handle{e4h_coll_name, Gaudi::DataHandle::Reader, this};
  const auto                             cluster_coll = cluster_handle.get();

  auto conv_clusters = EDM4hep2LCIOConv::convertClusters(cluster_coll, cluster_vec);

  // Add clusters to event
  lcio_event->addCollection(conv_clusters.release(), lcio_coll_name);
}

// Convert EDM4hep Vertices to LCIO
// Add converted LCIO ptr and original EDM4hep collection to vector of pairs
// Add converted LCIO Collection Vector to LCIO event
void EDM4hep2LcioTool::convertVertices(VertexMap& vertex_vec, const std::string& e4h_coll_name,
                                       const std::string& lcio_coll_name, lcio::LCEventImpl* lcio_event) {
  DataHandle<edm4hep::VertexCollection> vertex_handle{e4h_coll_name, Gaudi::DataHandle::Reader, this};
  const auto                            vertex_coll = vertex_handle.get();

  auto conv_vertices = EDM4hep2LCIOConv::convertVertices(vertex_coll, vertex_vec);

  // Add all tracks to event
  lcio_event->addCollection(conv_vertices.release(), lcio_coll_name);
}

// Convert MC Particles to LCIO
// Add converted LCIO ptr and original EDM4hep collection to vector of pairs
// Add converted LCIO Collection Vector to LCIO event
void EDM4hep2LcioTool::convertMCParticles(MCParticleMap& mc_particles_vec, const std::string& e4h_coll_name,
                                          const std::string& lcio_coll_name, lcio::LCEventImpl* lcio_event) {
  // MCParticles handle
  DataHandle<edm4hep::MCParticleCollection> mcparticle_handle{e4h_coll_name, Gaudi::DataHandle::Reader, this};
  const auto                                mcparticle_coll = mcparticle_handle.get();

  auto conv_mcparticles = EDM4hep2LCIOConv::convertMCParticles(mcparticle_coll, mc_particles_vec);

  // Add all reconstructed particles to event
  lcio_event->addCollection(conv_mcparticles.release(), lcio_coll_name);
}

// Convert EDM4hep RecoParticles to LCIO
// Add converted LCIO ptr and original EDM4hep collection to vector of pairs
// Add converted LCIO Collection Vector to LCIO event
void EDM4hep2LcioTool::convertReconstructedParticles(RecoParticleMap&   recoparticles_vec,
                                                     const std::string& e4h_coll_name,
                                                     const std::string& lcio_coll_name, lcio::LCEventImpl* lcio_event) {
  // ReconstructedParticles handle
  DataHandle<edm4hep::ReconstructedParticleCollection> recos_handle{e4h_coll_name, Gaudi::DataHandle::Reader, this};
  const auto                                           recos_coll = recos_handle.get();

  auto conv_recops = EDM4hep2LCIOConv::convertReconstructedParticles(recos_coll, recoparticles_vec);

  // Add all reconstructed particles to event
  lcio_event->addCollection(conv_recops.release(), lcio_coll_name);
}

// Transfer info from EDM4hep EventHeader to LCIO event
void EDM4hep2LcioTool::convertEventHeader(const std::string& e4h_coll_name, lcio::LCEventImpl* lcio_event) {
  DataHandle<edm4hep::EventHeaderCollection> header_handle{e4h_coll_name, Gaudi::DataHandle::Reader, this};
  const auto                                 header_coll = header_handle.get();

  if (header_coll->size() != 1) {
    error() << "Header collection contains " << header_coll->size() << " headers, expected 1." << endmsg;
    return;
  }
  EDM4hep2LCIOConv::convertEventHeader(header_coll, lcio_event);
}

podio::CollectionBase* EDM4hep2LcioTool::getEDM4hepCollection(const std::string& collName) const {
  DataObject* p;
  auto        sc = m_eventDataSvc->retrieveObject(collName, p);
  if (sc.isFailure()) {
    throw GaudiException("Collection not found: " + collName, name(), StatusCode::FAILURE);
  }
  auto ptr = dynamic_cast<DataWrapperBase*>(p);
  if (ptr) {
    return ptr->collectionBase();
  }
  // When the collection can't be retrieved from the frame there is still the
  // possibility that it was generated from a functional algorithm
  // We keep for a while the possibility of obtaining the collections from
  // std::shared_ptr but this has been removed in k4FWCore so it can be deleted
  // at some point
  auto uptr = dynamic_cast<AnyDataWrapper<std::unique_ptr<podio::CollectionBase>>*>(p);
  if (uptr) {
    return uptr->getData().get();
  }
  auto sptr = dynamic_cast<AnyDataWrapper<std::shared_ptr<podio::CollectionBase>>*>(p);
  if (sptr) {
    return sptr->getData().get();
  }

  throw GaudiException("Collection could not be casted to the expected type", name(), StatusCode::FAILURE);
}

// Select the appropriate method to convert a collection given its type
void EDM4hep2LcioTool::convertAdd(const std::string& e4h_coll_name, const std::string& lcio_coll_name,
                                  lcio::LCEventImpl* lcio_event, CollectionPairMappings& collection_pairs,
                                  std::vector<EDM4hep2LCIOConv::ParticleIDConvData>& pidCollections,
                                  std::vector<EDM4hep2LCIOConv::TrackDqdxConvData>&  dQdxCollections) {
  const auto& metadata = m_podioDataSvc->getMetaDataFrame();
  const auto  collPtr  = getEDM4hepCollection(e4h_coll_name);
  const auto  fulltype = collPtr->getValueTypeName();

  debug() << "Converting type " << fulltype << " from input " << e4h_coll_name << endmsg;

  if (fulltype == "edm4hep::Track") {
    convertTracks(collection_pairs.tracks, e4h_coll_name, lcio_coll_name, lcio_event);
  } else if (fulltype == "edm4hep::TrackerHit" || fulltype == "edm4hep::TrackerHit3D") {
    convertTrackerHits(collection_pairs.trackerHits, e4h_coll_name, lcio_coll_name, lcio_event);
  } else if (fulltype == "edm4hep::TrackerHitPlane") {
    convertTrackerHitPlanes(collection_pairs.trackerHitPlanes, e4h_coll_name, lcio_coll_name, lcio_event);
  } else if (fulltype == "edm4hep::SimTrackerHit") {
    convertSimTrackerHits(collection_pairs.simTrackerHits, e4h_coll_name, lcio_coll_name, lcio_event);
  } else if (fulltype == "edm4hep::CalorimeterHit") {
    convertCalorimeterHits(collection_pairs.caloHits, e4h_coll_name, lcio_coll_name, lcio_event);
  } else if (fulltype == "edm4hep::RawCalorimeterHit") {
    convertRawCalorimeterHits(collection_pairs.rawCaloHits, e4h_coll_name, lcio_coll_name, lcio_event);
  } else if (fulltype == "edm4hep::SimCalorimeterHit") {
    convertSimCalorimeterHits(collection_pairs.simCaloHits, e4h_coll_name, lcio_coll_name, lcio_event);
  } else if (fulltype == "edm4hep::RawTimeSeries") {
    convertTPCHits(collection_pairs.tpcHits, e4h_coll_name, lcio_coll_name, lcio_event);
  } else if (fulltype == "edm4hep::Cluster") {
    convertClusters(collection_pairs.clusters, e4h_coll_name, lcio_coll_name, lcio_event);
  } else if (fulltype == "edm4hep::Vertex") {
    convertVertices(collection_pairs.vertices, e4h_coll_name, lcio_coll_name, lcio_event);
  } else if (fulltype == "edm4hep::MCParticle") {
    convertMCParticles(collection_pairs.mcParticles, e4h_coll_name, lcio_coll_name, lcio_event);
  } else if (fulltype == "edm4hep::ReconstructedParticle") {
    convertReconstructedParticles(collection_pairs.recoParticles, e4h_coll_name, lcio_coll_name, lcio_event);
  } else if (fulltype == "edm4hep::EventHeader") {
    convertEventHeader(e4h_coll_name, lcio_event);
  } else if (fulltype == "edm4hep::ParticleID") {
    std::optional<edm4hep::utils::ParticleIDMeta> pidInfo;
    if (m_podioDataSvc) {
      pidInfo = edm4hep::utils::PIDHandler::getAlgoInfo(metadata, e4h_coll_name);
    } else {
      pidInfo = m_metadataSvc->get<edm4hep::utils::ParticleIDMeta>(e4h_coll_name);
    }

    pidCollections.emplace_back(e4h_coll_name, static_cast<const edm4hep::ParticleIDCollection*>(collPtr), pidInfo);
  } else if (fulltype == "edm4hep::RecDqDx") {
    dQdxCollections.emplace_back(e4h_coll_name, static_cast<const edm4hep::RecDqdxCollection*>(collPtr));
  }

  else if (fulltype == "edm4hep::CaloHitContribution") {
    debug() << "CaloHitContribution collection cannot be converted standalone. "
            << "SimCalorimeterHit collection to be converted in order to be able to attach to them" << endmsg;
  } else {
    warning() << "Error trying to convert requested " << fulltype << " with name " << e4h_coll_name << endmsg;
    warning() << "List of supported types: "
              << "Track, TrackerHit3D, TrackerHitPlane, SimTrackerHit, "
              << "Cluster, CalorimeterHit, RawCalorimeterHit, "
              << "SimCalorimeterHit, Vertex, ReconstructedParticle, "
              << "MCParticle." << endmsg;
  }
}

// Parse property parameters and convert the indicated collections.
// Use the collection names in the parameters to read and write them
StatusCode EDM4hep2LcioTool::convertCollections(lcio::LCEventImpl* lcio_event) {
  std::optional<std::reference_wrapper<const podio::Frame>> edmEvent;
  if (m_collectionNames.empty() && m_podioDataSvc) {
    edmEvent          = m_podioDataSvc->getEventFrame();
    m_collectionNames = edmEvent.value().get().getAvailableCollections();
  } else if (m_collectionNames.empty()) {
    std::optional<std::map<uint32_t, std::string>> idToNameOpt(std::move(m_idToName));
    auto                                           collections = getAvailableCollectionsFromStore(this, idToNameOpt);
    m_idToName                                                 = std::move(idToNameOpt.value());
    m_collectionNames.insert(m_collectionNames.end(), collections.begin(), collections.end());
  }
  // Start off with the pre-defined collection name mappings
  auto collsToConvert{m_collNames.value()};
  // We *always* want to convert the EventHeader
  collsToConvert.emplace(edm4hep::labels::EventHeader, "<directly into LCEvent>");
  if (m_convertAll) {
    info() << "Converting all collections from EDM4hep to LCIO" << endmsg;
    // And simply add the rest, exploiting the fact that emplace will not
    // replace existing entries with the same key
    for (const auto& name : m_collectionNames) {
      collsToConvert.emplace(name, name);
    }
  }

  CollectionPairMappings                            collection_pairs{};
  std::vector<EDM4hep2LCIOConv::ParticleIDConvData> pidCollections{};
  std::vector<EDM4hep2LCIOConv::TrackDqdxConvData>  dQdxCollections{};

  std::vector<std::tuple<std::string, const podio::CollectionBase*>> linkCollections{};

  for (const auto& [edm4hepName, lcioName] : collsToConvert) {
    const auto coll = getEDM4hepCollection(edm4hepName);
    if (coll->getTypeName().find("LinkCollection") != std::string_view::npos) {
      debug() << edm4hepName << " is a link collection, converting it later" << endmsg;
      linkCollections.emplace_back(lcioName, coll);
      continue;
    }
    debug() << "Converting collection " << edm4hepName << " (storing it as " << lcioName << ")" << endmsg;
    if (!EDM4hep2LCIOConv::collectionExist(lcioName, lcio_event)) {
      convertAdd(edm4hepName, lcioName, lcio_event, collection_pairs, pidCollections, dQdxCollections);
    } else {
      debug() << " Collection " << lcioName << " already in place, skipping conversion. " << endmsg;
    }
  }
  debug() << "Event: " << lcio_event->getEventNumber() << " Run: " << lcio_event->getRunNumber() << endmsg;

  EDM4hep2LCIOConv::sortParticleIDs(pidCollections);
  if (!m_podioDataSvc) {
    DataObject* p;
    StatusCode  code = m_eventDataSvc->retrieveObject("/Event" + k4FWCore::frameLocation, p);
    if (code.isSuccess()) {
      auto* frame = dynamic_cast<AnyDataWrapper<podio::Frame>*>(p);
      edmEvent    = std::cref(frame->getData());
    } else {
      auto frame = podio::Frame{};
      edmEvent   = frame;
    }
  }
  for (const auto& pidCollMeta : pidCollections) {
    auto algoId = attachParticleIDMetaData(lcio_event, edmEvent.value(), pidCollMeta);
    if (!algoId.has_value()) {
      // Now go over the collections that have been produced in a functional algorithm (if any)
      bool found = false;
      if (!m_podioDataSvc) {
        const auto id = (*pidCollMeta.coll)[0].getParticle().id().collectionID;
        if (auto it = m_idToName.find(id); it != m_idToName.end()) {
          auto name = it->second;
          if (pidCollMeta.metadata.has_value()) {
            UTIL::PIDHandler pidHandler(lcio_event->getCollection(name));
            algoId =
                pidHandler.addAlgorithm(pidCollMeta.metadata.value().algoName, pidCollMeta.metadata.value().paramNames);
            found = true;
          }
        }
      }
      if (!found) {
        warning() << "Could not determine algorithm type for ParticleID collection " << pidCollMeta.name
                  << " for setting consistent metadata" << endmsg;
      }
    }
    convertParticleIDs(collection_pairs.particleIDs, pidCollMeta.name, algoId.value_or(-1));
  }

  EDM4hep2LCIOConv::attachDqdxInfo(collection_pairs.tracks, dQdxCollections);

  // We want one "global" map that is created the first time it is used in the event.
  DataObject* obj = nullptr;
  auto        sc  = evtSvc()->retrieveObject(GlobalConvertedObjectsMap::TESpath.data(), obj);
  if (sc.isFailure()) {
    debug() << "Creating GlobalconvertedObjectsMap for this event since it is not already in the EventStore" << endmsg;
    auto globalObjMapWrapper = new AnyDataWrapper(GlobalConvertedObjectsMap{});
    auto nsc                 = evtSvc()->registerObject(GlobalConvertedObjectsMap::TESpath.data(), globalObjMapWrapper);
    if (nsc.isFailure()) {
      error() << "Could not register GlobalConvertedObjectsMap in the EventStore" << endmsg;
      return StatusCode::FAILURE;
    }
    obj = globalObjMapWrapper;
  }

  auto  globalObjMapWrapper = static_cast<AnyDataWrapper<GlobalConvertedObjectsMap>*>(obj);
  auto& globalObjMap        = globalObjMapWrapper->getData();

  debug() << "Updating global object map" << endmsg;
  globalObjMap.update(collection_pairs);

  debug() << "Resolving relations between objects" << endmsg;
  EDM4hep2LCIOConv::resolveRelations(collection_pairs, globalObjMap);

  // Now we can convert the links and add them to the event
  debug() << "Converting " << linkCollections.size() << " link collections to LCRelation collections" << endmsg;
  for (auto& [name, coll] : EDM4hep2LCIOConv::createLCRelationCollections(linkCollections, globalObjMap)) {
    debug() << "Adding LCRelation collection " << name << " of type " << coll->getTypeName() << endmsg;
    lcio_event->addCollection(coll.release(), name);
  }

  return StatusCode::SUCCESS;
}
