/*
 * Copyright (c) 2019-2024 Key4hep-Project.
 *
 * This file is part of Key4hep.
 * See https://key4hep.github.io/key4hep-doc/ for further info.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
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
