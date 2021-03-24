#ifndef K4MARLINWRAPPER_EDM4HEP2LCIO_H
#define K4MARLINWRAPPER_EDM4HEP2LCIO_H


// GAUDI
#include "GaudiAlg/GaudiTool.h"

// FWCore
#include "k4FWCore/DataHandle.h"

// EDM4hep
#include "edm4hep/ParticleIDCollection.h"
#include "edm4hep/ReconstructedParticle.h"
#include "edm4hep/ReconstructedParticleCollection.h"
#include "edm4hep/ReconstructedParticleData.h"
#include "edm4hep/Track.h"
#include "edm4hep/TrackCollection.h"
#include "edm4hep/CalorimeterHit.h"
#include "edm4hep/CalorimeterHitCollection.h"
#include "edm4hep/Cluster.h"
#include "edm4hep/ClusterCollection.h"
#include "edm4hep/Vertex.h"
#include "edm4hep/VertexCollection.h"

// LCIO
#include <LCEventWrapper.h>

#include "lcio.h"
#include "IMPL/LCEventImpl.h"
#include "IMPL/LCCollectionVec.h"
#include "IMPL/ReconstructedParticleImpl.h"
#include "IMPL/TrackImpl.h"
#include "IMPL/TrackStateImpl.h"
#include "IMPL/CalorimeterHitImpl.h"
#include "IMPL/TrackerHitImpl.h"
#include "IMPL/ClusterImpl.h"
#include "IMPL/VertexImpl.h"
#include "IMPL/ParticleIDImpl.h"

#include "LCIOSTLTypes.h"

#include <vector>
#include <string>
#include <bitset>

// Interface
#include "IEDMConverter.h"


struct CollectionsPairVectors {
  std::vector<std::pair<
    lcio::TrackImpl*, edm4hep::Track>> tracks;
  std::vector<std::pair<
    lcio::CalorimeterHitImpl*, edm4hep::CalorimeterHit>> calohits;
  std::vector<std::pair<
      lcio::ClusterImpl*, edm4hep::Cluster>> clusters;
  std::vector<std::pair<
    lcio::VertexImpl*, edm4hep::Vertex>> vertices;
  std::vector<std::pair<
    lcio::ParticleIDImpl*, edm4hep::ParticleID>> particleIDs;
  std::vector<std::pair<
    lcio::ReconstructedParticleImpl*, edm4hep::ReconstructedParticle>> recoparticles;
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

  void convertLCIOTracks(
    std::vector<std::pair<lcio::TrackImpl*, edm4hep::Track>>& tracks_vec,
    const std::string& e4h_coll_name,
    const std::string& lcio_coll_name,
    lcio::LCEventImpl* lcio_event);

void convertLCIOCalorimeterHits(
  std::vector<std::pair<lcio::CalorimeterHitImpl*, edm4hep::CalorimeterHit>>& calo_hits_vec,
  const std::string& e4h_coll_name,
  const std::string& lcio_coll_name,
  lcio::LCEventImpl* lcio_event);

  void convertLCIOClusters(
    std::vector<std::pair<lcio::ClusterImpl*, edm4hep::Cluster>>& cluster_vec,
    const std::vector<std::pair<lcio::ParticleIDImpl*, edm4hep::ParticleID>>& particleIDs_vec,
    const std::vector<std::pair<lcio::CalorimeterHitImpl*, edm4hep::CalorimeterHit>>& calohits_vec,
    const std::string& e4h_coll_name,
    const std::string& lcio_coll_name,
    lcio::LCEventImpl* lcio_event);

  void convertLCIOVertices(
    std::vector<std::pair<lcio::VertexImpl*, edm4hep::Vertex>>& vertex_vec,
    const std::vector<std::pair<lcio::ReconstructedParticleImpl*, edm4hep::ReconstructedParticle>>& recoparticles_vec,
    const std::string& e4h_name,
    const std::string& lcio_coll_name,
    lcio::LCEventImpl* lcio_event);

  void convertLCIOParticleIDs(
    std::vector<std::pair<lcio::ParticleIDImpl*, edm4hep::ParticleID>>& particleIDs_vec,
    const std::string& e4h_coll_name,
    const std::string& lcio_coll_name,
    lcio::LCEventImpl* lcio_event);

  void convertLCIOReconstructedParticles(
    std::vector<std::pair<lcio::ReconstructedParticleImpl*, edm4hep::ReconstructedParticle>>& recoparticles_vec,
    const std::vector<std::pair<lcio::ParticleIDImpl*, edm4hep::ParticleID>>& particleIDs_vec,
    const std::vector<std::pair<lcio::TrackImpl*, edm4hep::Track>>& tracks_vec,
    const std::vector<std::pair<lcio::VertexImpl*, edm4hep::Vertex>>& vertex_vec,
    const std::vector<std::pair<lcio::ClusterImpl*, edm4hep::Cluster>>& clusters_vec,
    const std::string& e4h_coll_name,
    const std::string& lcio_coll_name,
    lcio::LCEventImpl* lcio_event);

  void FillMissingCollections(
    CollectionsPairVectors& collection_pairs);

  void convertAdd(
    const std::string& type,
    const std::string& e4h_coll_name,
    const std::string& lcio_coll_name,
    lcio::LCEventImpl* lcio_event,
    CollectionsPairVectors& collection_pairs);

  bool collectionExist(
    const std::string& collection_name,
    lcio::LCEventImpl* lcio_event);
};

#endif