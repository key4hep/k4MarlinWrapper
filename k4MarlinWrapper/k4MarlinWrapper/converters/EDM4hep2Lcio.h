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
#ifndef K4MARLINWRAPPER_EDM4HEP2LCIO_H
#define K4MARLINWRAPPER_EDM4HEP2LCIO_H

// k4MarlinWrapper
#include "k4MarlinWrapper/converters/IEDMConverter.h"

// FWCore
#include <k4FWCore/PodioDataSvc.h>

//k4EDM4hep2LcioConv
#include "k4EDM4hep2LcioConv/k4EDM4hep2LcioConv.h"

// GAUDI
#include <GaudiAlg/GaudiTool.h>

// std
#include <map>
#include <string>
#include <tuple>
#include <vector>

template <typename K, typename V> using ObjMapT = k4EDM4hep2LcioConv::VecMapT<K, V>;

using TrackMap         = ObjMapT<lcio::TrackImpl*, edm4hep::Track>;
using ClusterMap       = ObjMapT<lcio::ClusterImpl*, edm4hep::Cluster>;
using VertexMap        = ObjMapT<lcio::VertexImpl*, edm4hep::Vertex>;
using TrackerHitMap    = ObjMapT<lcio::TrackerHitImpl*, edm4hep::TrackerHit>;
using SimTrackerHitMap = ObjMapT<lcio::SimTrackerHitImpl*, edm4hep::SimTrackerHit>;
using CaloHitMap       = ObjMapT<lcio::CalorimeterHitImpl*, edm4hep::CalorimeterHit>;
using SimCaloHitMap    = ObjMapT<lcio::SimCalorimeterHitImpl*, edm4hep::SimCalorimeterHit>;
using RawCaloHitMap    = ObjMapT<lcio::RawCalorimeterHitImpl*, edm4hep::RawCalorimeterHit>;
using TPCHitMap        = ObjMapT<lcio::TPCHitImpl*, edm4hep::RawTimeSeries>;
using RecoParticleMap  = ObjMapT<lcio::ReconstructedParticleImpl*, edm4hep::ReconstructedParticle>;
using MCParticleMap    = ObjMapT<lcio::MCParticleImpl*, edm4hep::MCParticle>;

struct CollectionPairMappings;

class EDM4hep2LcioTool : public GaudiTool, virtual public IEDMConverter {
public:
  EDM4hep2LcioTool(const std::string& type, const std::string& name, const IInterface* parent);
  virtual ~EDM4hep2LcioTool();
  virtual StatusCode initialize();
  virtual StatusCode finalize();

  StatusCode convertCollections(lcio::LCEventImpl* lcio_event);

private:
  Gaudi::Property<std::map<std::string, std::string>> m_collNames{this, "collNameMapping", {}};
  Gaudi::Property<bool>                               m_convertAll{this, "convertAll", true};

  PodioDataSvc*                   m_podioDataSvc;
  ServiceHandle<IDataProviderSvc> m_eventDataSvc;

  void convertTracks(TrackMap& tracks_vec, const TrackerHitMap& trackerhits_vec, const std::string& e4h_coll_name,
                     const std::string& lcio_coll_name, lcio::LCEventImpl* lcio_event);

  void convertTrackerHits(TrackerHitMap& trackerhits_vec, const std::string& e4h_coll_name,
                          const std::string& lcio_coll_name, lcio::LCEventImpl* lcio_event);

  void convertSimTrackerHits(SimTrackerHitMap& simtrackerhits_vec, const MCParticleMap& mcparticles_vec,
                             const std::string& e4h_coll_name, const std::string& lcio_coll_name,
                             lcio::LCEventImpl* lcio_event);

  void convertCalorimeterHits(CaloHitMap& calo_hits_vec, const std::string& e4h_coll_name,
                              const std::string& lcio_coll_name, lcio::LCEventImpl* lcio_event);

  void convertRawCalorimeterHits(RawCaloHitMap& raw_calo_hits_vec, const std::string& e4h_coll_name,
                                 const std::string& lcio_coll_name, lcio::LCEventImpl* lcio_event);

  void convertSimCalorimeterHits(SimCaloHitMap& sim_calo_hits_vec, const MCParticleMap& mcparticles,
                                 const std::string& e4h_coll_name, const std::string& lcio_coll_name,
                                 lcio::LCEventImpl* lcio_event);

  void convertTPCHits(TPCHitMap& tpc_hits_vec, const std::string& e4h_coll_name, const std::string& lcio_coll_name,
                      lcio::LCEventImpl* lcio_event);

  void convertClusters(ClusterMap& cluster_vec, const CaloHitMap& calohits_vec, const std::string& e4h_coll_name,
                       const std::string& lcio_coll_name, lcio::LCEventImpl* lcio_event);

  void convertVertices(VertexMap& vertex_vec, const RecoParticleMap& recoparticles_vec, const std::string& e4h_name,
                       const std::string& lcio_coll_name, lcio::LCEventImpl* lcio_event);

  void convertReconstructedParticles(RecoParticleMap& recoparticles_vec, const TrackMap& tracks_vec,
                                     const VertexMap& vertex_vec, const ClusterMap& clusters_vec,
                                     const std::string& e4h_coll_name, const std::string& lcio_coll_name,
                                     lcio::LCEventImpl* lcio_event);

  void convertMCParticles(MCParticleMap& mc_particles_vec, const std::string& e4h_coll_name,
                          const std::string& lcio_coll_name, lcio::LCEventImpl* lcio_event);

  void convertEventHeader(const std::string& e4h_coll_name, lcio::LCEventImpl* lcio_event);

  void convertAdd(const std::string& e4h_coll_name, const std::string& lcio_coll_name, lcio::LCEventImpl* lcio_event,
                  CollectionPairMappings& collection_pairs);
};

#endif
