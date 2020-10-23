/** MarlinProcessorWrapper.h ---
 *
 * Copyright (C) 2019 Andre Sailer
 *
 * Author: Andre Sailer <andre.philippe.sailer@cern.ch>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see http://www.gnu.org/licenses/.
 */

#pragma once
#include <GaudiAlg/GaudiAlgorithm.h>

#include <streamlog/logbuffer.h>
#include <streamlog/streamlog.h>

#include <stack>

#include <../src/util/gmpUtil.h>

namespace marlin {
  class Processor;
  class StringParameters;
}  // namespace marlin

class MarlinProcessorWrapper : public GaudiAlgorithm {
public:
  explicit MarlinProcessorWrapper(const std::string& name, ISvcLocator* pSL);
  virtual ~MarlinProcessorWrapper() = default;
  virtual StatusCode execute() override final;
  virtual StatusCode finalize() override final;
  virtual StatusCode initialize() override final;

private:
  streamlog::logstream  m_logstream;
  streamlog::logbuffer* m_lb        = nullptr;
  std::string           m_verbosity = "MESSAGE";
  marlin::Processor*    m_processor = nullptr;

  /// Load libraries specified by MARLIN_DLL environment variable
  StatusCode loadProcessorLibraries() const;

  /// Instantiate the Marlin processor and assign name and parameters
  StatusCode instantiateProcessor(
    std::shared_ptr<marlin::StringParameters>& parameters,
    Gaudi::Property<std::string>& processorTypeStr);

  /// Parse the parameters from the Property
  std::shared_ptr<marlin::StringParameters> parseParameters(
    const Gaudi::Property<std::vector<std::string>>& parameters,
    std::string& verbosity) const;

  /// ProcessorType: The Type of the MarlinProcessor to use
  Gaudi::Property<std::string> m_processorType{this, "ProcessorType", {}};
  /// Parameters: Dictionary of key and list of strings would be nice, but we just use a vector of strings for the moment
  Gaudi::Property<std::vector<std::string>> m_parameters{this, "Parameters", {}};

  static std::stack<marlin::Processor*>& ProcessorStack();
};

std::stack<marlin::Processor*>& MarlinProcessorWrapper::ProcessorStack() {
  static std::stack<marlin::Processor*> stack;
  return stack;
}
