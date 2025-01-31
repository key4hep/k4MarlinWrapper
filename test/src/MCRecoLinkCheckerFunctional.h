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
#ifndef K4MARLINWRAPPER_TEST_MCRECOLINKCHECKERFUNCTIONAL_H
#define K4MARLINWRAPPER_TEST_MCRECOLINKCHECKERFUNCTIONAL_H

#include "edm4hep/MCParticleCollection.h"
#include "edm4hep/RecoMCParticleLinkCollection.h"
#include "edm4hep/ReconstructedParticleCollection.h"

#include "k4FWCore/Consumer.h"

struct MCRecoLinkCheckerFunctional final
    : k4FWCore::Consumer<void(const edm4hep::RecoMCParticleLinkCollection&, const edm4hep::MCParticleCollection&,
                              const edm4hep::ReconstructedParticleCollection&)> {
  MCRecoLinkCheckerFunctional(const std::string& name, ISvcLocator* svcLoc);

  void operator()(const edm4hep::RecoMCParticleLinkCollection&    relationColl,
                  const edm4hep::MCParticleCollection&            mcColl,
                  const edm4hep::ReconstructedParticleCollection& recoColl) const override;
};

#endif  // K4MARLINWRAPPER_TEST_MCRECOLINKCHECKERFUNCTIONAL_H
