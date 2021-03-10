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
#include "IMPL/VertexImpl.h"
#include "IMPL/ParticleIDImpl.h"

#include <vector>
#include <string>

// Interface
#include "IEDM4hep2Lcio.h"


class EDM4hep2LcioTool : public GaudiTool, virtual public IEDM4hep2LcioTool {
public:

  EDM4hep2LcioTool(const std::string& type, const std::string& name, const IInterface* parent);
  virtual ~EDM4hep2LcioTool();
  virtual StatusCode initialize();
  virtual StatusCode finalize();

  StatusCode convertCollections(
    lcio::LCEventImpl* lcio_event);

private:

  Gaudi::Property<std::vector<std::string>> m_edm2lcio_params{this, "EDM2LCIOConversion", {}};

  // Save pointer to converted element, and pointer to original element in a std::pair
  std::vector<std::pair<
    lcio::TrackImpl*, edm4hep::Track>> m_lcio_tracks_vec;
  std::vector<std::pair<
    lcio::VertexImpl*, edm4hep::Vertex>> m_lcio_vertex_vec;
  std::vector<std::pair<
    lcio::ParticleIDImpl*, edm4hep::ParticleID>> m_lcio_particleIDs_vec;
  std::vector<std::pair<
    lcio::ReconstructedParticleImpl*, edm4hep::ReconstructedParticle>> m_lcio_rec_particles_vec;

  void addLCIOConvertedTracks(
    std::vector<std::pair<lcio::TrackImpl*, edm4hep::Track>>& m_lcio_tracks_vec,
    const std::string& name,
    const std::string& lcio_collection_name,
    lcio::LCEventImpl* lcio_event);

  void addLCIOVertices(
    std::vector<std::pair<lcio::VertexImpl*, edm4hep::Vertex>>& m_lcio_vertex_vec,
    const std::string& name,
    const std::string& lcio_collection_name,
    lcio::LCEventImpl* lcio_event);

  void addLCIOParticleIDs(
    std::vector<std::pair<lcio::ParticleIDImpl*, edm4hep::ParticleID>>& m_lcio_particleIDs_vec,
    const std::string& name,
    const std::string& lcio_collection_name,
    lcio::LCEventImpl* lcio_event);

  void addLCIOReconstructedParticles(
    std::vector<std::pair<lcio::ReconstructedParticleImpl*, edm4hep::ReconstructedParticle>>& m_lcio_rec_particles_vec,
    const std::vector<std::pair<lcio::ParticleIDImpl*, edm4hep::ParticleID>>& m_lcio_particleIDs_vec,
    const std::vector<std::pair<lcio::TrackImpl*, edm4hep::Track>>& m_lcio_tracks_vec,
    const std::vector<std::pair<lcio::VertexImpl*, edm4hep::Vertex>>& m_lcio_vertex_vec,
    const std::string& name,
    const std::string& lcio_collection_name,
    lcio::LCEventImpl* lcio_event);

  void FillMissingCollections(
    std::vector<std::pair<lcio::ReconstructedParticleImpl*, edm4hep::ReconstructedParticle>>& m_lcio_rec_particles_vec,
    const std::vector<std::pair<lcio::ParticleIDImpl*, edm4hep::ParticleID>>& m_lcio_particleIDs_vec,
    const std::vector<std::pair<lcio::TrackImpl*, edm4hep::Track>>& m_lcio_tracks_vec,
    const std::vector<std::pair<lcio::VertexImpl*, edm4hep::Vertex>>& m_lcio_vertex_vec);

  void convertAdd(
    const std::string& type,
    const std::string& name,
    const std::string& lcio_collection_name,
    lcio::LCEventImpl* lcio_event);

  bool collectionExist(
    const std::string& collection_name,
    lcio::LCEventImpl* lcio_event);
};

#endif