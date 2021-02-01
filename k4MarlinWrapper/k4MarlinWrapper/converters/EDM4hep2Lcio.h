#pragma once

// GAUDI
#include "Gaudi/Property.h"
#include "GaudiAlg/GaudiTool.h"

// FWCore
#include "k4FWCore/DataHandle.h"

// EDM4hep
#include "edm4hep/MCParticle.h"
#include "edm4hep/MCParticleData.h"
#include "edm4hep/MCParticleCollection.h"
#include "edm4hep/ParticleIDCollection.h"
#include "edm4hep/ReconstructedParticle.h"
#include "edm4hep/ReconstructedParticleCollection.h"
#include "edm4hep/ReconstructedParticleData.h"
#include "edm4hep/RecoParticleRefData.h"
#include "edm4hep/Track.h"
#include "edm4hep/TrackCollection.h"
#include "edm4hep/VertexConst.h"

// LCIO
#include <LCEventWrapper.h>
#include <EVENT/LCEvent.h>

#include "lcio.h"
#include "IO/LCWriter.h"
#include "IMPL/LCEventImpl.h"
#include "IMPL/LCRunHeaderImpl.h"
#include "IMPL/LCCollectionVec.h"
#include "IMPL/MCParticleImpl.h"
#include "IMPL/ReconstructedParticleImpl.h"
#include "IMPL/TrackImpl.h"
#include "IMPL/TrackStateImpl.h"
#include "IMPL/VertexImpl.h"
#include "IMPL/ParticleIDImpl.h"

// ROOT
#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"

#include <vector>
#include <string>
#include <ctime>
#include <chrono>
#include <type_traits>
#include <utility>

// Interface
#include "IEDM4hep2Lcio.h"



class EDM4hep2LcioTool : public GaudiTool, virtual public IEDM4hep2LcioTool {
public:
  EDM4hep2LcioTool(const std::string& type, const std::string& name, const IInterface* parent);
  virtual ~EDM4hep2LcioTool();
  virtual StatusCode initialize();

  void addLCIOConvertedTracks(std::vector<std::pair<lcio::TrackImpl*, edm4hep::Track>>& lcio_tracks_vec);
  void addLCIOParticleIDs(std::vector<std::pair<lcio::ParticleIDImpl*, edm4hep::ParticleID>>& lcio_particleIDs_vec);
  void addLCIOReconstructedParticles(
    std::vector<std::pair<lcio::ReconstructedParticleImpl*, edm4hep::ReconstructedParticle>>& lcio_rec_particles_vec,
    const std::vector<std::pair<lcio::ParticleIDImpl*, edm4hep::ParticleID>>& lcio_particleIDs_vec,
    const std::vector<std::pair<lcio::TrackImpl*, edm4hep::Track>>& lcio_tracks_vec);

  void dispatcher(
    const std::string& type,
    const std::string& name);

  StatusCode convertCollections();

  // Save pointer to converted element, and pointer to original element in a std::pair
  std::vector<std::pair<
    lcio::TrackImpl*, edm4hep::Track>> lcio_tracks_vec;
  std::vector<std::pair<
    lcio::ParticleIDImpl*, edm4hep::ParticleID>> lcio_particleIDs_vec;
  std::vector<std::pair<
    lcio::ReconstructedParticleImpl*, edm4hep::ReconstructedParticle>> lcio_rec_particles_vec;

};