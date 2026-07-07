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

#include "k4FWCore/MetadataUtils.h"
#include "k4FWCore/Transformer.h"

#include "edm4hep/ParticleIDCollection.h"
#include "edm4hep/ReconstructedParticleCollection.h"
#include "edm4hep/utils/ParticleIDUtils.h"

#include "Gaudi/Property.h"

#include <cstddef>
#include <string>
#include <tuple>
#include <vector>

using retType =
    std::tuple<edm4hep::ReconstructedParticleCollection, edm4hep::ParticleIDCollection, edm4hep::ParticleIDCollection>;

struct PIDProducer final : k4FWCore::MultiTransformer<retType()> {
  PIDProducer(const std::string& name, ISvcLocator* svcLoc)
      : MultiTransformer(name, svcLoc, {},
                         {KeyValue("OutputRecoColl", "RecoParticles"), KeyValue("OutputFilledPIDColl", "ParticleIDs"),
                          KeyValue("OutputPIDColl", "EmptyParticleIDs")}) {}

  StatusCode initialize() override {
    m_pidMeta = {m_pidAlgoName, m_pidParamNames};
    k4FWCore::putParameter(outputLocations("OutputFilledPIDColl")[0], m_pidMeta, this);
    k4FWCore::putParameter(outputLocations("OutputPIDColl")[0], m_pidMeta, this);
    return StatusCode::SUCCESS;
  }

  retType operator()() const override {
    // Create a non-empty ReconstructedParticle collection that the non-empty
    // ParticleID collection can point to
    auto recoColl = edm4hep::ReconstructedParticleCollection{};
    for (int i = 0; i < m_numRecos; ++i) {
      auto reco = recoColl.create();
      reco.setCharge(1.0f);
      reco.setPDG(11);
      reco.setEnergy(static_cast<float>(i + 1));
      reco.setMomentum({static_cast<float>(i + 1), 2.0f, 3.0f});
    }

    // Create a non-empty ParticleID collection linked to the ReconstructedParticles
    auto filledPidColl = edm4hep::ParticleIDCollection{};
    for (const auto& reco : recoColl) {
      auto pid = filledPidColl.create();
      pid.setAlgorithmType(m_pidMeta.algoType());
      pid.setPDG(reco.getPDG());
      pid.setParticle(reco);
      for (std::size_t i = 0; i < m_pidMeta.paramNames.size(); ++i) {
        pid.addToParameters(static_cast<float>(i) * 0.5f);
      }
    }

    // Also produce an empty ParticleID collection to exercise the empty conversion path
    auto emptyPidColl = edm4hep::ParticleIDCollection{};

    return std::make_tuple(std::move(recoColl), std::move(filledPidColl), std::move(emptyPidColl));
  }

private:
  Gaudi::Property<int> m_numRecos{this, "NumRecoParticles", 3,
                                  "Number of ReconstructedParticles (and non-empty ParticleIDs) to produce"};
  Gaudi::Property<std::string> m_pidAlgoName{this, "PIDAlgoName", "testPIDAlgo",
                                             "Name of the ParticleID algorithm stored in the metadata"};
  Gaudi::Property<std::vector<std::string>> m_pidParamNames{
      this, "PIDParamNames", {"param1", "param2"}, "Names of the ParticleID parameters stored in the metadata"};

  edm4hep::utils::ParticleIDMeta m_pidMeta{};
};

DECLARE_COMPONENT(PIDProducer)
