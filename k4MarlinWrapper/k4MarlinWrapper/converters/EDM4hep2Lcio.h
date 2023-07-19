#ifndef K4MARLINWRAPPER_EDM4HEP2LCIO_H
#define K4MARLINWRAPPER_EDM4HEP2LCIO_H

// k4MarlinWrapper
#include "k4MarlinWrapper/LCEventWrapper.h"
#include "k4MarlinWrapper/converters/IEDMConverter.h"
#include "k4MarlinWrapper/util/k4MarlinWrapperUtil.h"

// FWCore
#include <k4FWCore/DataHandle.h>

//k4EDM4hep2LcioConv
#include "k4EDM4hep2LcioConv/k4EDM4hep2LcioConv.h"

// GAUDI
#include <GaudiAlg/GaudiTool.h>

// std
#include <algorithm>
#include <bitset>
#include <map>
#include <string>
#include <vector>

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

  PodioLegacyDataSvc*             m_podioDataSvc;
  ServiceHandle<IDataProviderSvc> m_eventDataSvc;

  void convertTracks(vec_pair<lcio::TrackImpl*, edm4hep::Track>&           tracks_vec,
                     vec_pair<lcio::TrackerHitImpl*, edm4hep::TrackerHit>& trackerhits_vec,
                     const std::string& e4h_coll_name, const std::string& lcio_coll_name,
                     lcio::LCEventImpl* lcio_event);

  void convertTrackerHits(vec_pair<lcio::TrackerHitImpl*, edm4hep::TrackerHit>& trackerhits_vec,
                          const std::string& e4h_coll_name, const std::string& lcio_coll_name,
                          lcio::LCEventImpl* lcio_event);

  void convertSimTrackerHits(vec_pair<lcio::SimTrackerHitImpl*, edm4hep::SimTrackerHit>& simtrackerhits_vec,
                             const vec_pair<lcio::MCParticleImpl*, edm4hep::MCParticle>& mcparticles_vec,
                             const std::string& e4h_coll_name, const std::string& lcio_coll_name,
                             lcio::LCEventImpl* lcio_event);

  void convertCalorimeterHits(vec_pair<lcio::CalorimeterHitImpl*, edm4hep::CalorimeterHit>& calo_hits_vec,
                              const std::string& e4h_coll_name, const std::string& lcio_coll_name,
                              lcio::LCEventImpl* lcio_event);

  void convertRawCalorimeterHits(vec_pair<lcio::RawCalorimeterHitImpl*, edm4hep::RawCalorimeterHit>& raw_calo_hits_vec,
                                 const std::string& e4h_coll_name, const std::string& lcio_coll_name,
                                 lcio::LCEventImpl* lcio_event);

  void convertSimCalorimeterHits(vec_pair<lcio::SimCalorimeterHitImpl*, edm4hep::SimCalorimeterHit>& sim_calo_hits_vec,
                                 const vec_pair<lcio::MCParticleImpl*, edm4hep::MCParticle>&         mcparticles,
                                 const std::string& e4h_coll_name, const std::string& lcio_coll_name,
                                 lcio::LCEventImpl* lcio_event);

  void convertTPCHits(vec_pair<lcio::TPCHitImpl*, edm4hep::RawTimeSeries>& tpc_hits_vec,
                      const std::string& e4h_coll_name, const std::string& lcio_coll_name,
                      lcio::LCEventImpl* lcio_event);

  void convertClusters(vec_pair<lcio::ClusterImpl*, edm4hep::Cluster>&                     cluster_vec,
                       const vec_pair<lcio::CalorimeterHitImpl*, edm4hep::CalorimeterHit>& calohits_vec,
                       const std::string& e4h_coll_name, const std::string& lcio_coll_name,
                       lcio::LCEventImpl* lcio_event);

  void convertVertices(
      vec_pair<lcio::VertexImpl*, edm4hep::Vertex>&                                     vertex_vec,
      const vec_pair<lcio::ReconstructedParticleImpl*, edm4hep::ReconstructedParticle>& recoparticles_vec,
      const std::string& e4h_name, const std::string& lcio_coll_name, lcio::LCEventImpl* lcio_event);

  void convertReconstructedParticles(
      vec_pair<lcio::ReconstructedParticleImpl*, edm4hep::ReconstructedParticle>& recoparticles_vec,
      const vec_pair<lcio::TrackImpl*, edm4hep::Track>&                           tracks_vec,
      const vec_pair<lcio::VertexImpl*, edm4hep::Vertex>&                         vertex_vec,
      const vec_pair<lcio::ClusterImpl*, edm4hep::Cluster>& clusters_vec, const std::string& e4h_coll_name,
      const std::string& lcio_coll_name, lcio::LCEventImpl* lcio_event);

  void convertMCParticles(vec_pair<lcio::MCParticleImpl*, edm4hep::MCParticle>& mc_particles_vec,
                          const std::string& e4h_coll_name, const std::string& lcio_coll_name,
                          lcio::LCEventImpl* lcio_event);

  void convertEventHeader(const std::string& e4h_coll_name, lcio::LCEventImpl* lcio_event);

  void convertAdd(const std::string& e4h_coll_name, const std::string& lcio_coll_name, lcio::LCEventImpl* lcio_event,
                  CollectionsPairVectors& collection_pairs);
};

#endif
