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
#include "GlobalConvertedObjectsMap.h"

#include "edm4hep/Constants.h"

#include "k4FWCore/DataHandle.h"
#include "k4FWCore/MetaDataHandle.h"

#include "GaudiKernel/AnyDataWrapper.h"

#include <memory>

DECLARE_COMPONENT(EDM4hep2LcioTool);

using namespace k4MarlinWrapper;

using GlobalMapWrapper = AnyDataWrapper<GlobalConvertedObjectsMap>;

struct CollectionPairMappings {
  TrackMap         tracks{};
  TrackerHitMap    trackerHits{};
  SimTrackerHitMap simTrackerHits{};
  CaloHitMap       caloHits{};
  RawCaloHitMap    rawCaloHits{};
  SimCaloHitMap    simCaloHits{};
  TPCHitMap        tpcHits{};
  ClusterMap       clusters{};
  VertexMap        vertices{};
  RecoParticleMap  recoParticles{};
  MCParticleMap    mcParticles{};
  ParticleIDMap    particleIDs{};
};

EDM4hep2LcioTool::EDM4hep2LcioTool(const std::string& type, const std::string& name, const IInterface* parent)
    : GaudiTool(type, name, parent), m_eventDataSvc("EventDataSvc", "EDM4hep2LcioTool") {
  declareInterface<IEDMConverter>(this);
}

EDM4hep2LcioTool::~EDM4hep2LcioTool() { ; }

StatusCode EDM4hep2LcioTool::initialize() {
  StatusCode sc  = m_eventDataSvc.retrieve();
  m_podioDataSvc = dynamic_cast<PodioDataSvc*>(m_eventDataSvc.get());

  if (sc == StatusCode::FAILURE) {
    error() << "Error retrieving Event Data Service" << endmsg;
    return StatusCode::FAILURE;
  }

  return GaudiTool::initialize();
}

StatusCode EDM4hep2LcioTool::finalize() { return GaudiTool::finalize(); }

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

  MetaDataHandle<std::string> cellIDStrHandle{trackerhits_handle, edm4hep::CellIDEncoding, Gaudi::DataHandle::Reader};

  auto conv_trackerhits =
      EDM4hep2LCIOConv::convertTrackerHits(trackerhits_coll, cellIDStrHandle.get(), trackerhits_vec);

  // Add all trackerhits to event
  lcio_event->addCollection(conv_trackerhits.release(), lcio_coll_name);
}

void EDM4hep2LcioTool::convertParticleIDs(ParticleIDMap& pidMap, const std::string& e4h_coll_name, int32_t algoId) {
  DataHandle<edm4hep::ParticleIDCollection> pidHandle{e4h_coll_name, Gaudi::DataHandle::Reader, this};

  EDM4hep2LCIOConv::convertParticleIDs(pidHandle.get(), pidMap, algoId);
}

// Convert EDM4hep SimTrackerHits to LCIO
// Add converted LCIO ptr and original EDM4hep collection to vector of pairs
// Add LCIO Collection Vector to LCIO event
void EDM4hep2LcioTool::convertSimTrackerHits(SimTrackerHitMap& simtrackerhits_vec, const std::string& e4h_coll_name,
                                             const std::string& lcio_coll_name, lcio::LCEventImpl* lcio_event) {
  DataHandle<edm4hep::SimTrackerHitCollection> simtrackerhits_handle{e4h_coll_name, Gaudi::DataHandle::Reader, this};
  const auto                                   simtrackerhits_coll = simtrackerhits_handle.get();

  MetaDataHandle<std::string> cellIDHandle{simtrackerhits_handle, edm4hep::CellIDEncoding, Gaudi::DataHandle::Reader};
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

  MetaDataHandle<std::string> cellIDHandle{calohit_handle, edm4hep::CellIDEncoding, Gaudi::DataHandle::Reader};
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

  MetaDataHandle<std::string> cellIDHandle{sim_calohit_handle, edm4hep::CellIDEncoding, Gaudi::DataHandle::Reader};
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

// Select the appropiate method to convert a collection given its type
void EDM4hep2LcioTool::convertAdd(const std::string& e4h_coll_name, const std::string& lcio_coll_name,
                                  lcio::LCEventImpl* lcio_event, CollectionPairMappings& collection_pairs,
                                  std::vector<EDM4hep2LCIOConv::ParticleIDConvData>& pidCollections) {
  const auto& evtFrame = m_podioDataSvc->getEventFrame();
  const auto& metadata = m_podioDataSvc->getMetaDataFrame();
  const auto  collPtr  = evtFrame.get(e4h_coll_name);
  if (!collPtr) {
    error() << "No collection with name: " << e4h_coll_name << " available for conversion" << endmsg;
  }
  const auto fulltype = collPtr->getValueTypeName();

  debug() << "Converting type " << fulltype << " from input " << e4h_coll_name << endmsg;

  if (fulltype == "edm4hep::Track") {
    convertTracks(collection_pairs.tracks, e4h_coll_name, lcio_coll_name, lcio_event);
  } else if (fulltype == "edm4hep::TrackerHit") {
    convertTrackerHits(collection_pairs.trackerHits, e4h_coll_name, lcio_coll_name, lcio_event);
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
    pidCollections.emplace_back(e4h_coll_name, static_cast<const edm4hep::ParticleIDCollection*>(collPtr),
                                edm4hep::utils::PIDHandler::getAlgoInfo(metadata, e4h_coll_name));
  }

  else if (fulltype == "edm4hep::CaloHitContribution") {
    debug() << "CaloHitContribution collection cannot be converted standalone. "
            << "SimCalorimeterHit collection to be converted in order to be able to attach to them" << endmsg;
  } else {
    warning() << "Error trying to convert requested " << fulltype << " with name " << e4h_coll_name << endmsg;
    warning() << "List of supported types: "
              << "Track, TrackerHit, SimTrackerHit, "
              << "Cluster, CalorimeterHit, RawCalorimeterHit, "
              << "SimCalorimeterHit, Vertex, ReconstructedParticle, "
              << "MCParticle." << endmsg;
  }
}

// Parse property parameters and convert the indicated collections.
// Use the collection names in the parameters to read and write them
StatusCode EDM4hep2LcioTool::convertCollections(lcio::LCEventImpl* lcio_event) {
  const auto& edmEvent    = m_podioDataSvc->getEventFrame();
  const auto  collections = edmEvent.getAvailableCollections();
  // Start off with the pre-defined collection name mappings
  auto collsToConvert{m_collNames.value()};
  // We *always* want to convert the EventHeader
  collsToConvert.emplace(edm4hep::EventHeaderName, "");
  if (m_convertAll) {
    info() << "Converting all collections from EDM4hep to LCIO" << endmsg;
    // And simply add the rest, exploiting the fact that emplace will not
    // replace existing entries with the same key
    for (const auto& name : collections) {
      collsToConvert.emplace(name, name);
    }
  }

  CollectionPairMappings                            collection_pairs{};
  std::vector<EDM4hep2LCIOConv::ParticleIDConvData> pidCollections{};

  for (const auto& [edm4hepName, lcioName] : collsToConvert) {
    debug() << "Converting collection " << edm4hepName << " (storing it as " << lcioName << ")" << endmsg;
    if (!EDM4hep2LCIOConv::collectionExist(lcioName, lcio_event)) {
      convertAdd(edm4hepName, lcioName, lcio_event, collection_pairs, pidCollections);
    } else {
      debug() << " Collection " << lcioName << " already in place, skipping conversion. " << endmsg;
    }
  }
  debug() << "Event: " << lcio_event->getEventNumber() << " Run: " << lcio_event->getRunNumber() << endmsg;

  // Deal with
  EDM4hep2LCIOConv::sortParticleIDs(pidCollections);
  for (const auto& pidCollMeta : pidCollections) {
    const auto algoId = attachParticleIDMetaData(lcio_event, edmEvent, pidCollMeta);
    if (!algoId.has_value()) {
      warning() << "Could not determine algorithm type for ParticleID collection " << pidCollMeta.name
                << " for setting consistent metadata" << endmsg;
    }
    convertParticleIDs(collection_pairs.particleIDs, pidCollMeta.name, algoId.value_or(-1));
  }

  // We want one "global" map that is created the first time it is use in the
  // event.
  //
  // Technically getOrCreate is a thing in GaudiTool but that doesn't seem to
  // easily work with the AnyDataWrapper we want to use here. So doing the two
  // step process here
  if (!exist<GlobalMapWrapper>(GlobalConvertedObjectsMap::TESpath.data())) {
    debug() << "Creating GlobalconvertedObjectsMap for this event since it is not already in the EventStore" << endmsg;
    auto globalObjMapWrapper = std::make_unique<GlobalMapWrapper>(GlobalConvertedObjectsMap{});
    put(std::move(globalObjMapWrapper), GlobalConvertedObjectsMap::TESpath.data());
  }

  auto  globalObjMapWrapper = get<GlobalMapWrapper>(GlobalConvertedObjectsMap::TESpath.data());
  auto& globalObjMap        = globalObjMapWrapper->getData();

  globalObjMap.update(collection_pairs);

  EDM4hep2LCIOConv::resolveRelations(collection_pairs, globalObjMap);

  return StatusCode::SUCCESS;
}
