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

#include "PseudoRecoAlgorithm.h"

#include "edm4hep/MCParticleCollection.h"
#include "edm4hep/ReconstructedParticleCollection.h"

#include "Gaudi/Algorithm.h"

#include <string>

PseudoRecoAlgorithm::PseudoRecoAlgorithm(const std::string& name, ISvcLocator* pSL) : Gaudi::Algorithm(name, pSL) {
  declareProperty("InputMCs", m_mcCollHandle, "Name of the input MC collection");
  declareProperty("OutputRecos", m_recoCollHandle, "Name of the input Reco collection");
}

StatusCode PseudoRecoAlgorithm::execute(const EventContext&) const {
  auto* coll_out = m_recoCollHandle.createAndPut();
  auto* coll_in  = m_mcCollHandle.get();
  for (const auto& particle : *coll_in) {
    auto new_particle = coll_out->create();
    new_particle.setCharge(particle.getCharge());
    new_particle.setMomentum({static_cast<float>(particle.getMomentum()[0]),
                              static_cast<float>(particle.getMomentum()[1]),
                              static_cast<float>(particle.getMomentum()[2])});
    new_particle.setEnergy(particle.getEnergy());
  }
  return StatusCode::SUCCESS;
}

DECLARE_COMPONENT(PseudoRecoAlgorithm)
