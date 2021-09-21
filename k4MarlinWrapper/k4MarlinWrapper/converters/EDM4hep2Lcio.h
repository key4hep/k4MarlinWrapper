#ifndef K4MARLINWRAPPER_EDM4HEP2LCIO_H
#define K4MARLINWRAPPER_EDM4HEP2LCIO_H

// std
#include <vector>
#include <string>
#include <bitset>
#include <algorithm>

// GAUDI
#include <GaudiAlg/GaudiTool.h>

// FWCore
#include <k4FWCore/DataHandle.h>

// k4MarlinWrapper
#include "k4MarlinWrapper/converters/IEDMConverter.h"
#include "k4MarlinWrapper/LCEventWrapper.h"
#include "k4MarlinWrapper/util/k4MarlinWrapperUtil.h"



template <typename T1, typename T2>
using vec_pair = std::vector<std::pair<T1, T2>>;

struct CollectionsPairVectors {
  vec_pair<lcio::TrackImpl*, edm4hep::ConstTrack> tracks;
  vec_pair<lcio::TrackerHitImpl*, edm4hep::ConstTrackerHit> trackerhits;
  vec_pair<lcio::SimTrackerHitImpl*, edm4hep::ConstSimTrackerHit> simtrackerhits;
  vec_pair<lcio::CalorimeterHitImpl*, edm4hep::ConstCalorimeterHit> calohits;
  vec_pair<lcio::RawCalorimeterHitImpl*, edm4hep::ConstRawCalorimeterHit> rawcalohits;
  vec_pair<lcio::SimCalorimeterHitImpl*, edm4hep::ConstSimCalorimeterHit> simcalohits;
  vec_pair<lcio::TPCHitImpl*, edm4hep::ConstTPCHit> tpchits;
  vec_pair<lcio::ClusterImpl*, edm4hep::ConstCluster> clusters;
  vec_pair<lcio::VertexImpl*, edm4hep::ConstVertex> vertices;
  vec_pair<lcio::ReconstructedParticleImpl*, edm4hep::ConstReconstructedParticle> recoparticles;
  vec_pair<lcio::MCParticleImpl*, edm4hep::ConstMCParticle> mcparticles;
};

class EDM4hep2LcioTool : public GaudiTool, virtual public IEDMConverter {
public:

  EDM4hep2LcioTool(const std::string& type, const std::string& name, const IInterface* parent);
  virtual ~EDM4hep2LcioTool();
  virtual StatusCode initialize();
  virtual StatusCode finalize();

  StatusCode convertCollections(
    lcio::LCEventImpl* lcio_event);

private:

  Gaudi::Property<std::vector<std::string>> m_edm2lcio_params{this, "Parameters", {}};

  PodioDataSvc* m_podioDataSvc;
  ServiceHandle<IDataProviderSvc> m_eventDataSvc;

  void convertTracks(
    vec_pair<lcio::TrackImpl*, edm4hep::ConstTrack>& tracks_vec,
    vec_pair<lcio::TrackerHitImpl*, edm4hep::ConstTrackerHit>& trackerhits_vec,
    const std::string& e4h_coll_name,
    const std::string& lcio_coll_name,
    lcio::LCEventImpl* lcio_event);

  void convertTrackerHits(
    vec_pair<lcio::TrackerHitImpl*, edm4hep::ConstTrackerHit>& trackerhits_vec,
    const std::string& e4h_coll_name,
    const std::string& lcio_coll_name,
    lcio::LCEventImpl* lcio_event);

  void convertSimTrackerHits(
    vec_pair<lcio::SimTrackerHitImpl*, edm4hep::ConstSimTrackerHit>& simtrackerhits_vec,
    const vec_pair<lcio::MCParticleImpl*, edm4hep::ConstMCParticle>& mcparticles_vec,
    const std::string& e4h_coll_name,
    const std::string& lcio_coll_name,
    lcio::LCEventImpl* lcio_event);

  void convertCalorimeterHits(
    vec_pair<lcio::CalorimeterHitImpl*, edm4hep::ConstCalorimeterHit>& calo_hits_vec,
    const std::string& e4h_coll_name,
    const std::string& lcio_coll_name,
    lcio::LCEventImpl* lcio_event);

  void convertRawCalorimeterHits(
    vec_pair<lcio::RawCalorimeterHitImpl*, edm4hep::ConstRawCalorimeterHit>& raw_calo_hits_vec,
    const std::string& e4h_coll_name,
    const std::string& lcio_coll_name,
    lcio::LCEventImpl* lcio_event);

  void convertSimCalorimeterHits(
    vec_pair<lcio::SimCalorimeterHitImpl*, edm4hep::ConstSimCalorimeterHit>& sim_calo_hits_vec,
    const vec_pair<lcio::MCParticleImpl*, edm4hep::ConstMCParticle>& mcparticles,
    const std::string& e4h_coll_name,
    const std::string& lcio_coll_name,
    lcio::LCEventImpl* lcio_event);

  void convertTPCHits(
    vec_pair<lcio::TPCHitImpl*, edm4hep::ConstTPCHit>& tpc_hits_vec,
    const std::string& e4h_coll_name,
    const std::string& lcio_coll_name,
    lcio::LCEventImpl* lcio_event);

  void convertClusters(
    vec_pair<lcio::ClusterImpl*, edm4hep::ConstCluster>& cluster_vec,
    const vec_pair<lcio::CalorimeterHitImpl*, edm4hep::ConstCalorimeterHit>& calohits_vec,
    const std::string& e4h_coll_name,
    const std::string& lcio_coll_name,
    lcio::LCEventImpl* lcio_event);

  void convertVertices(
    vec_pair<lcio::VertexImpl*, edm4hep::ConstVertex>& vertex_vec,
    const vec_pair<lcio::ReconstructedParticleImpl*, edm4hep::ConstReconstructedParticle>& recoparticles_vec,
    const std::string& e4h_name,
    const std::string& lcio_coll_name,
    lcio::LCEventImpl* lcio_event);

  void convertReconstructedParticles(
    vec_pair<lcio::ReconstructedParticleImpl*, edm4hep::ConstReconstructedParticle>& recoparticles_vec,
    const vec_pair<lcio::TrackImpl*, edm4hep::ConstTrack>& tracks_vec,
    const vec_pair<lcio::VertexImpl*, edm4hep::ConstVertex>& vertex_vec,
    const vec_pair<lcio::ClusterImpl*, edm4hep::ConstCluster>& clusters_vec,
    const std::string& e4h_coll_name,
    const std::string& lcio_coll_name,
    lcio::LCEventImpl* lcio_event);

  void convertMCParticles(
    vec_pair<lcio::MCParticleImpl*, edm4hep::ConstMCParticle>& mc_particles_vec,
    const std::string& e4h_coll_name,
    const std::string& lcio_coll_name,
    lcio::LCEventImpl* lcio_event);

  void FillMissingCollections(
    CollectionsPairVectors& collection_pairs);

  void convertAdd(
    const std::string& e4h_coll_name,
    const std::string& lcio_coll_name,
    lcio::LCEventImpl* lcio_event,
    CollectionsPairVectors& collection_pairs);

  bool collectionExist(
    const std::string& collection_name,
    lcio::LCEventImpl* lcio_event);

};

#endif
