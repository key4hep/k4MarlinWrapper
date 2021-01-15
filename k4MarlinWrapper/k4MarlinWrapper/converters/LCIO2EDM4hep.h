#pragma once

// GAUDI
#include "GaudiAlg/GaudiAlgorithm.h"

// FWCore
#include "k4FWCore/DataHandle.h"

// EDM4hep
#include "edm4hep/MCParticle.h"
#include "edm4hep/MCParticleData.h"
#include "edm4hep/MCParticleCollection.h"
// #include "edm4hep/ReconstructedParticle.h"
#include "edm4hep/ReconstructedParticleCollection.h"
#include "edm4hep/ReconstructedParticleData.h"

class LCIO2EDM4hep : public GaudiAlgorithm {
public:
  LCIO2EDM4hep(const std::string& name, ISvcLocator* svcLoc);

  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();

private:
  /// Handle for the edm4hep MC particles to be read
  // DataHandle<edm4hep::ReconstructedParticleData> m_e4hhandle {"ReconstructedParticles", Gaudi::DataHandle::Reader, this};
  // DataHandle<edm4hep::MCParticleData> m_e4hhandle {"Particle", Gaudi::DataHandle::Reader, this};
  // DataHandle<edm4hep::MCParticleData> m_e4hhandle {"Particle", Gaudi::DataHandle::Reader, this};
};