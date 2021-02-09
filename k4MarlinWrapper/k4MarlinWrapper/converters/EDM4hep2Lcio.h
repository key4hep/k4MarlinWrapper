#ifndef K4MARLINWRAPPER_EDM4HEP2LCIO_H
#define K4MARLINWRAPPER_EDM4HEP2LCIO_H

// GAUDI
#include "GaudiKernel/AlgTool.h"

// FWCore
#include "k4FWCore/DataHandle.h"

// EDM4hep
#include "edm4hep/ParticleIDCollection.h"
#include "edm4hep/ReconstructedParticle.h"
#include "edm4hep/ReconstructedParticleCollection.h"
#include "edm4hep/ReconstructedParticleData.h"
#include "edm4hep/Track.h"
#include "edm4hep/TrackCollection.h"
#include "edm4hep/VertexConst.h"

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
#include <chrono>

// Interface
#include "IEDM4hep2Lcio.h"


class EDM4hep2LcioTool : public extends<AlgTool, IEDM4hep2LcioTool> {
public:

  // Standard constructor
  using extends::extends;

  // Save pointer to converted element, and pointer to original element in a std::pair
  std::vector<std::pair<
    lcio::TrackImpl*, edm4hep::Track>> lcio_tracks_vec;
  std::vector<std::pair<
    lcio::ParticleIDImpl*, edm4hep::ParticleID>> lcio_particleIDs_vec;
  std::vector<std::pair<
    lcio::ReconstructedParticleImpl*, edm4hep::ReconstructedParticle>> lcio_rec_particles_vec;

  void addLCIOConvertedTracks(
    std::vector<std::pair<lcio::TrackImpl*, edm4hep::Track>>& lcio_tracks_vec,
    const std::string& name,
    const std::string& lcio_collection_name,
    lcio::LCEventImpl* lcio_event);
  void addLCIOParticleIDs(
    std::vector<std::pair<lcio::ParticleIDImpl*, edm4hep::ParticleID>>& lcio_particleIDs_vec,
    const std::string& name,
    const std::string& lcio_collection_name,
    lcio::LCEventImpl* lcio_event);
  void addLCIOReconstructedParticles(
    std::vector<std::pair<lcio::ReconstructedParticleImpl*, edm4hep::ReconstructedParticle>>& lcio_rec_particles_vec,
    const std::vector<std::pair<lcio::ParticleIDImpl*, edm4hep::ParticleID>>& lcio_particleIDs_vec,
    const std::vector<std::pair<lcio::TrackImpl*, edm4hep::Track>>& lcio_tracks_vec,
    const std::string& name,
    const std::string& lcio_collection_name,
    lcio::LCEventImpl* lcio_event);

  void convert_add(
    const std::string& type,
    const std::string& name,
    const std::string& lcio_collection_name,
    lcio::LCEventImpl* lcio_event);

  StatusCode convertCollections(
    const Gaudi::Property<std::vector<std::string>>& parameters);
};

#endif