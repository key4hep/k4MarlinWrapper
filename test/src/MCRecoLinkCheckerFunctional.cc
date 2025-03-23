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
#include "MCRecoLinkCheckerFunctional.h"

MCRecoLinkCheckerFunctional::MCRecoLinkCheckerFunctional(const std::string& name, ISvcLocator* svcLoc)
    : Consumer(name, svcLoc,
               {KeyValues("InputMCRecoLinks", {"InputMCRecoLinks"}), KeyValues("InputMCs", {"InputMCs"}),
                KeyValues("InputRecos", {"InputRecos"})}) {}

void MCRecoLinkCheckerFunctional::operator()(const edm4hep::RecoMCParticleLinkCollection& relationColl,
                                             const edm4hep::MCParticleCollection& mcColl,
                                             const edm4hep::ReconstructedParticleCollection& recoColl) const {
  if (relationColl.size() != mcColl.size()) {
    error() << "The MCReco relation collection does not have the expected size (expected: " << relationColl.size()
            << ", actual: " << mcColl.size() << ")" << endmsg;
    throw std::runtime_error(
        "MCRecoLinkCheckerFunctional: The MCReco relation collection does not have the expected size");
  }

  for (size_t i = 0; i < mcColl.size(); ++i) {
    const auto mc = mcColl[i];
    const auto reco = recoColl[i];
    const auto relation = relationColl[i];

    if (relation.getWeight() != i) {
      error() << "Relation " << i << " does not not have the correct weight (expected: " << i
              << ", actual: " << relation.getWeight() << ")" << endmsg;
      throw std::runtime_error("The MCReco relation collection does not have the expected weight");
    }

    if (!(relation.getTo() == mc)) {
      auto relMC = relation.getTo();
      error() << "Relation " << i
              << " does not point to the correct MCParticle (expected: " << mc.getObjectID().collectionID << "|"
              << mc.getObjectID().index << ", actual: " << relMC.getObjectID().collectionID << "|"
              << relMC.getObjectID().index << ")" << endmsg;
      throw std::runtime_error("The MCReco relation collection does not point to the correct MCParticle");
    }

    if (!(relation.getFrom() == reco)) {
      auto relRec = relation.getFrom();
      error() << "Relation " << i
              << " does not point to the correct RecoParticle (expected: " << reco.getObjectID().collectionID << "|"
              << reco.getObjectID().index << ", actual: " << relRec.getObjectID().collectionID << "|"
              << relRec.getObjectID().index << ")" << endmsg;
      throw std::runtime_error("The MCReco relation collection does not point to the correct RecoParticle");
    }
  }
}

DECLARE_COMPONENT(MCRecoLinkCheckerFunctional)
