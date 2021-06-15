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

#include "k4MarlinWrapper/MarlinProcessorWrapper.h"

DECLARE_COMPONENT(MarlinProcessorWrapper)

MarlinProcessorWrapper::MarlinProcessorWrapper(const std::string& name, ISvcLocator* pSL) : GaudiAlgorithm(name, pSL) {
  // register log level names with the logstream ---------
  streamlog::out.addLevelName<streamlog::DEBUG>();
  streamlog::out.addLevelName<streamlog::DEBUG0>();
  streamlog::out.addLevelName<streamlog::DEBUG1>();
  streamlog::out.addLevelName<streamlog::DEBUG2>();
  streamlog::out.addLevelName<streamlog::DEBUG3>();
  streamlog::out.addLevelName<streamlog::DEBUG4>();
  streamlog::out.addLevelName<streamlog::DEBUG5>();
  streamlog::out.addLevelName<streamlog::DEBUG6>();
  streamlog::out.addLevelName<streamlog::DEBUG7>();
  streamlog::out.addLevelName<streamlog::DEBUG8>();
  streamlog::out.addLevelName<streamlog::DEBUG9>();
  streamlog::out.addLevelName<streamlog::MESSAGE>();
  streamlog::out.addLevelName<streamlog::MESSAGE0>();
  streamlog::out.addLevelName<streamlog::MESSAGE1>();
  streamlog::out.addLevelName<streamlog::MESSAGE2>();
  streamlog::out.addLevelName<streamlog::MESSAGE3>();
  streamlog::out.addLevelName<streamlog::MESSAGE4>();
  streamlog::out.addLevelName<streamlog::MESSAGE5>();
  streamlog::out.addLevelName<streamlog::MESSAGE6>();
  streamlog::out.addLevelName<streamlog::MESSAGE7>();
  streamlog::out.addLevelName<streamlog::MESSAGE8>();
  streamlog::out.addLevelName<streamlog::MESSAGE9>();
  streamlog::out.addLevelName<streamlog::WARNING>();
  streamlog::out.addLevelName<streamlog::WARNING0>();
  streamlog::out.addLevelName<streamlog::WARNING1>();
  streamlog::out.addLevelName<streamlog::WARNING2>();
  streamlog::out.addLevelName<streamlog::WARNING3>();
  streamlog::out.addLevelName<streamlog::WARNING4>();
  streamlog::out.addLevelName<streamlog::WARNING5>();
  streamlog::out.addLevelName<streamlog::WARNING6>();
  streamlog::out.addLevelName<streamlog::WARNING7>();
  streamlog::out.addLevelName<streamlog::WARNING8>();
  streamlog::out.addLevelName<streamlog::WARNING9>();
  streamlog::out.addLevelName<streamlog::ERROR>();
  streamlog::out.addLevelName<streamlog::ERROR0>();
  streamlog::out.addLevelName<streamlog::ERROR1>();
  streamlog::out.addLevelName<streamlog::ERROR2>();
  streamlog::out.addLevelName<streamlog::ERROR3>();
  streamlog::out.addLevelName<streamlog::ERROR4>();
  streamlog::out.addLevelName<streamlog::ERROR5>();
  streamlog::out.addLevelName<streamlog::ERROR6>();
  streamlog::out.addLevelName<streamlog::ERROR7>();
  streamlog::out.addLevelName<streamlog::ERROR8>();
  streamlog::out.addLevelName<streamlog::ERROR9>();
  streamlog::out.addLevelName<streamlog::SILENT>();

  declareProperty("EDM4hep2LcioTool", m_edm_conversionTool = nullptr);
  declareProperty("Lcio2EDM4hepTool", m_lcio_conversionTool = nullptr);
}

StatusCode MarlinProcessorWrapper::loadProcessorLibraries() const {
  // Load all libraries from the marlin_dll
  info() << "looking for marlindll" << endmsg;
  const char* const marlin_dll = getenv("MARLIN_DLL");
  if (marlin_dll == nullptr) {
    warning() << "MARLIN_DLL not set, not loading any processors " << endmsg;
  } else {
    info() << "Found marlin_dll " << marlin_dll << endmsg;
    const std::string marlin_dll_str(marlin_dll);
    std::regex re{":+"};
    std::vector<std::string> libraries = k4MW::util::split(marlin_dll_str, re);
    if (libraries.back().empty())
      libraries.pop_back();
    for (const auto& library : libraries) {
      info() << "Loading library " << library << endmsg;
      auto ret = gSystem->Load(library.c_str());
      if (ret < 0) {
        error() << "Failed to load " << library << "   " << gSystem->GetErrorStr() << endmsg;
        return StatusCode::FAILURE;
      }
    }
  }
  return StatusCode::SUCCESS;
}


std::shared_ptr<marlin::StringParameters> MarlinProcessorWrapper::parseParameters(
  const Gaudi::Property<std::map<std::string, std::vector<std::string>>>& parameters,
  std::string& verbosity) const
{
  auto parameters_ptr = std::make_shared<marlin::StringParameters>();
  info() << "Parameter values for: " << name() << " of type " << std::string(m_processorType) << endmsg;
  std::vector<std::string> parameterValues = {};

  // convert the list of string into parameter name and value
  for (const auto& [paramName, paramValues] : parameters) {
    if (paramName == "Verbosity") {
      info() << "Setting verbosity to " << paramName << endmsg;
      verbosity = paramName;
    }

    for (auto& value : paramValues) {
      // split to keep track of case where constants replacing merges various params into one string
      auto split_parameter = k4MW::util::split(value);
      parameterValues.insert(parameterValues.end(), split_parameter.begin(), split_parameter.end());
    }

    parameters_ptr->add(paramName, parameterValues);
    parameterValues.clear();
  }

  return parameters_ptr;
}


StatusCode MarlinProcessorWrapper::instantiateProcessor(
  std::shared_ptr<marlin::StringParameters>& parameters,
  Gaudi::Property<std::string>& processorTypeStr)
{
  auto processorType = marlin::ProcessorMgr::instance()->getProcessor(processorTypeStr);
  if (not processorType) {
    error() << " Failed to instantiate " << name() << endmsg;
    return StatusCode::FAILURE;
  }
  m_processor = processorType->newProcessor();
  if (not m_processor) {
    error() << " Failed to instantiate " << name() << endmsg;
    return StatusCode::FAILURE;
  }
  info() << "new processor " << m_processor << endmsg;
  m_processor->setName(name());
  m_processor->setParameters(parameters);
  ProcessorStack().push(m_processor);
  return StatusCode::SUCCESS;
}

StatusCode MarlinProcessorWrapper::initialize() {
  // initalize global marlin information, maybe betters as a _tool_
  static bool once = true;
  if (once) {
    once = false;
    streamlog::out.init(std::cout, "k4MarlinWrapper");
    marlin::Global::parameters = new marlin::StringParameters();
    marlin::Global::parameters->add("AllowToModifyEvent", {"true"});
    marlin::Global::parameters->add("RandomSeed", {"123456"});
    // marlin::Global::EVENTSEEDER = new marlin::ProcessorEventSeeder() ;
    if (loadProcessorLibraries().isFailure()) {
      return StatusCode::FAILURE;
    }
  }

  auto parameters = parseParameters(m_parameters, m_verbosity);
  if (instantiateProcessor(parameters, m_processorType).isFailure()) {
    return StatusCode::FAILURE;
  }

  streamlog::logscope scope(streamlog::out);
  scope.setName(name());
  scope.setLevel(m_verbosity);

  info() << "init " << endmsg;

  // initialize the processor
  m_processor->init();

  info() << "Init processor " << endmsg;
  return StatusCode::SUCCESS;
}

StatusCode MarlinProcessorWrapper::execute() {

  // Get Event
  info() << "Getting the event for " << m_processor->name() << endmsg;
  DataObject* pObject = nullptr;
  StatusCode  sc      = eventSvc()->retrieveObject("/Event/LCEvent", pObject);

  lcio::LCEventImpl* the_event = nullptr;

  if (sc.isFailure()) {
    the_event = new lcio::LCEventImpl();
    // Register empty event
    debug() << "Registering conversion EDM4hep to LCIO event in TES" << endmsg;
    auto pO = std::make_unique<LCEventWrapper>(the_event, true);
    StatusCode reg_sc = evtSvc()->registerObject("/Event/LCEvent", pO.release());
    if (reg_sc.isFailure()) {
      error() << "Failed to store the EDM4hep to LCIO event" << endmsg;
      return reg_sc;
    }
  } else {
    debug() << "LCEvent retrieved successfully" << endmsg;
    the_event =
      dynamic_cast<IMPL::LCEventImpl*>(static_cast<LCEventWrapper*>(pObject)->getEvent());
  }

  // Found EDM Conversion tool
  if (!m_edm_conversionTool.empty()) {
    StatusCode edm_sc =  m_edm_conversionTool->convertCollections(the_event);
    if (edm_sc.isFailure()) {
      error() << "Failed converting EDM4hep to LCIO collection " << endmsg;
    }
  }

  // call the refreshSeeds via the processor manager
  // FIXME: this is an overkill, but we need to call this once per event, not once for each execute call
  // how can this be done more efficiently?
  auto* procMgr = marlin::ProcessorMgr::instance();
  procMgr->modifyEvent(the_event);

  streamlog::logscope scope(streamlog::out);
  scope.setName(name());
  scope.setLevel(m_verbosity);

  //process the event in the processor
  auto modifier = dynamic_cast<marlin::EventModifier*>(m_processor);
  if (modifier) {
    modifier->modifyEvent(the_event);
  } else {
    m_processor->processEvent(the_event);
  }

  // Found LCIO Conversion tool
  if (!m_lcio_conversionTool.empty()) {
    StatusCode lcio_sc =  m_lcio_conversionTool->convertCollections(the_event);
    if (lcio_sc.isFailure()) {
      error() << "Failed converting LCIO to EDM4hep collection " << endmsg;
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode MarlinProcessorWrapper::finalize() {
  // need to call processors in reverse order
  auto processor = ProcessorStack().top();
  ProcessorStack().pop();
  info() << "Finalising " << processor->name() << endmsg;

  streamlog::logscope scope(streamlog::out);
  scope.setName(processor->name());
  // Should get verbosity from actual processor being ended
  scope.setLevel(m_verbosity);

  // finalize the processor
  processor->end();

  return Algorithm::finalize();
}
