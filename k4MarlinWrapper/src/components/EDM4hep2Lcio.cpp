#include "k4MarlinWrapper/converters/EDM4hep2Lcio.h"
#include <unordered_map>

DECLARE_COMPONENT(EDM4hep2LcioTool);

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
void EDM4hep2LcioTool::convertTracks(vec_pair<lcio::TrackImpl*, edm4hep::Track>&           tracks_vec,
                                     vec_pair<lcio::TrackerHitImpl*, edm4hep::TrackerHit>& trackerhits_vec,
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
void EDM4hep2LcioTool::convertTrackerHits(vec_pair<lcio::TrackerHitImpl*, edm4hep::TrackerHit>& trackerhits_vec,
                                          const std::string& e4h_coll_name, const std::string& lcio_coll_name,
                                          lcio::LCEventImpl* lcio_event) {
  DataHandle<edm4hep::TrackerHitCollection> trackerhits_handle{e4h_coll_name, Gaudi::DataHandle::Reader, this};
  const auto                                trackerhits_coll = trackerhits_handle.get();

  const auto collID    = trackerhits_coll->getID();
  const auto cellIDstr = trackerhits_handle.getCollMetadataCellID(collID);

  auto* conv_trackerhits = convTrackerHits(trackerhits_coll, cellIDstr, trackerhits_vec);

  // Add all trackerhits to event
  lcio_event->addCollection(conv_trackerhits, lcio_coll_name);
}

// Convert EDM4hep SimTrackerHits to LCIO
// Add converted LCIO ptr and original EDM4hep collection to vector of pairs
// Add LCIO Collection Vector to LCIO event
void EDM4hep2LcioTool::convertSimTrackerHits(
    vec_pair<lcio::SimTrackerHitImpl*, edm4hep::SimTrackerHit>& simtrackerhits_vec,
    const vec_pair<lcio::MCParticleImpl*, edm4hep::MCParticle>& mcparticles_vec, const std::string& e4h_coll_name,
    const std::string& lcio_coll_name, lcio::LCEventImpl* lcio_event) {
  DataHandle<edm4hep::SimTrackerHitCollection> simtrackerhits_handle{e4h_coll_name, Gaudi::DataHandle::Reader, this};
  const auto                                   simtrackerhits_coll = simtrackerhits_handle.get();

  const auto collID    = simtrackerhits_coll->getID();
  const auto cellIDstr = simtrackerhits_handle.getCollMetadataCellID(collID);

  auto* conv_simtrackerhits = convSimTrackerHits(simtrackerhits_coll, cellIDstr, simtrackerhits_vec, mcparticles_vec);

  // Add all simtrackerhits to event
  lcio_event->addCollection(conv_simtrackerhits, lcio_coll_name);
}

// Convert EDM4hep Calorimeter Hits to LCIO
// Add converted LCIO ptr and original EDM4hep collection to vector of pairs
// Add converted LCIO Collection Vector to LCIO event
void EDM4hep2LcioTool::convertCalorimeterHits(
    vec_pair<lcio::CalorimeterHitImpl*, edm4hep::CalorimeterHit>& calo_hits_vec, const std::string& e4h_coll_name,
    const std::string& lcio_coll_name, lcio::LCEventImpl* lcio_event) {
  DataHandle<edm4hep::CalorimeterHitCollection> calohit_handle{e4h_coll_name, Gaudi::DataHandle::Reader, this};
  const auto                                    calohit_coll = calohit_handle.get();

  const auto collID    = calohit_coll->getID();
  const auto cellIDstr = calohit_handle.getCollMetadataCellID(collID);

  auto* conv_calohits = convCalorimeterHits(calohit_coll, cellIDstr, calo_hits_vec);

  // Add all Calorimeter Hits to event
  lcio_event->addCollection(conv_calohits, lcio_coll_name);
}

// Convert EDM4hep RAW Calorimeter Hits to LCIO
// Add converted LCIO ptr and original EDM4hep collection to vector of pairs
// Add converted LCIO Collection Vector to LCIO event
void EDM4hep2LcioTool::convertRawCalorimeterHits(
    vec_pair<lcio::RawCalorimeterHitImpl*, edm4hep::RawCalorimeterHit>& raw_calo_hits_vec,
    const std::string& e4h_coll_name, const std::string& lcio_coll_name, lcio::LCEventImpl* lcio_event) {
  DataHandle<edm4hep::RawCalorimeterHitCollection> raw_calohit_handle{e4h_coll_name, Gaudi::DataHandle::Reader, this};
  const auto                                       rawcalohit_coll = raw_calohit_handle.get();

  auto* conv_rawcalohits = convRawCalorimeterHits(rawcalohit_coll, raw_calo_hits_vec);

  // Add all Raw Calorimeter Hits to event
  lcio_event->addCollection(conv_rawcalohits, lcio_coll_name);
}

// Convert EDM4hep Sim Calorimeter Hits to LCIO
// Add converted LCIO ptr and original EDM4hep collection to vector of pairs
// Add converted LCIO Collection Vector to LCIO event
void EDM4hep2LcioTool::convertSimCalorimeterHits(
    vec_pair<lcio::SimCalorimeterHitImpl*, edm4hep::SimCalorimeterHit>& sim_calo_hits_vec,
    const vec_pair<lcio::MCParticleImpl*, edm4hep::MCParticle>& mcparticles, const std::string& e4h_coll_name,
    const std::string& lcio_coll_name, lcio::LCEventImpl* lcio_event) {
  DataHandle<edm4hep::SimCalorimeterHitCollection> sim_calohit_handle{e4h_coll_name, Gaudi::DataHandle::Reader, this};
  const auto                                       simcalohit_coll = sim_calohit_handle.get();

  auto       collID    = simcalohit_coll->getID();
  const auto cellIDstr = sim_calohit_handle.getCollMetadataCellID(collID);

  // TODO mcparticles_vdc
  auto* conv_simcalohits = convSimCalorimeterHits(simcalohit_coll, cellIDstr, sim_calo_hits_vec, mcparticles);

  // Add all Sim Calorimeter Hits to event
  lcio_event->addCollection(conv_simcalohits, lcio_coll_name);
}

// Convert EDM4hep TPC Hits to LCIO
// Add converted LCIO ptr and original EDM4hep collection to vector of pairs
// Add converted LCIO Collection Vector to LCIO event
void EDM4hep2LcioTool::convertTPCHits(vec_pair<lcio::TPCHitImpl*, edm4hep::RawTimeSeries>& tpc_hits_vec,
                                      const std::string& e4h_coll_name, const std::string& lcio_coll_name,
                                      lcio::LCEventImpl* lcio_event) {
  DataHandle<edm4hep::RawTimeSeriesCollection> tpchit_handle{e4h_coll_name, Gaudi::DataHandle::Reader, this};
  const auto                            tpchit_coll = tpchit_handle.get();

  auto* conv_tpchits = convTPCHits(tpchit_coll, tpc_hits_vec);

  // Add all TPC Hits to event
  lcio_event->addCollection(conv_tpchits, lcio_coll_name);
}

// Convert EDM4hep Clusters to LCIO
// Add converted LCIO ptr and original EDM4hep collection to vector of pairs
// Add converted LCIO Collection Vector to LCIO event
void EDM4hep2LcioTool::convertClusters(vec_pair<lcio::ClusterImpl*, edm4hep::Cluster>&                     cluster_vec,
                                       const vec_pair<lcio::CalorimeterHitImpl*, edm4hep::CalorimeterHit>& calohits_vec,
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
void EDM4hep2LcioTool::convertVertices(
    vec_pair<lcio::VertexImpl*, edm4hep::Vertex>&                                     vertex_vec,
    const vec_pair<lcio::ReconstructedParticleImpl*, edm4hep::ReconstructedParticle>& recoparticles_vec,
    const std::string& e4h_coll_name, const std::string& lcio_coll_name, lcio::LCEventImpl* lcio_event) {
  DataHandle<edm4hep::VertexCollection> vertex_handle{e4h_coll_name, Gaudi::DataHandle::Reader, this};
  const auto                            vertex_coll = vertex_handle.get();

  auto* conv_vertices = convVertices(vertex_coll, vertex_vec, recoparticles_vec);

  // Add all tracks to event
  lcio_event->addCollection(conv_vertices, lcio_coll_name);
}

// Convert MC Particles to LCIO
// Add converted LCIO ptr and original EDM4hep collection to vector of pairs
// Add converted LCIO Collection Vector to LCIO event
void EDM4hep2LcioTool::convertMCParticles(vec_pair<lcio::MCParticleImpl*, edm4hep::MCParticle>& mc_particles_vec,
                                          const std::string& e4h_coll_name, const std::string& lcio_coll_name,
                                          lcio::LCEventImpl* lcio_event) {
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
void EDM4hep2LcioTool::convertReconstructedParticles(
    vec_pair<lcio::ReconstructedParticleImpl*, edm4hep::ReconstructedParticle>& recoparticles_vec,
    const vec_pair<lcio::TrackImpl*, edm4hep::Track>&                           tracks_vec,
    const vec_pair<lcio::VertexImpl*, edm4hep::Vertex>&                         vertex_vec,
    const vec_pair<lcio::ClusterImpl*, edm4hep::Cluster>& clusters_vec, const std::string& e4h_coll_name,
    const std::string& lcio_coll_name, lcio::LCEventImpl* lcio_event) {
  // ReconstructedParticles handle
  DataHandle<edm4hep::ReconstructedParticleCollection> recos_handle{e4h_coll_name, Gaudi::DataHandle::Reader, this};
  const auto                                           recos_coll = recos_handle.get();

  auto* conv_recops = convReconstructedParticles(recos_coll, recoparticles_vec, tracks_vec, vertex_vec, clusters_vec);

  // Add all reconstructed particles to event
  lcio_event->addCollection(conv_recops, lcio_coll_name);
}

// Select the appropiate method to convert a collection given its type
void EDM4hep2LcioTool::convertAdd(const std::string& e4h_coll_name, const std::string& lcio_coll_name,
                                  lcio::LCEventImpl* lcio_event, CollectionsPairVectors& collection_pairs) {
  // Get the associated type to the collection name
  auto evt_store = m_podioDataSvc->getCollections();

  std::string fulltype = "";

  for (auto& [name, coll] : evt_store) {
    if (name == e4h_coll_name) {
      fulltype = coll->getValueTypeName();
      break;
    }
  }

  if (fulltype == "") {
    error() << "Could not get type from collection name: " << e4h_coll_name << endmsg;
    return;
  }

  if (fulltype == "edm4hep::Track") {
    convertTracks(collection_pairs.tracks, collection_pairs.trackerhits, e4h_coll_name, lcio_coll_name, lcio_event);
  } else if (fulltype == "edm4hep::TrackerHit") {
    convertTrackerHits(collection_pairs.trackerhits, e4h_coll_name, lcio_coll_name, lcio_event);
  } else if (fulltype == "edm4hep::SimTrackerHit") {
    convertSimTrackerHits(collection_pairs.simtrackerhits, collection_pairs.mcparticles, e4h_coll_name, lcio_coll_name,
                          lcio_event);
  } else if (fulltype == "edm4hep::CalorimeterHit") {
    convertCalorimeterHits(collection_pairs.calohits, e4h_coll_name, lcio_coll_name, lcio_event);
  } else if (fulltype == "edm4hep::RawCalorimeterHit") {
    convertRawCalorimeterHits(collection_pairs.rawcalohits, e4h_coll_name, lcio_coll_name, lcio_event);
  } else if (fulltype == "edm4hep::SimCalorimeterHit") {
    convertSimCalorimeterHits(collection_pairs.simcalohits, collection_pairs.mcparticles, e4h_coll_name, lcio_coll_name,
                              lcio_event);
  } else if (fulltype == "edm4hep::RawTimeSeries") {
    convertTPCHits(collection_pairs.tpchits, e4h_coll_name, lcio_coll_name, lcio_event);
  } else if (fulltype == "edm4hep::Cluster") {
    convertClusters(collection_pairs.clusters, collection_pairs.calohits, e4h_coll_name, lcio_coll_name, lcio_event);
  } else if (fulltype == "edm4hep::Vertex") {
    convertVertices(collection_pairs.vertices, collection_pairs.recoparticles, e4h_coll_name, lcio_coll_name,
                    lcio_event);
  } else if (fulltype == "edm4hep::MCParticle") {
    convertMCParticles(collection_pairs.mcparticles, e4h_coll_name, lcio_coll_name, lcio_event);
  } else if (fulltype == "edm4hep::ReconstructedParticle") {
    convertReconstructedParticles(collection_pairs.recoparticles, collection_pairs.tracks, collection_pairs.vertices,
                                  collection_pairs.clusters, e4h_coll_name, lcio_coll_name, lcio_event);
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
  const auto collections = m_podioDataSvc->getCollections();

  // Start off with the pre-defined collection name mappings
  auto collsToConvert{m_collNames.value()};
  if (m_convertAll) {
    info() << "Converting all collections from EDM4hep to LCIO" << endmsg;
    // And simply add the rest, exploiting the fact that emplace will not
    // replace existing entries with the same key
    for (const auto& [name, _] : collections) {
      collsToConvert.emplace(name, name);
    }
  }

  CollectionsPairVectors collection_pairs{};
  for (const auto& [edm4hepName, lcioName] : collsToConvert) {
    if (!collectionExist(lcioName, lcio_event)) {
      convertAdd(edm4hepName, lcioName, lcio_event, collection_pairs);
    } else {
      debug() << " Collection " << lcioName << " already in place, skipping conversion. " << endmsg;
    }
  }

  FillMissingCollections(collection_pairs);

  return StatusCode::SUCCESS;
}
