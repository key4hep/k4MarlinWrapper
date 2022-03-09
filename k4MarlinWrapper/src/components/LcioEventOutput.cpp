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

////////////////////////////////////////////
// Init MT writer and write mode
////////////////////////////////////////////
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
  } else if (m_write_mode == "") {
    m_writer->open(m_filename);
  } else {
    error() << "Unrecognized option \"" << m_write_mode << "\" for WriteMode." << endmsg;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

////////////////////////////////////////////
// Go over all collections and drop based on input parameters
////////////////////////////////////////////
void LcioEventOutput::dropCollections(lcio::LCEventImpl* event, std::vector<lcio::LCCollectionVec*>& subsets) {
  const std::vector<std::string>* evt_coll_names = event->getCollectionNames();

  bool th_drop, ch_drop = false;

  // Check if there are TrackerHits or CaloHits
  for (const auto& coll_type : m_drop_coll_types) {
    if (coll_type == lcio::LCIO::TRACKERHIT) {
      th_drop = true;
    } else if (coll_type == lcio::LCIO::CALORIMETERHIT) {
      ch_drop = true;
    }
  }

  for (const auto& evt_coll_name : *evt_coll_names) {
    auto* evt_coll      = dynamic_cast<lcio::LCCollectionVec*>(event->getCollection(evt_coll_name));
    auto  evt_coll_type = evt_coll->getTypeName();

    // Drop types
    for (const auto& drop_type : m_drop_coll_types) {
      if (drop_type == evt_coll_type) {
        evt_coll->setTransient(true);
      }
    }

    // Drop names
    for (const auto& drop_name : m_drop_coll_names) {
      if (drop_name == evt_coll_name) {
        evt_coll->setTransient(true);
      }
    }

    // Keep names
    for (const auto& keep_name : m_keep_coll_names) {
      if (keep_name == evt_coll_name) {
        evt_coll->setTransient(false);
      }
    }

    // Un-set subsets
    for (const auto& subset_coll : m_full_subset_colls) {
      if (subset_coll == evt_coll_name) {
        if (evt_coll->isSubset()) {
          evt_coll->setSubset(false);
          subsets.push_back(evt_coll);
        }
      }
    }

    // don't store hit pointers if hits are droped
    if (evt_coll_type == lcio::LCIO::TRACK && th_drop) {
      std::bitset<32> flag(evt_coll->getFlag());
      flag[lcio::LCIO::TRBIT_HITS] = 0;
      evt_coll->setFlag(flag.to_ulong());
    }
    if (evt_coll_type == lcio::LCIO::CLUSTER && ch_drop) {
      std::bitset<32> flag(evt_coll->getFlag());
      flag[lcio::LCIO::CLBIT_HITS] = 0;
      evt_coll->setFlag(flag.to_ulong());
    }
  }
}

////////////////////////////////////////////
// Revert subsets marked while dropping collections
////////////////////////////////////////////
void LcioEventOutput::revertSubsets(const std::vector<lcio::LCCollectionVec*>& subsets) {
  // revert subset flag - if any
  for (auto& subset : subsets) {
    (*subset).setSubset(true);
  }
}

////////////////////////////////////////////
// Get LCIO event from event service, write it to output file
////////////////////////////////////////////
StatusCode LcioEventOutput::execute() {
  // Get event
  DataObject*        pObject   = nullptr;
  StatusCode         sc        = evtSvc()->retrieveObject("/Event/LCEvent", pObject);
  lcio::LCEventImpl* the_event = dynamic_cast<IMPL::LCEventImpl*>(static_cast<LCEventWrapper*>(pObject)->getEvent());

  std::vector<lcio::LCCollectionVec*> subsets{};

  dropCollections(the_event, subsets);
  m_writer->writeEvent(the_event);
  revertSubsets(subsets);

  return StatusCode::SUCCESS;
}

////////////////////////////////////////////
// Clean-up resources
////////////////////////////////////////////
StatusCode LcioEventOutput::finalize() {
  // Cleanup
  m_writer->close();
  delete (m_writer);

  return StatusCode::SUCCESS;
}
