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
#include "edm4hep/ReconstructedParticle.h"
#include "edm4hep/ReconstructedParticleCollection.h"
#include "edm4hep/ReconstructedParticleData.h"
#include "edm4hep/RecoParticleRefData.h"
#include "edm4hep/TrackCollection.h"

class LCIO2EDM4hep : public GaudiAlgorithm {
public:
  LCIO2EDM4hep(const std::string& name, ISvcLocator* svcLoc);

  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();

private:

  int m_member = 0;
  // DataHandle<edm4hep::MCParticleCollection> mcps_handle {"Particle", Gaudi::DataHandle::Reader, this};
  /// Handle for the edm4hep MC particles to be read
  // DataHandle<edm4hep::ReconstructedParticleData> m_e4hhandle {"ReconstructedParticles", Gaudi::DataHandle::Reader, this};
  // DataHandle<edm4hep::MCParticleData> m_e4hhandle {"Particle", Gaudi::DataHandle::Reader, this};
  // DataHandle<edm4hep::MCParticleData> m_e4hhandle {"Particle", Gaudi::DataHandle::Reader, this};
};