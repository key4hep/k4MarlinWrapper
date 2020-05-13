/**
 *   Copyright 2019 CERN
 *
 *   Licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 *
 *   In applying this licence, CERN does not waive the privileges and immunities
 *   granted to it by virtue of its status as an Intergovernmental Organization
 *   or submit itself to any jurisdiction. 
 *
 */

#include "LcioEventAlgo.h"
#include <EVENT/LCIO.h>
#include <LCEventWrapper.h>

#include <iostream>

DECLARE_COMPONENT(LcioEvent)

LcioEvent::LcioEvent(std::string const& name, ISvcLocator* pSL) : GaudiAlgorithm(name, pSL) {}

StatusCode LcioEvent::initialize() {
  m_reader = IOIMPL::LCFactory::getInstance()->createLCReader();
  m_reader->open(m_fileNames);
  info() << "Initialized the LcioEvent Algo: " << m_fileNames[0] << endmsg;
  return StatusCode::SUCCESS;
}

StatusCode LcioEvent::execute() {
  auto* theEvent = m_reader->readNextEvent(EVENT::LCIO::UPDATE);
  if (theEvent == nullptr) {
    return StatusCode::FAILURE;
  }

  // pass theEvent to the DataStore, so we can access them in our processor wrappers
  info() << "Reading from file: " << m_fileNames[0] << endmsg;

  auto*      pO = new LCEventWrapper(theEvent);
  StatusCode sc = eventSvc()->registerObject("/Event/LCEvent", pO);
  if (sc.isFailure()) {
    error() << "Failed to store the LCEvent" << endmsg;
    return sc;
  }
  return StatusCode::SUCCESS;
}

StatusCode LcioEvent::finalize() { return StatusCode::SUCCESS; }
