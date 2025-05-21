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
#include "StoreUtils.h"
#include <GaudiKernel/StatusCode.h>

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

#include <fmt/format.h>
#include <fmt/ranges.h>

#include <functional>
#include <memory>

DECLARE_COMPONENT(EDM4hep2LcioTool);

using namespace k4MarlinWrapper;

struct CollectionPairMappings {
  TrackMap tracks{};
  TrackerHitMap trackerHits{};
  TrackerHitPlaneMap trackerHitPlanes{};
  SimTrackerHitMap simTrackerHits{};
  CaloHitMap caloHits{};
  RawCaloHitMap rawCaloHits{};
  SimCaloHitMap simCaloHits{};
  TPCHitMap tpcHits{};
  ClusterMap clusters{};
  VertexMap vertices{};
  RecoParticleMap recoParticles{};
  MCParticleMap mcParticles{};
  ParticleIDMap particleIDs{};
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

  m_collFromObjSvc = service("CollectionFromObjectSvc", false);
  if (!m_collFromObjSvc) {
    error() << "Could not retrieve CollectionFromObjectSvc" << endmsg;
    return StatusCode::FAILURE;
  }

  return AlgTool::initialize();
}

StatusCode EDM4hep2LcioTool::finalize() { return AlgTool::finalize(); }

// Convert EDM4hep Tracks to LCIO
// Add converted LCIO ptr and original EDM4hep collection to vector of pairs
// Add LCIO Collection Vector to LCIO event
void EDM4hep2LcioTool::convertTracks(TrackMap& tracks_vec, const std::string& e4h_coll_name,
                                     const std::string& lcio_coll_name, lcio::LCEventImpl* lcio_event) {
  k4FWCore::DataHandle<edm4hep::TrackCollection> tracks_handle{e4h_coll_name, Gaudi::DataHandle::Reader, this};
  const auto* tracks_coll = tracks_handle.get();

  auto conv_tracks = EDM4hep2LCIOConv::convertTracks(tracks_coll, tracks_vec);

  // Add all tracks to event
  lcio_event->addCollection(conv_tracks.release(), lcio_coll_name);
}

// Convert EDM4hep TrackerHits to LCIO
// Add converted LCIO ptr and original EDM4hep collection to vector of pairs
// Add LCIO Collection Vector to LCIO event
void EDM4hep2LcioTool::convertTrackerHits(TrackerHitMap& trackerhits_vec, const std::string& e4h_coll_name,
                                          const std::string& lcio_coll_name, lcio::LCEventImpl* lcio_event) {
  k4FWCore::DataHandle<edm4hep::TrackerHit3DCollection> trackerhits_handle{e4h_coll_name, Gaudi::DataHandle::Reader,
                                                                           this};
  const auto trackerhits_coll = trackerhits_handle.get();

  k4FWCore::MetaDataHandle<std::string> cellIDStrHandle{trackerhits_handle, edm4hep::labels::CellIDEncoding,
                                                        Gaudi::DataHandle::Reader};

  auto conv_trackerhits =
      EDM4hep2LCIOConv::convertTrackerHits(trackerhits_coll, cellIDStrHandle.get(""), trackerhits_vec);

  // Add all trackerhits to event
  lcio_event->addCollection(conv_trackerhits.release(), lcio_coll_name);
}

void EDM4hep2LcioTool::convertParticleIDs(ParticleIDMap& pidMap, const std::string& e4h_coll_name, int32_t algoId) {
  k4FWCore::DataHandle<edm4hep::ParticleIDCollection> pidHandle{e4h_coll_name, Gaudi::DataHandle::Reader, this};

  EDM4hep2LCIOConv::convertParticleIDs(pidHandle.get(), pidMap, algoId);
}

void EDM4hep2LcioTool::convertTrackerHitPlanes(TrackerHitPlaneMap& trackerhits_vec, const std::string& e4h_coll_name,
                                               const std::string& lcio_coll_name, lcio::LCEventImpl* lcio_event) {
  k4FWCore::DataHandle<edm4hep::TrackerHitPlaneCollection> trackerhits_handle{e4h_coll_name, Gaudi::DataHandle::Reader,
                                                                              this};
  const auto trackerhits_coll = trackerhits_handle.get();

  k4FWCore::MetaDataHandle<std::string> cellIDStrHandle{trackerhits_handle, edm4hep::labels::CellIDEncoding,
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
  k4FWCore::DataHandle<edm4hep::SimTrackerHitCollection> simtrackerhits_handle{e4h_coll_name, Gaudi::DataHandle::Reader,
                                                                               this};
  const auto simtrackerhits_coll = simtrackerhits_handle.get();

  k4FWCore::MetaDataHandle<std::string> cellIDHandle{simtrackerhits_handle, edm4hep::labels::CellIDEncoding,
                                                     Gaudi::DataHandle::Reader};
  const auto cellIDstr = cellIDHandle.get();

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
  k4FWCore::DataHandle<edm4hep::CalorimeterHitCollection> calohit_handle{e4h_coll_name, Gaudi::DataHandle::Reader,
                                                                         this};
  const auto calohit_coll = calohit_handle.get();

  k4FWCore::MetaDataHandle<std::string> cellIDHandle{calohit_handle, edm4hep::labels::CellIDEncoding,
                                                     Gaudi::DataHandle::Reader};
  const auto cellIDstr = cellIDHandle.get();

  auto conv_calohits = EDM4hep2LCIOConv::convertCalorimeterHits(calohit_coll, cellIDstr, calo_hits_vec);

  // Add all Calorimeter Hits to event
  lcio_event->addCollection(conv_calohits.release(), lcio_coll_name);
}

// Convert EDM4hep RAW Calorimeter Hits to LCIO
// Add converted LCIO ptr and original EDM4hep collection to vector of pairs
// Add converted LCIO Collection Vector to LCIO event
void EDM4hep2LcioTool::convertRawCalorimeterHits(RawCaloHitMap& raw_calo_hits_vec, const std::string& e4h_coll_name,
                                                 const std::string& lcio_coll_name, lcio::LCEventImpl* lcio_event) {
  k4FWCore::DataHandle<edm4hep::RawCalorimeterHitCollection> raw_calohit_handle{e4h_coll_name,
                                                                                Gaudi::DataHandle::Reader, this};
  const auto rawcalohit_coll = raw_calohit_handle.get();

  auto conv_rawcalohits = EDM4hep2LCIOConv::convertRawCalorimeterHits(rawcalohit_coll, raw_calo_hits_vec);

  // Add all Raw Calorimeter Hits to event
  lcio_event->addCollection(conv_rawcalohits.release(), lcio_coll_name);
}

// Convert EDM4hep Sim Calorimeter Hits to LCIO
// Add converted LCIO ptr and original EDM4hep collection to vector of pairs
// Add converted LCIO Collection Vector to LCIO event
void EDM4hep2LcioTool::convertSimCalorimeterHits(SimCaloHitMap& sim_calo_hits_vec, const std::string& e4h_coll_name,
                                                 const std::string& lcio_coll_name, lcio::LCEventImpl* lcio_event) {
  k4FWCore::DataHandle<edm4hep::SimCalorimeterHitCollection> sim_calohit_handle{e4h_coll_name,
                                                                                Gaudi::DataHandle::Reader, this};
  const auto simcalohit_coll = sim_calohit_handle.get();

  k4FWCore::MetaDataHandle<std::string> cellIDHandle{sim_calohit_handle, edm4hep::labels::CellIDEncoding,
                                                     Gaudi::DataHandle::Reader};
  const auto cellIDstr = cellIDHandle.get();

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
  k4FWCore::DataHandle<edm4hep::RawTimeSeriesCollection> tpchit_handle{e4h_coll_name, Gaudi::DataHandle::Reader, this};
  const auto tpchit_coll = tpchit_handle.get();

  auto conv_tpchits = EDM4hep2LCIOConv::convertTPCHits(tpchit_coll, tpc_hits_vec);

  // Add all TPC Hits to event
  lcio_event->addCollection(conv_tpchits.release(), lcio_coll_name);
}

// Convert EDM4hep Clusters to LCIO
// Add converted LCIO ptr and original EDM4hep collection to vector of pairs
// Add converted LCIO Collection Vector to LCIO event
void EDM4hep2LcioTool::convertClusters(ClusterMap& cluster_vec, const std::string& e4h_coll_name,
                                       const std::string& lcio_coll_name, lcio::LCEventImpl* lcio_event) {
  k4FWCore::DataHandle<edm4hep::ClusterCollection> cluster_handle{e4h_coll_name, Gaudi::DataHandle::Reader, this};
  const auto cluster_coll = cluster_handle.get();

  auto conv_clusters = EDM4hep2LCIOConv::convertClusters(cluster_coll, cluster_vec);

  // Add clusters to event
  lcio_event->addCollection(conv_clusters.release(), lcio_coll_name);
}

// Convert EDM4hep Vertices to LCIO
// Add converted LCIO ptr and original EDM4hep collection to vector of pairs
// Add converted LCIO Collection Vector to LCIO event
void EDM4hep2LcioTool::convertVertices(VertexMap& vertex_vec, const std::string& e4h_coll_name,
                                       const std::string& lcio_coll_name, lcio::LCEventImpl* lcio_event) {
  k4FWCore::DataHandle<edm4hep::VertexCollection> vertex_handle{e4h_coll_name, Gaudi::DataHandle::Reader, this};
  const auto vertex_coll = vertex_handle.get();

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
  k4FWCore::DataHandle<edm4hep::MCParticleCollection> mcparticle_handle{e4h_coll_name, Gaudi::DataHandle::Reader, this};
  const auto mcparticle_coll = mcparticle_handle.get();

  auto conv_mcparticles = EDM4hep2LCIOConv::convertMCParticles(mcparticle_coll, mc_particles_vec);

  // Add all reconstructed particles to event
  lcio_event->addCollection(conv_mcparticles.release(), lcio_coll_name);
}

// Convert EDM4hep RecoParticles to LCIO
// Add converted LCIO ptr and original EDM4hep collection to vector of pairs
// Add converted LCIO Collection Vector to LCIO event
void EDM4hep2LcioTool::convertReconstructedParticles(RecoParticleMap& recoparticles_vec,
                                                     const std::string& e4h_coll_name,
                                                     const std::string& lcio_coll_name, lcio::LCEventImpl* lcio_event) {
  // ReconstructedParticles handle
  k4FWCore::DataHandle<edm4hep::ReconstructedParticleCollection> recos_handle{e4h_coll_name, Gaudi::DataHandle::Reader,
                                                                              this};
  const auto recos_coll = recos_handle.get();

  auto conv_recops = EDM4hep2LCIOConv::convertReconstructedParticles(recos_coll, recoparticles_vec);

  // Add all reconstructed particles to event
  lcio_event->addCollection(conv_recops.release(), lcio_coll_name);
}

// Transfer info from EDM4hep EventHeader to LCIO event
void EDM4hep2LcioTool::convertEventHeader(const std::string& e4h_coll_name, lcio::LCEventImpl* lcio_event) {
  k4FWCore::DataHandle<edm4hep::EventHeaderCollection> header_handle{e4h_coll_name, Gaudi::DataHandle::Reader, this};
  const auto header_coll = header_handle.get();

  if (header_coll->size() != 1) {
    error() << "Header collection contains " << header_coll->size() << " headers, expected 1." << endmsg;
    return;
  }
  EDM4hep2LCIOConv::convertEventHeader(header_coll, lcio_event);
}

podio::CollectionBase* EDM4hep2LcioTool::getEDM4hepCollection(const std::string& collName) const {
  DataObject* p;
  auto sc = m_eventDataSvc->retrieveObject(collName, p);
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
                                  std::vector<EDM4hep2LCIOConv::TrackDqdxConvData>& dQdxCollections) {
  const auto& metadata = m_podioDataSvc->getMetaDataFrame();
  const auto collPtr = getEDM4hepCollection(e4h_coll_name);
  const auto fulltype = collPtr->getValueTypeName();

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
    warning() << "List of supported types: " << "Track, TrackerHit3D, TrackerHitPlane, SimTrackerHit, "
              << "Cluster, CalorimeterHit, RawCalorimeterHit, " << "SimCalorimeterHit, Vertex, ReconstructedParticle, "
              << "MCParticle." << endmsg;
  }
}

const podio::Frame& EDM4hep2LcioTool::getEDM4hepEvent() const {
  debug() << "Retrieving EDM4hep event (Frame)" << endmsg;
  if (m_podioDataSvc) {
    debug() << "Getting it from PodioDataSvc" << endmsg;
    return m_podioDataSvc->getEventFrame();
  } else {
    debug() << "Trying to get it from TES" << endmsg;
    DataObject* p;
    StatusCode code = m_eventDataSvc->retrieveObject("/Event" + k4FWCore::frameLocation, p);
    if (code.isSuccess()) {
      auto* frame = dynamic_cast<AnyDataWrapper<podio::Frame>*>(p);
      return frame->getData();
    }
  }

  // We can do this because the following assumptions are true:
  // - We only end up here if we are using the IOSvc and we are NOT reading
  //   EDM4hep data. Otherwise the Reader will be scheduled as FIRST algorithm,
  //   most importantly BEFORE any of the wrapped Marlin processors to which
  //   this converter is attached.
  // - The empty Frame we introduce into the TES here does not interfere with
  //   the Writer for EDM4hep output (which is always scheduled last), as that
  //   will simply get this Frame instead of creating an empty one itself
  // - There are no scheduling issues / race conditions, since the
  //   MarlinProcessorWrapper algorithm is not re-entrant and can thus not be
  //   run in parallel
  debug() << "Could not retrieve Frame from expected location. Registering a new empty Frame into the TES" << endmsg;
  auto tmp = new AnyDataWrapper<podio::Frame>(podio::Frame());
  if (m_eventDataSvc->registerObject("/Event" + k4FWCore::frameLocation, tmp).isFailure()) {
    error() << "Could not retrieve Frame from expected location in TES and could not register a new one" << endmsg;
    throw std::runtime_error("Could not get EDM4hep event (Frame) for conversions");
  }
  return tmp->getData();
}

// Parse property parameters and convert the indicated collections.
// Use the collection names in the parameters to read and write them
StatusCode EDM4hep2LcioTool::convertCollections(lcio::LCEventImpl* lcio_event) {
  const auto& edmEvent = getEDM4hepEvent();
  // use m_collsToConvert to detect whether we run the first time and cache the
  // results as we can assume that all the events have the same contents
  if (m_collsToConvert.empty()) {
    // Start off with the pre-defined collection name mappings. We start with a
    // copy of the configuration to not change that on the fly
    auto collNameMapping = m_collNames.value();

    // We *always* want to convert the EventHeader
    m_collsToConvert.emplace(edm4hep::labels::EventHeader, "<directly into LCEvent>");

    if (m_convertAll) {
      info() << "Converting all collections from EDM4hep to LCIO" << endmsg;
      if (m_podioDataSvc) {
        // If we have the PodioDataSvc get the collections available from frame
        for (const auto& name : edmEvent.getAvailableCollections()) {
          const auto& [_, inserted] = collNameMapping.emplace(name, name);
          debug() << fmt::format("Adding '{}' from Frame to conversion? {}", name, inserted) << endmsg;
        }
      }
      // Always check the contents of the TES because algorithms that do not use
      // the PodioDataSvc (e.g. all Functional ones) go to the TES directly and
      // the PodioDataSvc Frame doesn't now about them.
      std::optional<std::map<uint32_t, std::string>> idToNameOpt(std::move(m_idToName));
      for (const auto& name : getAvailableCollectionsFromStore(this, idToNameOpt)) {
        const auto& [_, inserted] = collNameMapping.emplace(name, name);
        debug() << fmt::format("Adding '{}' from TES to conversion? {}", name, inserted) << endmsg;
      }
      m_idToName = std::move(idToNameOpt.value());

      for (auto&& [origName, newName] : collNameMapping) {
        m_collsToConvert.emplace(std::move(origName), std::move(newName));
      }
    }
  }

  CollectionPairMappings collection_pairs{};
  std::vector<EDM4hep2LCIOConv::ParticleIDConvData> pidCollections{};
  std::vector<EDM4hep2LCIOConv::TrackDqdxConvData> dQdxCollections{};

  std::vector<std::tuple<std::string, const podio::CollectionBase*>> linkCollections{};

  for (const auto& [edm4hepName, lcioName] : m_collsToConvert) {
    debug() << "Converting collection " << edm4hepName << " (storing it as " << lcioName << ")" << endmsg;
    if (!EDM4hep2LCIOConv::collectionExist(lcioName, lcio_event)) {
      const auto coll = getEDM4hepCollection(edm4hepName);
      if (coll->getTypeName().find("LinkCollection") != std::string_view::npos) {
        debug() << edm4hepName << " is a link collection, converting it later" << endmsg;
        linkCollections.emplace_back(lcioName, coll);
        continue;
      }
      convertAdd(edm4hepName, lcioName, lcio_event, collection_pairs, pidCollections, dQdxCollections);
    } else {
      debug() << " Collection " << lcioName << " already in place, skipping conversion. " << endmsg;
    }
  }
  debug() << "Event: " << lcio_event->getEventNumber() << " Run: " << lcio_event->getRunNumber() << endmsg;

  EDM4hep2LCIOConv::sortParticleIDs(pidCollections);
  for (const auto& pidCollMeta : pidCollections) {
    // We first have to figure out how the corresponding reco collection is
    // named in EDM4hep. With that we can map it to the correct LCIO name and
    // then attach the corresponding meta information via the PIDHandler
    const auto& [pidCollName, pidColl, pidMetaInfo] = pidCollMeta;
    debug() << fmt::format(
                   "Attaching PID meta information for ParticleID collection {}, PID meta information available? {}",
                   pidCollName, pidMetaInfo.has_value())
            << endmsg;
    if (msgLevel(MSG::DEBUG) && pidMetaInfo.has_value()) {
      debug() << fmt::format("PID meta information: algoName: {}, algoType: {}, paramNames: {}", pidMetaInfo->algoName,
                             pidMetaInfo->algoType(), pidMetaInfo->paramNames)
              << endmsg;
    }
    if (!pidMetaInfo.has_value()) {
      info() << "Could not collect PID meta information for ParticleID collection " << pidCollName << endmsg;
    }
    const auto recoCollName = [&]() {
      auto name = m_collFromObjSvc->getCollectionNameFor((*pidColl)[0].getParticle());
      if (!name.has_value()) {
        // If we can't get a name from the TES we try again via the PodioDataSvc
        if (m_podioDataSvc) {
          name = edmEvent.value().get().getName((*pidColl)[0].getParticle());
        }
      }
      return name;
    }();
    if (!recoCollName.has_value()) {
      warning() << "Could not determine the name of the (LCIO) ReconstructedParticle collection to attach ParticleID "
                   "metadata for (EDM4hep) ParticleID collection "
                << pidCollName << endmsg;
    }

    std::optional<int32_t> algoId{std::nullopt};
    if (recoCollName.has_value()) {
      debug() << "Corresponding ReconstructedParticle (EDM4hep) collection is " << recoCollName.value() << endmsg;
      if (const auto it = m_collsToConvert.find(recoCollName.value()); it != m_collsToConvert.end()) {
        const auto lcioRecoName = it->second;
        debug() << "Corresponding ReconstructedParticle (LCIO) collection is " << lcioRecoName << endmsg;
        UTIL::PIDHandler pidHandler(lcio_event->getCollection(lcioRecoName));
        if (pidMetaInfo.has_value()) {
          try {
            algoId = pidHandler.getAlgorithmID(pidMetaInfo->algoName);
            debug() << "PID Algorithm already present with id " << algoId.value() << endmsg;
            debug() << fmt::format("params: {}", pidHandler.getParameterNames(algoId.value())) << endmsg;
            debug() << fmt::format("our params: {}", pidMetaInfo->paramNames) << endmsg;
          } catch (const UTIL::UnknownAlgorithm&) {
            algoId = pidHandler.addAlgorithm(pidMetaInfo->algoName, pidMetaInfo->paramNames);
            debug() << "Determined algoId via LCIO PIDHandler: " << algoId.value() << endmsg;
          }
        }
      } else {
        warning() << "Could not find a name mapping for ReconstructedParticle collection " << recoCollName.value()
                  << " when trying to attach ParticleID meta information" << endmsg;
      }
    } else if (pidMetaInfo.has_value()) {
      // We can still set the value we collected along the way. NOTE: It will
      // almost certainly not be consistent in roundtrip conversions.
      algoId = pidMetaInfo->algoType();
    }

    convertParticleIDs(collection_pairs.particleIDs, pidCollMeta.name, algoId.value_or(-1));
  }

  EDM4hep2LCIOConv::attachDqdxInfo(collection_pairs.tracks, dQdxCollections);

  auto& globalObjMap = getGlobalObjectMap(this);

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
