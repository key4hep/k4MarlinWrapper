#include "TrivialMCTruthLinkerProcessor.h"

#include "EVENT/MCParticle.h"
#include "EVENT/ReconstructedParticle.h"
#include "IMPL/LCCollectionVec.h"
#include "IMPL/LCRelationImpl.h"

TrivialMCTruthLinkerProcessor aTrivialMCTruthLinkerProcessor;

TrivialMCTruthLinkerProcessor::TrivialMCTruthLinkerProcessor() : marlin::Processor("TrivialMCTruthLinkerProcessor") {
  registerInputCollection(LCIO::MCPARTICLE, "InputMCs", "Name of the input MCParticle collection", m_mcCollName,
                          std::string("MCParticles"));

  registerInputCollection(LCIO::RECONSTRUCTEDPARTICLE, "InputRecos",
                          "Name of the input ReconstructedParticle collection", m_recoCollName,
                          std::string("PseudoRecoParticles"));
  registerOutputCollection(LCIO::LCRELATION, "OutputMCRecoLinks", "Name of the output Reco - MC Truth link collection",
                           m_relCollName, std::string("TrivialMCRecoLinks"));
}

void TrivialMCTruthLinkerProcessor::processEvent(LCEvent* evt) {
  const auto mcColl   = evt->getCollection(m_mcCollName);
  const auto recoColl = evt->getCollection(m_recoCollName);

  // NOTE: Not using LCRelationNavigator here because the internal map might
  // screw up the order and we want an easy check afterwards
  auto relationColl = new IMPL::LCCollectionVec(LCIO::LCRELATION);
  relationColl->parameters().setValue("FromType", LCIO::MCPARTICLE);
  relationColl->parameters().setValue("ToType", LCIO::RECONSTRUCTEDPARTICLE);

  for (int i = 0; i < mcColl->getNumberOfElements(); ++i) {
    auto relation = new IMPL::LCRelationImpl();
    relation->setFrom(mcColl->getElementAt(i));
    relation->setTo(recoColl->getElementAt(i));
    relation->setWeight(i);
    relationColl->addElement(relation);
  }

  evt->addCollection(relationColl, m_relCollName);
}
