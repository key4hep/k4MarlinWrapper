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

#include "edm4hep/MCParticleCollection.h"
#include "edm4hep/ReconstructedParticleCollection.h"

#include "k4FWCore/Transformer.h"

#include <string>

struct PseudoRecoAlgorithm final
    : k4FWCore::Transformer<edm4hep::ReconstructedParticleCollection(const edm4hep::MCParticleCollection&)> {
  PseudoRecoAlgorithm(const std::string& name, ISvcLocator* svcLoc)
      : Transformer(name, svcLoc, {KeyValues("InputMCs", {"MCParticles"})},
                    {KeyValues("OutputRecos", {"PseudoRecoParticles"})}) {}

  // This is the function that will be called to transform the data
  // Note that the function has to be const, as well as all pointers to collections
  // we get from the input
  edm4hep::ReconstructedParticleCollection operator()(const edm4hep::MCParticleCollection& input) const override {
    auto coll_out = edm4hep::ReconstructedParticleCollection();
    for (const auto& particle : input) {
      auto new_particle = coll_out.create();
      new_particle.setCharge(particle.getCharge());
      new_particle.setMomentum({static_cast<float>(particle.getMomentum()[0]),
                                static_cast<float>(particle.getMomentum()[1]),
                                static_cast<float>(particle.getMomentum()[2])});
      new_particle.setEnergy(particle.getEnergy());
    }
    return coll_out;
  }
};

DECLARE_COMPONENT(PseudoRecoAlgorithm)
