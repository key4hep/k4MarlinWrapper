/*
 * Copyright (c) 2019-2023 Key4hep-Project.
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

#include "k4FWCore/DataHandle.h"
#include "k4FWCore/MetaDataHandle.h"

DECLARE_COMPONENT(EDM4hep2LcioTool);

#ifdef EDM4HEP2LCIOCONV_NAMESPACE
using namespace EDM4hep2LCIOConv;
#endif

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
void EDM4hep2LcioTool::convertTracks(TrackMap& tracks_vec, const TrackerHitMap& trackerhits_vec,
                                     const std::string& e4h_coll_name, const std::string& lcio_coll_name,
                                     lcio::LCEventImpl* lcio_event) {
  DataHandle<edm4hep::TrackCollection> tracks_handle{e4h_coll_name, Gaudi::DataHandle::Reader, this};
  const auto*                          tracks_coll = tracks_handle.get();

  auto* conv_tracks = convTracks(tracks_coll, tracks_vec, trackerhits_vec);

  // Add all tracks to event
  lcio_event->addCollection(conv_tracks, lcio_coll_name);
}

// Convert EDM4hep TrackerHits to LCIO
// Add converted LCIO ptr and original EDM4hep collection to vector of pairs
// Add LCIO Collection Vector to LCIO event
void EDM4hep2LcioTool::convertTrackerHits(TrackerHitMap& trackerhits_vec, const std::string& e4h_coll_name,
                                          const std::string& lcio_coll_name, lcio::LCEventImpl* lcio_event) {
  DataHandle<edm4hep::TrackerHitCollection> trackerhits_handle{e4h_coll_name, Gaudi::DataHandle::Reader, this};
  const auto                                trackerhits_coll = trackerhits_handle.get();

  MetaDataHandle<std::string> cellIDStrHandle{trackerhits_handle, "CellIDEncoding", Gaudi::DataHandle::Reader};

  auto* conv_trackerhits = convTrackerHits(trackerhits_coll, cellIDStrHandle.get(), trackerhits_vec);

  // Add all trackerhits to event
  lcio_event->addCollection(conv_trackerhits, lcio_coll_name);
}

// Convert EDM4hep SimTrackerHits to LCIO
// Add converted LCIO ptr and original EDM4hep collection to vector of pairs
// Add LCIO Collection Vector to LCIO event
void EDM4hep2LcioTool::convertSimTrackerHits(SimTrackerHitMap& simtrackerhits_vec, const MCParticleMap& mcparticles_vec,
                                             const std::string& e4h_coll_name, const std::string& lcio_coll_name,
                                             lcio::LCEventImpl* lcio_event) {
  DataHandle<edm4hep::SimTrackerHitCollection> simtrackerhits_handle{e4h_coll_name, Gaudi::DataHandle::Reader, this};
  const auto                                   simtrackerhits_coll = simtrackerhits_handle.get();

  MetaDataHandle<std::string> cellIDHandle{simtrackerhits_handle, "CellIDEncoding", Gaudi::DataHandle::Reader};
  const auto                  cellIDstr = cellIDHandle.get();

  auto* conv_simtrackerhits = convSimTrackerHits(simtrackerhits_coll, cellIDstr, simtrackerhits_vec, mcparticles_vec);

  // Add all simtrackerhits to event
  lcio_event->addCollection(conv_simtrackerhits, lcio_coll_name);
}

// Convert EDM4hep Calorimeter Hits to LCIO
// Add converted LCIO ptr and original EDM4hep collection to vector of pairs
// Add converted LCIO Collection Vector to LCIO event
void EDM4hep2LcioTool::convertCalorimeterHits(CaloHitMap& calo_hits_vec, const std::string& e4h_coll_name,
                                              const std::string& lcio_coll_name, lcio::LCEventImpl* lcio_event) {
  DataHandle<edm4hep::CalorimeterHitCollection> calohit_handle{e4h_coll_name, Gaudi::DataHandle::Reader, this};
  const auto                                    calohit_coll = calohit_handle.get();

  MetaDataHandle<std::string> cellIDHandle{calohit_handle, "CellIDEncoding", Gaudi::DataHandle::Reader};
  const auto                  cellIDstr = cellIDHandle.get();

  auto* conv_calohits = convCalorimeterHits(calohit_coll, cellIDstr, calo_hits_vec);

  // Add all Calorimeter Hits to event
  lcio_event->addCollection(conv_calohits, lcio_coll_name);
}

// Convert EDM4hep RAW Calorimeter Hits to LCIO
// Add converted LCIO ptr and original EDM4hep collection to vector of pairs
// Add converted LCIO Collection Vector to LCIO event
void EDM4hep2LcioTool::convertRawCalorimeterHits(RawCaloHitMap& raw_calo_hits_vec, const std::string& e4h_coll_name,
                                                 const std::string& lcio_coll_name, lcio::LCEventImpl* lcio_event) {
  DataHandle<edm4hep::RawCalorimeterHitCollection> raw_calohit_handle{e4h_coll_name, Gaudi::DataHandle::Reader, this};
  const auto                                       rawcalohit_coll = raw_calohit_handle.get();

  auto* conv_rawcalohits = convRawCalorimeterHits(rawcalohit_coll, raw_calo_hits_vec);

  // Add all Raw Calorimeter Hits to event
  lcio_event->addCollection(conv_rawcalohits, lcio_coll_name);
}

// Convert EDM4hep Sim Calorimeter Hits to LCIO
// Add converted LCIO ptr and original EDM4hep collection to vector of pairs
// Add converted LCIO Collection Vector to LCIO event
void EDM4hep2LcioTool::convertSimCalorimeterHits(SimCaloHitMap& sim_calo_hits_vec, const MCParticleMap& mcparticles,
                                                 const std::string& e4h_coll_name, const std::string& lcio_coll_name,
                                                 lcio::LCEventImpl* lcio_event) {
  DataHandle<edm4hep::SimCalorimeterHitCollection> sim_calohit_handle{e4h_coll_name, Gaudi::DataHandle::Reader, this};
  const auto                                       simcalohit_coll = sim_calohit_handle.get();

  MetaDataHandle<std::string> cellIDHandle{sim_calohit_handle, "CellIDEncoding", Gaudi::DataHandle::Reader};
  const auto                  cellIDstr = cellIDHandle.get();

  // TODO mcparticles_vdc
  auto* conv_simcalohits = convSimCalorimeterHits(simcalohit_coll, cellIDstr, sim_calo_hits_vec, mcparticles);

  // Add all Sim Calorimeter Hits to event
  lcio_event->addCollection(conv_simcalohits, lcio_coll_name);
}

// Convert EDM4hep TPC Hits to LCIO
// Add converted LCIO ptr and original EDM4hep collection to vector of pairs
// Add converted LCIO Collection Vector to LCIO event
void EDM4hep2LcioTool::convertTPCHits(TPCHitMap& tpc_hits_vec, const std::string& e4h_coll_name,
                                      const std::string& lcio_coll_name, lcio::LCEventImpl* lcio_event) {
  DataHandle<edm4hep::RawTimeSeriesCollection> tpchit_handle{e4h_coll_name, Gaudi::DataHandle::Reader, this};
  const auto                                   tpchit_coll = tpchit_handle.get();

  auto* conv_tpchits = convTPCHits(tpchit_coll, tpc_hits_vec);

  // Add all TPC Hits to event
  lcio_event->addCollection(conv_tpchits, lcio_coll_name);
}

// Convert EDM4hep Clusters to LCIO
// Add converted LCIO ptr and original EDM4hep collection to vector of pairs
// Add converted LCIO Collection Vector to LCIO event
void EDM4hep2LcioTool::convertClusters(ClusterMap& cluster_vec, const CaloHitMap& calohits_vec,
                                       const std::string& e4h_coll_name, const std::string& lcio_coll_name,
                                       lcio::LCEventImpl* lcio_event) {
  DataHandle<edm4hep::ClusterCollection> cluster_handle{e4h_coll_name, Gaudi::DataHandle::Reader, this};
  const auto                             cluster_coll = cluster_handle.get();

  auto* conv_clusters = convClusters(cluster_coll, cluster_vec, calohits_vec);

  // Add clusters to event
  lcio_event->addCollection(conv_clusters, lcio_coll_name);
}

// Convert EDM4hep Vertices to LCIO
// Add converted LCIO ptr and original EDM4hep collection to vector of pairs
// Add converted LCIO Collection Vector to LCIO event
void EDM4hep2LcioTool::convertVertices(VertexMap& vertex_vec, const RecoParticleMap& recoparticles_vec,
                                       const std::string& e4h_coll_name, const std::string& lcio_coll_name,
                                       lcio::LCEventImpl* lcio_event) {
  DataHandle<edm4hep::VertexCollection> vertex_handle{e4h_coll_name, Gaudi::DataHandle::Reader, this};
  const auto                            vertex_coll = vertex_handle.get();

  auto* conv_vertices = convVertices(vertex_coll, vertex_vec, recoparticles_vec);

  // Add all tracks to event
  lcio_event->addCollection(conv_vertices, lcio_coll_name);
}

// Convert MC Particles to LCIO
// Add converted LCIO ptr and original EDM4hep collection to vector of pairs
// Add converted LCIO Collection Vector to LCIO event
void EDM4hep2LcioTool::convertMCParticles(MCParticleMap& mc_particles_vec, const std::string& e4h_coll_name,
                                          const std::string& lcio_coll_name, lcio::LCEventImpl* lcio_event) {
  // MCParticles handle
  DataHandle<edm4hep::MCParticleCollection> mcparticle_handle{e4h_coll_name, Gaudi::DataHandle::Reader, this};
  const auto                                mcparticle_coll = mcparticle_handle.get();

  auto* conv_mcparticles = convMCParticles(mcparticle_coll, mc_particles_vec);

  // Add all reconstructed particles to event
  lcio_event->addCollection(conv_mcparticles, lcio_coll_name);
}

// Convert EDM4hep RecoParticles to LCIO
// Add converted LCIO ptr and original EDM4hep collection to vector of pairs
// Add converted LCIO Collection Vector to LCIO event
void EDM4hep2LcioTool::convertReconstructedParticles(RecoParticleMap& recoparticles_vec, const TrackMap& tracks_vec,
                                                     const VertexMap& vertex_vec, const ClusterMap& clusters_vec,
                                                     const std::string& e4h_coll_name,
                                                     const std::string& lcio_coll_name, lcio::LCEventImpl* lcio_event) {
  // ReconstructedParticles handle
  DataHandle<edm4hep::ReconstructedParticleCollection> recos_handle{e4h_coll_name, Gaudi::DataHandle::Reader, this};
  const auto                                           recos_coll = recos_handle.get();

  auto* conv_recops = convReconstructedParticles(recos_coll, recoparticles_vec, tracks_vec, vertex_vec, clusters_vec);

  // Add all reconstructed particles to event
  lcio_event->addCollection(conv_recops, lcio_coll_name);
}

// Transfer info from EDM4hep EventHeader to LCIO event
void EDM4hep2LcioTool::convertEventHeader(const std::string& e4h_coll_name, lcio::LCEventImpl* lcio_event) {
  DataHandle<edm4hep::EventHeaderCollection> header_handle{e4h_coll_name, Gaudi::DataHandle::Reader, this};
  const auto                                 header_coll = header_handle.get();

  if (header_coll->size() != 1) {
    error() << "Header collection contains " << header_coll->size() << " headers, expected 1." << endmsg;
    return;
  }
  convEventHeader(header_coll, lcio_event);
}

// Select the appropiate method to convert a collection given its type
void EDM4hep2LcioTool::convertAdd(const std::string& e4h_coll_name, const std::string& lcio_coll_name,
                                  lcio::LCEventImpl* lcio_event, CollectionPairMappings& collection_pairs) {
  const auto& evtFrame = m_podioDataSvc->getEventFrame();
  const auto  collPtr  = evtFrame.get(e4h_coll_name);
  if (!collPtr) {
    error() << "No collection with name: " << e4h_coll_name << " available for conversion" << endmsg;
  }
  const auto fulltype = collPtr->getValueTypeName();

  if (fulltype == "edm4hep::Track") {
    convertTracks(collection_pairs.tracks, collection_pairs.trackerHits, e4h_coll_name, lcio_coll_name, lcio_event);
  } else if (fulltype == "edm4hep::TrackerHit") {
    convertTrackerHits(collection_pairs.trackerHits, e4h_coll_name, lcio_coll_name, lcio_event);
  } else if (fulltype == "edm4hep::SimTrackerHit") {
    convertSimTrackerHits(collection_pairs.simTrackerHits, collection_pairs.mcParticles, e4h_coll_name, lcio_coll_name,
                          lcio_event);
  } else if (fulltype == "edm4hep::CalorimeterHit") {
    convertCalorimeterHits(collection_pairs.caloHits, e4h_coll_name, lcio_coll_name, lcio_event);
  } else if (fulltype == "edm4hep::RawCalorimeterHit") {
    convertRawCalorimeterHits(collection_pairs.rawCaloHits, e4h_coll_name, lcio_coll_name, lcio_event);
  } else if (fulltype == "edm4hep::SimCalorimeterHit") {
    convertSimCalorimeterHits(collection_pairs.simCaloHits, collection_pairs.mcParticles, e4h_coll_name, lcio_coll_name,
                              lcio_event);
  } else if (fulltype == "edm4hep::RawTimeSeries") {
    convertTPCHits(collection_pairs.tpcHits, e4h_coll_name, lcio_coll_name, lcio_event);
  } else if (fulltype == "edm4hep::Cluster") {
    convertClusters(collection_pairs.clusters, collection_pairs.caloHits, e4h_coll_name, lcio_coll_name, lcio_event);
  } else if (fulltype == "edm4hep::Vertex") {
    convertVertices(collection_pairs.vertices, collection_pairs.recoParticles, e4h_coll_name, lcio_coll_name,
                    lcio_event);
  } else if (fulltype == "edm4hep::MCParticle") {
    convertMCParticles(collection_pairs.mcParticles, e4h_coll_name, lcio_coll_name, lcio_event);
  } else if (fulltype == "edm4hep::ReconstructedParticle") {
    convertReconstructedParticles(collection_pairs.recoParticles, collection_pairs.tracks, collection_pairs.vertices,
                                  collection_pairs.clusters, e4h_coll_name, lcio_coll_name, lcio_event);
  } else if (fulltype == "edm4hep::EventHeader") {
    convertEventHeader(e4h_coll_name, lcio_event);
  } else if (fulltype == "edm4hep::CaloHitContribution") {
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
  const auto collections = m_podioDataSvc->getEventFrame().getAvailableCollections();
  // Start off with the pre-defined collection name mappings
  auto collsToConvert{m_collNames.value()};
  if (m_convertAll) {
    info() << "Converting all collections from EDM4hep to LCIO" << endmsg;
    // And simply add the rest, exploiting the fact that emplace will not
    // replace existing entries with the same key
    for (const auto& name : collections) {
      collsToConvert.emplace(name, name);
    }
  }

  CollectionPairMappings collection_pairs{};
  for (const auto& [edm4hepName, lcioName] : collsToConvert) {
    if (!collectionExist(lcioName, lcio_event)) {
      convertAdd(edm4hepName, lcioName, lcio_event, collection_pairs);
    } else {
      debug() << " Collection " << lcioName << " already in place, skipping conversion. " << endmsg;
    }
  }

  debug() << "Event: " << lcio_event->getEventNumber() << " Run: " << lcio_event->getRunNumber() << endmsg;

  k4MarlinWrapper::GlobalConvertedObjectsMap::update(collection_pairs);

  FillMissingCollections(collection_pairs, k4MarlinWrapper::GlobalConvertedObjectsMap::get());

  return StatusCode::SUCCESS;
}
