/**
 *   Copyright 2019 CERN
 *
 *  Author: Andre Sailer <andre.philippe.sailer@cern.ch>
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

#include "MarlinProcessorWrapper.h"
#include <LCEventWrapper.h>

#include <GaudiKernel/MsgStream.h>

#include <EVENT/LCEvent.h>

#include <marlin/ProcessorMgr.h>
#include <marlin/StringParameters.h>
#include <streamlog/streamlog.h>

#include <marlin/Global.h>

#include <iostream>
#include <string>

DECLARE_COMPONENT(MarlinProcessorWrapper)

MarlinProcessorWrapper::MarlinProcessorWrapper(std::string const& name, ISvcLocator* pSL) : GaudiAlgorithm(name, pSL) {}

StatusCode MarlinProcessorWrapper::initialize() {
  // initalize global marlin information, maybe betters as a _tool_
  static bool once = true;
  if (once) {
    streamlog::out.init(std::cout, "GMP");
    once                       = false;
    marlin::Global::parameters = new marlin::StringParameters();
    marlin::Global::parameters->add("RandomSeed", {"123456"});
  }

  auto  parameters = std::make_shared<marlin::StringParameters>();
  auto* procMgr    = marlin::ProcessorMgr::instance();

  //parse the parameters from the Property
  info() << "Parameter values for: " << name() << " of type " << std::string(m_processorType) << endmsg;
  std::string              parameterName   = "";
  std::vector<std::string> parameterValues = {};

  //convert the list of string into parameter name and value
  for (auto const& parameterString : m_parameters) {
    if (parameterString == "END_TAG") {
      parameters->add(parameterName, parameterValues);

      info() << parameterName;
      for (auto const& value : parameterValues) {
        info() << "  " << value;
      }
      info() << endmsg;

      parameterName = "";
      parameterValues.clear();

      continue;
    }

    if (parameterName == "") {
      parameterName = parameterString;
      continue;
    }

    parameterValues.push_back(parameterString);
  }

  streamlog::logscope scope(streamlog::out);
  scope.setName(name());
  scope.setLevel("DEBUG");

  // instantiate the Marlin processor and assign name and parameters
  m_processor = procMgr->getProcessor(m_processorType)->newProcessor();
  info() << "new processor " << m_processor << endmsg;
  if (not m_processor) {
    error() << " Failed to instantiate " << name() << endmsg;
    return StatusCode::FAILURE;
  }
  m_processor->setName(name());
  m_processor->setParameters(parameters);

  //initialize the processor
  m_processor->init();
  return StatusCode::SUCCESS;
}

StatusCode MarlinProcessorWrapper::execute() {
  info() << "Getting the event for " << m_processor->name() << endmsg;
  DataObject* pObject = nullptr;
  StatusCode  sc      = eventSvc()->retrieveObject("/Event/LCEvent", pObject);
  if (sc.isFailure()) {
    error() << "Failed to retrieve the LCEvent " << endmsg;
    return sc;
  }

  auto* theEvent = static_cast<LCEventWrapper*>(pObject)->getEvent();

  streamlog::logscope scope(streamlog::out);
  scope.setName(name());
  scope.setLevel("DEBUG");

  //process the event in the processor
  m_processor->processEvent(theEvent);

  return StatusCode::SUCCESS;
}

StatusCode MarlinProcessorWrapper::finalize() { return StatusCode::SUCCESS; }
