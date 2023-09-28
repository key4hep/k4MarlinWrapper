#include "PseudoRecoProcessor.h"

#include "EVENT/MCParticle.h"
#include "IMPL/LCCollectionVec.h"
#include "IMPL/ReconstructedParticleImpl.h"
#include "UTIL/LCIterator.h"

PseudoRecoProcessor aPseudoRecoProcessor;

PseudoRecoProcessor::PseudoRecoProcessor() : marlin::Processor("PseudoRecoProcessor") {
  registerInputCollection(LCIO::MCPARTICLE, "InputMCs", "Name of the input MCParticle collection", m_mcCollName,
                          std::string("MCParticles"));

  registerOutputCollection(LCIO::RECONSTRUCTEDPARTICLE, "OutputRecos",
                           "Name of the output ReconstructedParticle collection", m_recoCollName,
                           std::string("PseudoRecoParticles"));
}

void PseudoRecoProcessor::processEvent(LCEvent* evt) {
  auto  mcParticleIter = UTIL::LCIterator<EVENT::MCParticle>(evt, m_mcCollName);
  auto* recoColl       = new IMPL::LCCollectionVec(LCIO::RECONSTRUCTEDPARTICLE);

  while (auto* mcParticle = mcParticleIter.next()) {
    auto recoParticle = new IMPL::ReconstructedParticleImpl();
    recoParticle->setCharge(mcParticle->getCharge());
    recoParticle->setMomentum(mcParticle->getMomentum());
    recoParticle->setEnergy(mcParticle->getEnergy());
    recoParticle->setType(mcParticle->getPDG());

    recoColl->addElement(recoParticle);
  }

  evt->addCollection(recoColl, m_recoCollName);
}
