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

#include "edm4hep/ParticleIDCollection.h"
#include "edm4hep/ReconstructedParticleCollection.h"
#include "edm4hep/utils/ParticleIDUtils.h"

#include "k4FWCore/DataHandle.h"

#include "Gaudi/Algorithm.h"
#include "Gaudi/Property.h"

#include <string>
#include <vector>

/// Produce ParticleID collections (and the ReconstructedParticles they point
/// to) so that both the empty and the non-empty ParticleID conversion paths can
/// be exercised. The non-empty collection is linked to the ReconstructedParticles
/// and gets consistent metadata attached via the MetadataSvc.
class PIDProducer : public Gaudi::Algorithm {
public:
  explicit PIDProducer(const std::string& name, ISvcLocator* pSL);
  StatusCode initialize() override;
  StatusCode execute(const EventContext&) const override;

private:
  mutable k4FWCore::DataHandle<edm4hep::ReconstructedParticleCollection> m_recoCollHandle{
      "RecoParticles", Gaudi::DataHandle::Writer, this};
  mutable k4FWCore::DataHandle<edm4hep::ParticleIDCollection> m_pidCollHandle{"EmptyParticleIDs",
                                                                              Gaudi::DataHandle::Writer, this};
  mutable k4FWCore::DataHandle<edm4hep::ParticleIDCollection> m_filledPidCollHandle{"ParticleIDs",
                                                                                    Gaudi::DataHandle::Writer, this};

  Gaudi::Property<int> m_numRecos{this, "NumRecoParticles", 3,
                                  "Number of ReconstructedParticles (and non-empty ParticleIDs) to produce"};
  Gaudi::Property<std::string> m_pidAlgoName{this, "PIDAlgoName", "testPIDAlgo",
                                             "Name of the ParticleID algorithm stored in the metadata"};
  Gaudi::Property<std::vector<std::string>> m_pidParamNames{
      this, "PIDParamNames", {"param1", "param2"}, "Names of the ParticleID parameters stored in the metadata"};

  edm4hep::utils::ParticleIDMeta m_pidMeta{};
};
