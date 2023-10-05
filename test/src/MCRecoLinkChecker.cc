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
#include "MCRecoLinkChecker.h"

DECLARE_COMPONENT(MCRecoLinkChecker)

MCRecoLinkChecker::MCRecoLinkChecker(const std::string& name, ISvcLocator* pSL) : GaudiAlgorithm(name, pSL) {
  declareProperty("InputMCRecoLinks", m_relationCollHandle, "Name of the input MC Reco link collection");
  declareProperty("InputMCs", m_mcCollHandle, "Name of the input MC collection");
  declareProperty("InputRecos", m_recoCollHandle, "Name of the input Reco collection");
}

StatusCode MCRecoLinkChecker::execute() {
  const auto relationColl = m_relationCollHandle.get();
  const auto mcColl       = m_mcCollHandle.get();
  const auto recoColl     = m_recoCollHandle.get();

  if (relationColl->size() != mcColl->size()) {
    error() << "The MCReco relation collection does not have the expected size (expected: " << relationColl->size()
            << ", actual: " << mcColl->size() << ")" << endmsg;
    return StatusCode::FAILURE;
  }

  for (size_t i = 0; i < mcColl->size(); ++i) {
    const auto mc       = (*mcColl)[i];
    const auto reco     = (*recoColl)[i];
    const auto relation = (*relationColl)[i];

    if (relation.getWeight() != i) {
      error() << "Relation " << i << " dost not have the correct weight (expected: " << i
              << ", actual: " << relation.getWeight() << ")" << endmsg;
      return StatusCode::FAILURE;
    }

    if (!(relation.getSim() == mc)) {
      auto relMC = relation.getSim();
      error() << "Relation " << i
              << " does not point to the correct MCParticle (expected: " << mc.getObjectID().collectionID << "|"
              << mc.getObjectID().index << ", actual: " << relMC.getObjectID().collectionID << "|"
              << relMC.getObjectID().index << ")" << endmsg;
      return StatusCode::FAILURE;
    }

    if (!(relation.getRec() == reco)) {
      auto relRec = relation.getRec();
      error() << "Relation " << i
              << " does not point to the correct RecoParticle (expected: " << reco.getObjectID().collectionID << "|"
              << reco.getObjectID().index << ", actual: " << relRec.getObjectID().collectionID << "|"
              << relRec.getObjectID().index << ")" << endmsg;
      return StatusCode::FAILURE;
    }
  }

  return StatusCode::SUCCESS;
}
