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

#ifndef K4MARLINWRAPPER_LCIOEVENTALGO_H
#define K4MARLINWRAPPER_LCIOEVENTALGO_H

/***
 * LCEventAlgo: place the LCEvent from lcio file in the Gaudi Datastore for
 * later retrieval by wrapped Marlin Processors
 */

#include <iostream>
#include <memory>

#include <GaudiAlg/GaudiAlgorithm.h>
#include <GaudiKernel/IEventProcessor.h>

#include <EVENT/LCIO.h>
#include <MT/LCReader.h>

#include "k4MarlinWrapper/LCEventWrapper.h"

class LcioEvent : public GaudiAlgorithm {
public:
  explicit LcioEvent(const std::string& name, ISvcLocator* pSL);
  virtual ~LcioEvent() = default;
  virtual StatusCode initialize() override final;
  virtual StatusCode execute() override final;

private:
  Gaudi::Property<std::vector<std::string>> m_fileNames{this, "Files", {}};
  MT::LCReader*                             m_reader = nullptr;
};

#endif