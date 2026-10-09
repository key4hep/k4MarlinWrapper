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
#include "marlin/Global.h"
#include "marlin/Processor.h"
#include "marlin/ProcessorEventSeeder.h"

#include <EVENT/LCEvent.h>

#include <map>
#include <stdexcept>
#include <string>

/// Checks that the random seed Marlin provides to this processor changes from event to event. Marlin derives the
/// seeds from the run and event number, so events with different numbers have to get different seeds, unless the
/// seeds are not refreshed for every event.
class MarlinRandomSeedChecker : public marlin::Processor {
public:
  MarlinRandomSeedChecker();

  marlin::Processor* newProcessor() final { return new MarlinRandomSeedChecker; }

  void init() final;
  void processEvent(LCEvent* evt) final;

private:
  /// event number in which each seed was seen first
  std::map<unsigned int, int> m_seenSeeds{};
};

MarlinRandomSeedChecker::MarlinRandomSeedChecker() : marlin::Processor("MarlinRandomSeedChecker") {
  _description = "Fails if the random seed of this processor is the same in two events";
}

void MarlinRandomSeedChecker::init() { marlin::Global::EVENTSEEDER->registerProcessor(this); }

void MarlinRandomSeedChecker::processEvent(LCEvent* evt) {
  const auto seed = marlin::Global::EVENTSEEDER->getSeed(this);
  streamlog_out(MESSAGE) << "Event " << evt->getEventNumber() << " (run " << evt->getRunNumber() << "): seed " << seed
                         << std::endl;

  const auto [it, inserted] = m_seenSeeds.emplace(seed, evt->getEventNumber());
  if (!inserted) {
    throw std::runtime_error("Random seed " + std::to_string(seed) + " in event " +
                             std::to_string(evt->getEventNumber()) + " was already used in event " +
                             std::to_string(it->second) + ": the seeds were not refreshed for this event");
  }
}

MarlinRandomSeedChecker aMarlinRandomSeedChecker{};
