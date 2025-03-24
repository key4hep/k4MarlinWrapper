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
#ifndef K4MARLINWRAPPER_EDM4HEP2LCIO_H
#define K4MARLINWRAPPER_EDM4HEP2LCIO_H

#include "k4MarlinWrapper/converters/IEDMConverter.h"

#include "k4EDM4hep2LcioConv/k4EDM4hep2LcioConv.h"

#include <Gaudi/Property.h>
#include <GaudiKernel/AlgTool.h>

#include <map>
#include <string>
#include <tuple>
#include <vector>

class PodioDataSvc;
class IDataProviderSvc;
class IMetadataSvc;

template <typename K, typename V>
using ObjMapT = k4EDM4hep2LcioConv::VecMapT<K, V>;

using TrackMap = ObjMapT<lcio::TrackImpl*, edm4hep::Track>;
using ClusterMap = ObjMapT<lcio::ClusterImpl*, edm4hep::Cluster>;
using VertexMap = ObjMapT<lcio::VertexImpl*, edm4hep::Vertex>;
using TrackerHitMap = ObjMapT<lcio::TrackerHitImpl*, edm4hep::TrackerHit3D>;
using TrackerHitPlaneMap = ObjMapT<lcio::TrackerHitPlaneImpl*, edm4hep::TrackerHitPlane>;
using SimTrackerHitMap = ObjMapT<lcio::SimTrackerHitImpl*, edm4hep::SimTrackerHit>;
using CaloHitMap = ObjMapT<lcio::CalorimeterHitImpl*, edm4hep::CalorimeterHit>;
using SimCaloHitMap = ObjMapT<lcio::SimCalorimeterHitImpl*, edm4hep::SimCalorimeterHit>;
using RawCaloHitMap = ObjMapT<lcio::RawCalorimeterHitImpl*, edm4hep::RawCalorimeterHit>;
using TPCHitMap = ObjMapT<lcio::TPCHitImpl*, edm4hep::RawTimeSeries>;
using RecoParticleMap = ObjMapT<lcio::ReconstructedParticleImpl*, edm4hep::ReconstructedParticle>;
using MCParticleMap = ObjMapT<lcio::MCParticleImpl*, edm4hep::MCParticle>;
using ParticleIDMap = ObjMapT<lcio::ParticleIDImpl*, edm4hep::ParticleID>;

struct CollectionPairMappings;

class EDM4hep2LcioTool : public AlgTool, virtual public IEDMConverter {
public:
  EDM4hep2LcioTool(const std::string& type, const std::string& name, const IInterface* parent);
  StatusCode initialize() final;
  StatusCode finalize() final;

  StatusCode convertCollections(lcio::LCEventImpl* lcio_event) final;

private:
  Gaudi::Property<std::map<std::string, std::string>> m_collNames{this, "collNameMapping", {}};
  Gaudi::Property<bool> m_convertAll{this, "convertAll", true};

  PodioDataSvc* m_podioDataSvc;
  // EventDataSvc that is used together with IOSvc
  ServiceHandle<IDataProviderSvc> m_eventDataSvc;
  // Metadata service from k4FWCore that is used together with IOSvc
  SmartIF<IMetadataSvc> m_metadataSvc;
  /// A (caching) "map" of original to new collection names that will be populated
  /// during the first conversion
  std::vector<std::tuple<std::string, std::string>> m_collsToConvert{};
  std::map<uint32_t, std::string> m_idToName;

  void convertTracks(TrackMap& tracks_vec, const std::string& e4h_coll_name, const std::string& lcio_coll_name,
                     lcio::LCEventImpl* lcio_event);

  void convertTrackerHits(TrackerHitMap& trackerhits_vec, const std::string& e4h_coll_name,
                          const std::string& lcio_coll_name, lcio::LCEventImpl* lcio_event);

  void convertTrackerHitPlanes(TrackerHitPlaneMap& trackerhits_vec, const std::string& e4h_coll_name,
                               const std::string& lcio_coll_name, lcio::LCEventImpl* lcio_event);

  void convertSimTrackerHits(SimTrackerHitMap& simtrackerhits_vec, const std::string& e4h_coll_name,
                             const std::string& lcio_coll_name, lcio::LCEventImpl* lcio_event);

  void convertCalorimeterHits(CaloHitMap& calo_hits_vec, const std::string& e4h_coll_name,
                              const std::string& lcio_coll_name, lcio::LCEventImpl* lcio_event);

  void convertRawCalorimeterHits(RawCaloHitMap& raw_calo_hits_vec, const std::string& e4h_coll_name,
                                 const std::string& lcio_coll_name, lcio::LCEventImpl* lcio_event);

  void convertSimCalorimeterHits(SimCaloHitMap& sim_calo_hits_vec, const std::string& e4h_coll_name,
                                 const std::string& lcio_coll_name, lcio::LCEventImpl* lcio_event);

  void convertTPCHits(TPCHitMap& tpc_hits_vec, const std::string& e4h_coll_name, const std::string& lcio_coll_name,
                      lcio::LCEventImpl* lcio_event);

  void convertClusters(ClusterMap& cluster_vec, const std::string& e4h_coll_name, const std::string& lcio_coll_name,
                       lcio::LCEventImpl* lcio_event);

  void convertVertices(VertexMap& vertex_vec, const std::string& e4h_name, const std::string& lcio_coll_name,
                       lcio::LCEventImpl* lcio_event);

  void convertReconstructedParticles(RecoParticleMap& recoparticles_vec, const std::string& e4h_coll_name,
                                     const std::string& lcio_coll_name, lcio::LCEventImpl* lcio_event);

  void convertParticleIDs(ParticleIDMap& pidMap, const std::string& e4h_coll_name, int32_t algoId);

  void convertMCParticles(MCParticleMap& mc_particles_vec, const std::string& e4h_coll_name,
                          const std::string& lcio_coll_name, lcio::LCEventImpl* lcio_event);

  void convertEventHeader(const std::string& e4h_coll_name, lcio::LCEventImpl* lcio_event);

  void convertAdd(const std::string& e4h_coll_name, const std::string& lcio_coll_name, lcio::LCEventImpl* lcio_event,
                  CollectionPairMappings& collection_pairs,
                  std::vector<EDM4hep2LCIOConv::ParticleIDConvData>& pidCollections,
                  std::vector<EDM4hep2LCIOConv::TrackDqdxConvData>& dQdxCollections);

  /// Get an EDM4hep collection by name, consulting either the podio based data
  /// svc or the IOSvc
  podio::CollectionBase* getEDM4hepCollection(const std::string& name) const;
};
#endif
