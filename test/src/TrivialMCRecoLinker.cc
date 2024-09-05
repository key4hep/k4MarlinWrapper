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

#include <GaudiKernel/ISvcLocator.h>
#include "edm4hep/MCParticleCollection.h"
#include "edm4hep/RecoMCParticleLinkCollection.h"
#include "edm4hep/ReconstructedParticleCollection.h"

#include "k4FWCore/Transformer.h"

#include <string>

struct TrivialMCRecoLinker final
    : k4FWCore::Transformer<edm4hep::RecoMCParticleLinkCollection(const edm4hep::MCParticleCollection&,
                                                                  const edm4hep::ReconstructedParticleCollection&)> {
  TrivialMCRecoLinker(const std::string& name, ISvcLocator* svcLoc)
      : Transformer(name, svcLoc,
                    {KeyValues("InputMCs", {"MCParticles"}), KeyValues("InputRecos", {"PseudoRecoParticles"})},
                    KeyValues("OutputLinks", {"TrivialMCRecoLinks"})) {}

  edm4hep::RecoMCParticleLinkCollection operator()(
      const edm4hep::MCParticleCollection&            mcParticles,
      const edm4hep::ReconstructedParticleCollection& recoParticles) const override {
    auto links = edm4hep::RecoMCParticleLinkCollection{};

    for (size_t i = 0; i < mcParticles.size(); ++i) {
      const auto mc   = mcParticles[i];
      const auto reco = recoParticles[i];

      auto link = links.create();
      link.setFrom(reco);
      link.setTo(mc);
      link.setWeight(i);
    }

    return links;
  }
};

DECLARE_COMPONENT(TrivialMCRecoLinker)
