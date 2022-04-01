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
    const std::string        marlin_dll_str(marlin_dll);
    std::regex               re{":+"};
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
    const Gaudi::Property<std::map<std::string, std::vector<std::string>>>& parameters, std::string& verbosity) const {
  auto parameters_ptr = std::make_shared<marlin::StringParameters>();
  info() << "Parameter values for: " << name() << " of type " << std::string(m_processorType) << endmsg;

  // convert the list of string into parameter name and value
  for (const auto& [paramName, paramValues] : parameters) {
    std::vector<std::string> parameterValues = {};
    if (paramName == "Verbosity") {
      info() << "Setting verbosity to " << paramValues[0] << endmsg;
      verbosity = paramValues[0];
    }

    for (auto& value : paramValues) {
      // split to keep track of case where constants replacing merges various
      // params into one string
      auto split_parameter = k4MW::util::split(value);
      parameterValues.insert(parameterValues.end(), split_parameter.begin(), split_parameter.end());
    }

    parameters_ptr->add(paramName, parameterValues);
  }

  return parameters_ptr;
}

StatusCode MarlinProcessorWrapper::instantiateProcessor(std::shared_ptr<marlin::StringParameters>& parameters,
                                                        Gaudi::Property<std::string>&              processorTypeStr) {
  auto processorType = marlin::ProcessorMgr::instance()->getProcessor(processorTypeStr);
  if (not processorType) {
    error() << " Failed to instantiate " << name() << " because processor type could not be determined" << endmsg;
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

    // Get seed set for random engine
    std::vector<long> got_seeds{};
    StatusCode        got_seeds_sc = randSvc()->engine()->seeds(got_seeds);
    if (!got_seeds_sc.isSuccess())
      warning() << "Random Service seeds could not be set" << endmsg;

    std::string marlin_seed = std::to_string(got_seeds[0]);
    info() << "Setting global Marlin random seed to " << marlin_seed << endmsg;
    marlin::Global::parameters->add("RandomSeed", {marlin_seed});

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
  // Get flag to know if there was an underlying LCEvent
  DataObject* pStatus  = nullptr;
  StatusCode  scStatus = eventSvc()->retrieveObject("/Event/LCEventStatus", pStatus);
  if (scStatus.isSuccess()) {
    bool hasLCEvent = static_cast<LCEventWrapperStatus*>(pStatus)->hasLCEvent;
    if (not hasLCEvent) {
      warning() << "LCIO Event reading returned nullptr, so MarlinProcessorWrapper won't execute" << endmsg;
      return StatusCode::SUCCESS;
    }
  }

  // Get LCIO Event
  debug() << "Retrieving LCIO Event for wrapped processor " << m_processor->name() << endmsg;
  DataObject*        pObject   = nullptr;
  StatusCode         sc        = eventSvc()->retrieveObject("/Event/LCEvent", pObject);
  lcio::LCEventImpl* the_event = nullptr;

  if (sc.isFailure()) {
    // Register empty event
    the_event = new lcio::LCEventImpl();
    debug() << "Registering empty Event for EDM4hep to LCIO conversion event in TES" << endmsg;
    auto       pO     = std::make_unique<LCEventWrapper>(the_event, true);
    StatusCode reg_sc = evtSvc()->registerObject("/Event/LCEvent", pO.release());
    if (reg_sc.isFailure()) {
      error() << "Failed to register empty LCIO Event" << endmsg;
      return reg_sc;
    }
  } else {
    debug() << "LCIO Event retrieved successfully" << endmsg;
    the_event = dynamic_cast<IMPL::LCEventImpl*>(static_cast<LCEventWrapper*>(pObject)->getEvent());
  }

  // EDM4hep->LCIO conversion
  if (!m_edm_conversionTool.empty()) {
    StatusCode edm_sc = m_edm_conversionTool->convertCollections(the_event);
    if (edm_sc.isFailure()) {
      error() << "Failed to convert EDM4hep to LCIO collections" << endmsg;
    }
  }

  // call the refreshSeeds via the processor manager
  // FIXME: this is an overkill, but we need to call this once per event, not
  // once for each execute call
  // how can this be done more efficiently?
  try {
    auto* procMgr = marlin::ProcessorMgr::instance();
    procMgr->modifyEvent(the_event);

    streamlog::logscope scope(streamlog::out);
    scope.setName(name());
    scope.setLevel(m_verbosity);

    // process the event in the processor
    auto modifier = dynamic_cast<marlin::EventModifier*>(m_processor);
    if (modifier) {
      modifier->modifyEvent(the_event);
    } else {
      m_processor->processEvent(the_event);
    }
  }
  // Handle exceptions that may come from Marlin
  catch (marlin::SkipEventException& e) {
    // Store flag to prevent the rest of the event from processing
    auto             pStatus  = std::make_unique<LCEventWrapperStatus>(false);
    const StatusCode scStatus = eventSvc()->registerObject("/Event/LCEventStatus", pStatus.release());
    if (scStatus.isFailure()) {
      error() << "Failed to store flag to skip event on Marlin marlin::SkipEventException" << endmsg;
      return scStatus;
    }

    error() << e.what() << endmsg;
    return StatusCode::FAILURE;
  } catch (marlin::StopProcessingException& e) {
    // Store flag to prevent the rest of the event from processing
    auto             pStatus  = std::make_unique<LCEventWrapperStatus>(false);
    const StatusCode scStatus = eventSvc()->registerObject("/Event/LCEventStatus", pStatus.release());
    if (scStatus.isFailure()) {
      error() << "Failed to store flag to skip event on Marlin marlin::StopProcessingException" << endmsg;
      return scStatus;
    }

    error() << e.what() << endmsg;

    // Send stop to EventProcessor
    IEventProcessor* evt = nullptr;
    if (service("ApplicationMgr", evt, true).isSuccess()) {
      evt->stopRun().ignore();
      evt->release();
    } else {
      abort();
    }

    return StatusCode::FAILURE;
  } catch (lcio::Exception& e) {
    error() << "There was an exception while processing " << m_processor->name() << ": " << e.what() << endmsg;
    return StatusCode::FAILURE;
  }

  // LCIO->EDM4hep conversion
  if (!m_lcio_conversionTool.empty()) {
    StatusCode lcio_sc = m_lcio_conversionTool->convertCollections(the_event);
    if (lcio_sc.isFailure()) {
      error() << "Failed to convert LCIO to EDM4hep collections" << endmsg;
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
