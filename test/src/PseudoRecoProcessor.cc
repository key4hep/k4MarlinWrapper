/*
 * Copyright (c) 2019-2023 Key4hep-Project.
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
