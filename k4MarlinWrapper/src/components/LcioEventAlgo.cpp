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

#include "k4MarlinWrapper/LcioEventAlgo.h"
#include "k4MarlinWrapper/util/k4MarlinWrapperUtil.h"

DECLARE_COMPONENT(LcioEvent)

LcioEvent::LcioEvent(const std::string& name, ISvcLocator* pSL) : GaudiAlgorithm(name, pSL) {}

StatusCode LcioEvent::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize();
  if (sc.isFailure())
    return sc;

  marlin::Global::parameters->add("LCIOInputFiles", m_fileNames);

  m_reader = new MT::LCReader(0);
  m_reader->open(m_fileNames);
  info() << "Initialized the LcioEvent Algo: " << m_fileNames[0] << endmsg;
  return StatusCode::SUCCESS;
}

StatusCode LcioEvent::execute() {
  auto theEvent = m_reader->readNextEvent(EVENT::LCIO::UPDATE);

  if (theEvent == nullptr) {
    // Store flag to indicate there was NOT a LCEvent
    auto             pStatus  = std::make_unique<LCEventWrapperStatus>(false);
    const StatusCode scStatus = eventSvc()->registerObject("/Event/LCEventStatus", pStatus.release());
    if (scStatus.isFailure()) {
      error() << "Failed to store flag for underlying LCEvent: MarlinProcessorWrapper may try to run over non existing "
                 "event"
              << endmsg;
      return scStatus;
    }

    IEventProcessor* evt = nullptr;
    if (service("ApplicationMgr", evt, true).isSuccess()) {
      evt->stopRun().ignore();
      evt->release();
    } else {
      abort();
    }
  } else {
    // pass theEvent to the DataStore, so we can access them in our processor
    // wrappers
    info() << "Reading from file: " << m_fileNames[0] << endmsg;

    auto             myEvWr = new LCEventWrapper(std::move(theEvent));
    const StatusCode sc     = eventSvc()->registerObject("/Event/LCEvent", myEvWr);
    if (sc.isFailure()) {
      error() << "Failed to store the LCEvent" << endmsg;
      return sc;
    } else {
      // Store flag to indicate there was a LCEvent
      auto pStatus = std::make_unique<LCEventWrapperStatus>(true);
      std::cout << "Saving status: " << pStatus->hasLCEvent << std::endl;
      const StatusCode scStatus = eventSvc()->registerObject("/Event/LCEventStatus", pStatus.release());
      if (scStatus.isFailure()) {
        error() << "Failed to store flag for underlying LCEvent: MarlinProcessorWrapper may try to run over non "
                   "existing event"
                << endmsg;
        return scStatus;
      }
    }
  }

  return StatusCode::SUCCESS;
}
