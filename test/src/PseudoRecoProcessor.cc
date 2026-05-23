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
#include "PseudoRecoProcessor.h"

#include "EVENT/MCParticle.h"
#include "IMPL/LCCollectionVec.h"
#include "IMPL/ReconstructedParticleImpl.h"

#include <array>

PseudoRecoProcessor aPseudoRecoProcessor;

PseudoRecoProcessor::PseudoRecoProcessor() : marlin::Processor("PseudoRecoProcessor") {
  registerInputCollection(LCIO::MCPARTICLE, "InputMCs", "Name of the input MCParticle collection", m_mcCollName,
                          std::string("MCParticles"));

  registerOutputCollection(LCIO::RECONSTRUCTEDPARTICLE, "OutputRecos",
                           "Name of the output ReconstructedParticle collection", m_recoCollName,
                           std::string("PseudoRecoParticles"));
}

void PseudoRecoProcessor::processEvent(LCEvent* evt) {
  const auto mcColl = evt->getCollection(m_mcCollName);

  auto recoColl = new IMPL::LCCollectionVec(LCIO::RECONSTRUCTEDPARTICLE);
  for (int i = 0; i < mcColl->getNumberOfElements(); ++i) {
    auto mc = static_cast<EVENT::MCParticle*>(mcColl->getElementAt(i));
    auto reco = new IMPL::ReconstructedParticleImpl();
    reco->setCharge(mc->getCharge());
    reco->setEnergy(mc->getEnergy());
    std::array momentum = {mc->getMomentum()[0], mc->getMomentum()[1], mc->getMomentum()[2]};
    reco->setMomentum(momentum.data());

    recoColl->addElement(reco);
  }

  evt->addCollection(recoColl, m_recoCollName);
}
