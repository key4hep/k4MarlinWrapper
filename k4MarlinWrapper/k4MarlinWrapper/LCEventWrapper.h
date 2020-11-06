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

#pragma once

#include <EVENT/LCEvent.h>

#include <GaudiKernel/DataObject.h>

class LCEventWrapper : public DataObject {
public:
  LCEventWrapper(EVENT::LCEvent* theEvent) : m_event(theEvent) {}

  //m_event is still owned by LCIO
  ~LCEventWrapper(){};

  EVENT::LCEvent* getEvent() const { return m_event; }

private:
  EVENT::LCEvent* m_event = nullptr;
};
