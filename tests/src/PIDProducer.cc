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

#include "PIDProducer.h"

#include "edm4hep/ParticleIDCollection.h"
#include "edm4hep/ReconstructedParticleCollection.h"
#include "edm4hep/utils/ParticleIDUtils.h"

#include "k4FWCore/MetadataUtils.h"

#include "Gaudi/Algorithm.h"

#include <cstddef>
#include <string>

PIDProducer::PIDProducer(const std::string& name, ISvcLocator* pSL) : Gaudi::Algorithm(name, pSL) {
  declareProperty("OutputRecoColl", m_recoCollHandle, "Name of the output ReconstructedParticle collection");
  declareProperty("OutputPIDColl", m_pidCollHandle, "Name of the output empty ParticleID collection");
  declareProperty("OutputFilledPIDColl", m_filledPidCollHandle,
                  "Name of the output non-empty ParticleID collection linked to the ReconstructedParticles");
}

StatusCode PIDProducer::initialize() {
  if (Gaudi::Algorithm::initialize().isFailure()) {
    return StatusCode::FAILURE;
  }

  m_pidMeta = {m_pidAlgoName, m_pidParamNames};
  // The metadata has to be attached to both ParticleID collections before the
  // event loop starts, so that the EDM4hep2Lcio conversion can pick it up
  k4FWCore::putParameter(m_filledPidCollHandle.objKey(), m_pidMeta, this);
  k4FWCore::putParameter(m_pidCollHandle.objKey(), m_pidMeta, this);

  return StatusCode::SUCCESS;
}

StatusCode PIDProducer::execute(const EventContext&) const {
  // Create a non-empty ReconstructedParticle collection that the non-empty
  // ParticleID collection can point to
  auto* recoColl = m_recoCollHandle.createAndPut();
  for (int i = 0; i < m_numRecos; ++i) {
    auto reco = recoColl->create();
    reco.setCharge(1.0f);
    reco.setPDG(11);
    reco.setEnergy(static_cast<float>(i + 1));
    reco.setMomentum({static_cast<float>(i + 1), 2.0f, 3.0f});
  }

  // Create a non-empty ParticleID collection linked to the ReconstructedParticles
  auto* filledPidColl = m_filledPidCollHandle.createAndPut();
  for (const auto& reco : *recoColl) {
    auto pid = filledPidColl->create();
    pid.setAlgorithmType(m_pidMeta.algoType());
    pid.setPDG(reco.getPDG());
    pid.setParticle(reco);
    for (std::size_t i = 0; i < m_pidMeta.paramNames.size(); ++i) {
      pid.addToParameters(static_cast<float>(i) * 0.5f);
    }
  }

  // Create an empty ParticleID collection to also exercise the empty conversion path
  m_pidCollHandle.createAndPut();

  return StatusCode::SUCCESS;
}

DECLARE_COMPONENT(PIDProducer)
