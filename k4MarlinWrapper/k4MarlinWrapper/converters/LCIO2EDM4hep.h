#pragma once

// GAUDI
#include "GaudiAlg/GaudiAlgorithm.h"
#include "Gaudi/Property.h"

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

class LCIO2EDM4hep : public GaudiAlgorithm {
public:
  LCIO2EDM4hep(const std::string& name, ISvcLocator* svcLoc);

  void addLCIOConvertedTracks(std::vector<std::pair<lcio::TrackImpl*, edm4hep::Track>>& lcio_tracks_vec);
  void addLCIOParticleIDs(std::vector<std::pair<lcio::ParticleIDImpl*, edm4hep::ParticleID>>& lcio_particleIDs_vec);
  void addLCIOReconstructedParticles(
    std::vector<std::pair<lcio::ReconstructedParticleImpl*, edm4hep::ReconstructedParticle>>& lcio_rec_particles_vec,
    const std::vector<std::pair<lcio::ParticleIDImpl*, edm4hep::ParticleID>>& lcio_particleIDs_vec,
    const std::vector<std::pair<lcio::TrackImpl*, edm4hep::Track>>& lcio_tracks_vec);

  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();

private:

  // int m_member = 0;
  // DataHandle<edm4hep::MCParticleCollection> mcps_handle {"Particle", Gaudi::DataHandle::Reader, this};
  /// Handle for the edm4hep MC particles to be read
  // DataHandle<edm4hep::ReconstructedParticleData> m_e4hhandle {"ReconstructedParticles", Gaudi::DataHandle::Reader, this};
  // DataHandle<edm4hep::MCParticleData> m_e4hhandle {"Particle", Gaudi::DataHandle::Reader, this};
  // DataHandle<edm4hep::MCParticleData> m_e4hhandle {"Particle", Gaudi::DataHandle::Reader, this};
};