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

#ifndef K4MARLINWRAPPER_LCIOEVENTOUTPUT_H
#define K4MARLINWRAPPER_LCIOEVENTOUTPUT_H

////////////////////////////////////////////
// LCEventOutput: Write out LCIO events using MT writer from LCIO
////////////////////////////////////////////

#include <iostream>

#include <GaudiAlg/GaudiAlgorithm.h>

#include <EVENT/LCIO.h>
#include <IMPL/LCCollectionVec.h>
#include <IMPL/LCEventImpl.h>
#include <MT/LCWriter.h>
#include <lcio.h>

#include "k4MarlinWrapper/LCEventWrapper.h"

class LcioEventOutput : public GaudiAlgorithm {
public:
  explicit LcioEventOutput(const std::string& name, ISvcLocator* pSL);
  virtual ~LcioEventOutput() = default;
  virtual StatusCode initialize() override final;
  virtual StatusCode execute() override final;
  virtual StatusCode finalize() override final;

private:
  MT::LCWriter* m_writer = nullptr;

  Gaudi::Property<std::string>              m_write_mode{this, "WriteMode", {}};
  Gaudi::Property<std::string>              m_filename{this, "OutputFileName", {}};
  Gaudi::Property<std::vector<std::string>> m_drop_coll_names{this, "DropCollectionNames", {}};
  Gaudi::Property<std::vector<std::string>> m_keep_coll_names{this, "KeepCollectionNames", {}};
  Gaudi::Property<std::vector<std::string>> m_drop_coll_types{this, "DropCollectionTypes", {}};
  Gaudi::Property<std::vector<std::string>> m_full_subset_colls{this, "FullSubsetCollections", {}};

  void dropCollections(lcio::LCEventImpl* event, std::vector<lcio::LCCollectionVec*>& subsets);

  void revertSubsets(const std::vector<lcio::LCCollectionVec*>& subsets);
};

#endif