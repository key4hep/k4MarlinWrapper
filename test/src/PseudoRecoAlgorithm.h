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

#include "k4FWCore/DataHandle.h"

#include "Gaudi/Algorithm.h"

#include <string>

class PseudoRecoAlgorithm : public Gaudi::Algorithm {
public:
  explicit PseudoRecoAlgorithm(const std::string& name, ISvcLocator* pSL);
  StatusCode execute(const EventContext&) const override;

private:
  mutable DataHandle<edm4hep::MCParticleCollection> m_mcCollHandle{"MCParticles", Gaudi::DataHandle::Reader, this};
  mutable DataHandle<edm4hep::ReconstructedParticleCollection> m_recoCollHandle{"RecoParticles",
                                                                                Gaudi::DataHandle::Writer, this};
};
