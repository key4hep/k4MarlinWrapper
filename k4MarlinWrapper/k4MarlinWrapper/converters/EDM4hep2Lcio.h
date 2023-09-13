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

  void convertTracks(ObjMapT<lcio::TrackImpl*, edm4hep::Track>&           tracks_vec,
                     ObjMapT<lcio::TrackerHitImpl*, edm4hep::TrackerHit>& trackerhits_vec,
                     const std::string& e4h_coll_name, const std::string& lcio_coll_name,
                     lcio::LCEventImpl* lcio_event);

  void convertTrackerHits(ObjMapT<lcio::TrackerHitImpl*, edm4hep::TrackerHit>& trackerhits_vec,
                          const std::string& e4h_coll_name, const std::string& lcio_coll_name,
                          lcio::LCEventImpl* lcio_event);

  void convertSimTrackerHits(ObjMapT<lcio::SimTrackerHitImpl*, edm4hep::SimTrackerHit>& simtrackerhits_vec,
                             const ObjMapT<lcio::MCParticleImpl*, edm4hep::MCParticle>& mcparticles_vec,
                             const std::string& e4h_coll_name, const std::string& lcio_coll_name,
                             lcio::LCEventImpl* lcio_event);

  void convertCalorimeterHits(ObjMapT<lcio::CalorimeterHitImpl*, edm4hep::CalorimeterHit>& calo_hits_vec,
                              const std::string& e4h_coll_name, const std::string& lcio_coll_name,
                              lcio::LCEventImpl* lcio_event);

  void convertRawCalorimeterHits(ObjMapT<lcio::RawCalorimeterHitImpl*, edm4hep::RawCalorimeterHit>& raw_calo_hits_vec,
                                 const std::string& e4h_coll_name, const std::string& lcio_coll_name,
                                 lcio::LCEventImpl* lcio_event);

  void convertSimCalorimeterHits(ObjMapT<lcio::SimCalorimeterHitImpl*, edm4hep::SimCalorimeterHit>& sim_calo_hits_vec,
                                 const ObjMapT<lcio::MCParticleImpl*, edm4hep::MCParticle>&         mcparticles,
                                 const std::string& e4h_coll_name, const std::string& lcio_coll_name,
                                 lcio::LCEventImpl* lcio_event);

  void convertTPCHits(ObjMapT<lcio::TPCHitImpl*, edm4hep::RawTimeSeries>& tpc_hits_vec,
                      const std::string& e4h_coll_name, const std::string& lcio_coll_name,
                      lcio::LCEventImpl* lcio_event);

  void convertClusters(ObjMapT<lcio::ClusterImpl*, edm4hep::Cluster>&                     cluster_vec,
                       const ObjMapT<lcio::CalorimeterHitImpl*, edm4hep::CalorimeterHit>& calohits_vec,
                       const std::string& e4h_coll_name, const std::string& lcio_coll_name,
                       lcio::LCEventImpl* lcio_event);

  void convertVertices(
      ObjMapT<lcio::VertexImpl*, edm4hep::Vertex>&                                     vertex_vec,
      const ObjMapT<lcio::ReconstructedParticleImpl*, edm4hep::ReconstructedParticle>& recoparticles_vec,
      const std::string& e4h_name, const std::string& lcio_coll_name, lcio::LCEventImpl* lcio_event);

  void convertReconstructedParticles(
      ObjMapT<lcio::ReconstructedParticleImpl*, edm4hep::ReconstructedParticle>& recoparticles_vec,
      const ObjMapT<lcio::TrackImpl*, edm4hep::Track>&                           tracks_vec,
      const ObjMapT<lcio::VertexImpl*, edm4hep::Vertex>&                         vertex_vec,
      const ObjMapT<lcio::ClusterImpl*, edm4hep::Cluster>& clusters_vec, const std::string& e4h_coll_name,
      const std::string& lcio_coll_name, lcio::LCEventImpl* lcio_event);

  void convertMCParticles(ObjMapT<lcio::MCParticleImpl*, edm4hep::MCParticle>& mc_particles_vec,
                          const std::string& e4h_coll_name, const std::string& lcio_coll_name,
                          lcio::LCEventImpl* lcio_event);

  void convertEventHeader(const std::string& e4h_coll_name, lcio::LCEventImpl* lcio_event);

  void convertAdd(const std::string& e4h_coll_name, const std::string& lcio_coll_name, lcio::LCEventImpl* lcio_event,
                  CollectionPairMappings& collection_pairs);
};

#endif
