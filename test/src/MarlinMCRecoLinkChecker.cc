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
#include "marlin/Processor.h"

#include <EVENT/LCCollection.h>
#include <EVENT/LCEvent.h>
#include <EVENT/LCRelation.h>
#include <EVENT/MCParticle.h>
#include <EVENT/ReconstructedParticle.h>

#include <stdexcept>
#include <string>

class MarlinMCRecoLinkChecker : public marlin::Processor {
public:
  MarlinMCRecoLinkChecker();

  marlin::Processor* newProcessor() final { return new MarlinMCRecoLinkChecker; }

  void processEvent(LCEvent* evt) final;

private:
  std::string m_mcCollName{};
  std::string m_recoCollName{};
  std::string m_relCollName{};
};

MarlinMCRecoLinkChecker::MarlinMCRecoLinkChecker() : marlin::Processor("MarlinMCRecoLinkChecker") {
  registerInputCollection(LCIO::MCPARTICLE, "InputMCs", "Name of the input MCParticle collection", m_mcCollName,
                          std::string("MCParticles"));

  registerInputCollection(LCIO::RECONSTRUCTEDPARTICLE, "InputRecos",
                          "Name of the input ReconstructedParticle collection", m_recoCollName,
                          std::string("PseudoRecoParticles"));
  registerInputCollection(LCIO::LCRELATION, "MCRecoLinks", "Name of the input Reco - MC Truth link collection",
                          m_relCollName, std::string("TrivialMCRecoLinks"));
}

void MarlinMCRecoLinkChecker::processEvent(LCEvent* evt) {
  const auto mcColl   = evt->getCollection(m_mcCollName);
  const auto recoColl = evt->getCollection(m_recoCollName);
  const auto relColl  = evt->getCollection(m_relCollName);

  if (relColl->getNumberOfElements() != mcColl->getNumberOfElements()) {
    throw std::runtime_error("The LCRelation collection does not have the expected number of elements: (expected " +
                             std::to_string(mcColl->getNumberOfElements()) + ", actual " +
                             std::to_string(relColl->getNumberOfElements()) + ")");
  }

  for (size_t i = 0; i < static_cast<size_t>(mcColl->getNumberOfElements()); ++i) {
    const auto mc   = static_cast<EVENT::MCParticle*>(mcColl->getElementAt(i));
    const auto reco = static_cast<EVENT::ReconstructedParticle*>(recoColl->getElementAt(i));
    const auto rel  = static_cast<EVENT::LCRelation*>(relColl->getElementAt(i));

    if (rel->getWeight() != i) {
      throw std::runtime_error("Relation " + std::to_string(i) + " does not have the correct weight (expected: " +
                               std::to_string(i) + ", actual: " + std::to_string(rel->getWeight()) + ")");
    }

    if (rel->getTo() != mc) {
      throw std::runtime_error("Relation " + std::to_string(i) + " does not point to the correct MCParticle");
    }

    if (rel->getFrom() != reco) {
      throw std::runtime_error("Relation " + std::to_string(i) + " does not point to the correct MCParticle");
    }
  }
}

MarlinMCRecoLinkChecker aMarlinMCRecoLinkChecker{};
