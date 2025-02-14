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
#include "k4MarlinWrapper/LcioEventAlgo.h"
#include "k4MarlinWrapper/LCEventWrapper.h"

#include "marlin/Global.h"
#include "marlin/StringParameters.h"

#include <EVENT/LCIO.h>
#include <MT/LCReader.h>

#include <GaudiKernel/IEventProcessor.h>

#include <stdexcept>
#include <string>

DECLARE_COMPONENT(LcioEvent)

LcioEvent::LcioEvent(const std::string& name, ISvcLocator* pSL) : Gaudi::Algorithm(name, pSL) {}

StatusCode LcioEvent::initialize() {
  StatusCode sc = Gaudi::Algorithm::initialize();
  if (sc.isFailure())
    return sc;

  marlin::Global::parameters->add("LCIOInputFiles", m_fileNames);

  m_reader = new MT::LCReader(0);
  m_reader->open(m_fileNames);
  m_numberOfEvents = m_reader->getNumberOfEvents();
  m_reader->skipNEvents(m_skipNEvents.value());
  info() << "Initialized the LcioEvent Algo. Reading from " << m_fileNames.size() << " with " << m_numberOfEvents
         << " events in total. Skipping the first " << m_skipNEvents << " events." << endmsg;
  return StatusCode::SUCCESS;
}

StatusCode LcioEvent::execute(const EventContext&) const {
  debug() << "Reading event " << ++m_currentEvent << " / " << m_numberOfEvents << endmsg;
  auto theEvent = m_reader->readNextEvent(EVENT::LCIO::UPDATE);

  if (theEvent == nullptr) {
    fatal() << "Failed to read event " << m_currentEvent << endmsg;
    throw std::runtime_error("LCEvent could not be read");
  }

  // Since this is presumably the first algorithm to run we have to check here
  // to see if we need to stop the run. Events in flight will still be
  // processed, so technically this signals that we want to end the run after
  // all of the algorithms for this event have finished.
  if (m_currentEvent >= m_numberOfEvents) {
    info() << "This is the last event in the input files. Stopping the run" << endmsg;
    auto evtProcService = service<IEventProcessor>("ApplicationMgr", false);
    if (!evtProcService) {
      fatal() << "Could not get the ApplicationMgr for stopping the run" << endmsg;
    }
    if (evtProcService->stopRun().isFailure()) {
      error() << "Out of events, but could not signal to stop the run" << endmsg;
    }
  }

  auto             myEvWr = new LCEventWrapper(std::move(theEvent));
  const StatusCode sc     = eventSvc()->registerObject("/Event/LCEvent", myEvWr);
  if (sc.isFailure()) {
    error() << "Failed to store the LCEvent" << endmsg;
    return sc;
  }

  return StatusCode::SUCCESS;
}
