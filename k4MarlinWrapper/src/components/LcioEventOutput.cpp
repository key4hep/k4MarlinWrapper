/**
 *   Copyright 2021 CERN
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

#include "k4MarlinWrapper/LcioEventOutput.h"

DECLARE_COMPONENT(LcioEventOutput)

LcioEventOutput::LcioEventOutput(const std::string& name, ISvcLocator* pSL) : GaudiAlgorithm(name, pSL) {}

StatusCode LcioEventOutput::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize();
  if (sc.isFailure())
    return sc;

  // Init writer
  m_writer = new MT::LCWriter();
  // m_writer->setCompressionLevel();

  if (m_write_mode == "WRITE_APPEND") {
    m_writer->open(m_filename, EVENT::LCIO::WRITE_APPEND);
  } else if (m_write_mode == "WRITE_NEW") {
    m_writer->open(m_filename, EVENT::LCIO::WRITE_NEW);
  } else {
    m_writer->open(m_filename);
  }

  return StatusCode::SUCCESS;
}

StatusCode LcioEventOutput::execute() {
  // TODO filer collections

  // Get event
  DataObject* pObject = nullptr;
  StatusCode  sc      = evtSvc()->retrieveObject("/Event/LCEvent", pObject);

  lcio::LCEventImpl* the_event = dynamic_cast<IMPL::LCEventImpl*>(static_cast<LCEventWrapper*>(pObject)->getEvent());

  // Write event
  m_writer->writeEvent(the_event);

  return StatusCode::SUCCESS;
}

StatusCode LcioEventOutput::finalize() {
  // Cleanup
  delete (m_writer);

  return StatusCode::SUCCESS;
}
