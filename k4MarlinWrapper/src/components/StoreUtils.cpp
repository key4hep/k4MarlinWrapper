/*
 * Copyright (c) 2019-2024 Key4hep-Project.
 *
 * This file is part of Key4hep.
 * See https://key4hep.github.io/key4hep-doc/ for further info.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "StoreUtils.h"

#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/AnyDataWrapper.h"
#include "GaudiKernel/IDataManagerSvc.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/SmartDataPtr.h"

#include "podio/Frame.h"

#include "k4FWCore/FunctionalUtils.h"

#include <stdexcept>
#include <string>
#include <vector>

// This is a reimplementation of functionality to retrieve collections from the
// store that can be found in Writer.cpp in k4FWCore with some modifications
// that are specific to the usage of this function in the converters, like
// returning also a map from collection ID to collection name
std::vector<std::string> getAvailableCollectionsFromStore(const AlgTool*                                  thisClass,
                                                          std::optional<std::map<uint32_t, std::string>>& idToName,
                                                          bool returnFrameCollections) {
  std::vector<std::string> collectionNames;

  SmartIF<IDataManagerSvc> mgr;
  mgr = thisClass->evtSvc();

  SmartDataPtr<DataObject> root(thisClass->evtSvc(), "/Event");
  if (!root) {
    thisClass->error() << "Failed to retrieve root object /Event" << endmsg;
  }

  auto pObj = root->registry();
  if (!pObj) {
    throw std::runtime_error("Failed to retrieve the root registry object");
  }
  std::vector<IRegistry*> leaves;
  StatusCode              sc = mgr->objectLeaves(pObj, leaves);
  if (!sc.isSuccess()) {
    throw std::runtime_error("Failed to retrieve object leaves");
  }
  for (const auto& pReg : leaves) {
    if (pReg->name() == k4FWCore::frameLocation) {
      if (!returnFrameCollections)
        continue;
      auto wrapper = dynamic_cast<AnyDataWrapper<podio::Frame>*>(pReg->object());
      if (!wrapper) {
        throw std::runtime_error("Could not cast object to Frame");
      }
      for (const auto& name : wrapper->getData().getAvailableCollections()) {
        collectionNames.push_back(name);
      }
    }
    DataObject* p;
    sc = thisClass->evtSvc()->retrieveObject("/Event" + pReg->name(), p);
    if (sc.isFailure()) {
      thisClass->error() << "Could not retrieve object " << pReg->name() << " from the EventStore" << endmsg;
    }
    auto*            functionalWrapper = dynamic_cast<AnyDataWrapper<std::unique_ptr<podio::CollectionBase>>*>(p);
    DataWrapperBase* algorithmWrapper  = nullptr;
    if (!functionalWrapper) {
      // This may be a collection created by a Gaudi::Algorithm
      algorithmWrapper = dynamic_cast<DataWrapperBase*>(p);
      if (!algorithmWrapper) {
        // This can happen for objects that are not collections like in the
        // MarlinWrapper for converter maps or a LCEvent, or, in general,
        // anything else
        continue;
      }
    }
    // Remove the leading /
    collectionNames.push_back(pReg->name().substr(1, pReg->name().size() - 1));
    if (idToName) {
      if (functionalWrapper) {
        idToName->emplace(functionalWrapper->getData()->getID(), pReg->name());
      } else {
        idToName->emplace(algorithmWrapper->collectionBase()->getID(), pReg->name());
      }
    }
  }
  return collectionNames;
}
