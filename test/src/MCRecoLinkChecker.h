#ifndef K4MARLINWRAPPER_TEST_MCRECOLINKCHECKER_H
#define K4MARLINWRAPPER_TEST_MCRECOLINKCHECKER_H

#include "edm4hep/MCParticleCollection.h"
#include "edm4hep/MCRecoParticleAssociationCollection.h"
#include "edm4hep/ReconstructedParticleCollection.h"

#include "k4FWCore/DataHandle.h"

#include <GaudiAlg/GaudiAlgorithm.h>
#include <GaudiKernel/ISvcLocator.h>

#include <string>

class MCRecoLinkChecker : public GaudiAlgorithm {
public:
  explicit MCRecoLinkChecker(const std::string& name, ISvcLocator* pSL);
  StatusCode execute() final;

private:
  DataHandle<edm4hep::MCRecoParticleAssociationCollection> m_relationCollHandle{"MCRecoTruthLinks",
                                                                                Gaudi::DataHandle::Reader, this};
  DataHandle<edm4hep::MCParticleCollection>            m_mcCollHandle{"MCParticles", Gaudi::DataHandle::Reader, this};
  DataHandle<edm4hep::ReconstructedParticleCollection> m_recoCollHandle{"RecoParticles", Gaudi::DataHandle::Reader,
                                                                        this};
};

#endif  // K4MARLINWRAPPER_TEST_MCRECOLINKCHECKER_H
