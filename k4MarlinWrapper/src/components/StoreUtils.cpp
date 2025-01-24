#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/AnyDataWrapper.h"
#include "GaudiKernel/IDataManagerSvc.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/SmartDataPtr.h"

#include "StoreUtils.h"
#include "podio/Frame.h"

#include "k4FWCore/FunctionalUtils.h"

#include <vector>
#include <string>

std::vector<std::string> getAvailableCollectionsFromStore(const AlgTool*                                  thisClass,
                                                          std::optional<std::map<uint32_t, std::string>>& idToName,
                                                          bool returnFrameCollections
                                                          ) {
  std::vector<std::string> collectionNames;

  SmartIF<IDataManagerSvc> mgr;
  mgr = thisClass->evtSvc();

  SmartDataPtr<DataObject> root(thisClass->evtSvc(), "/Event");
  if (!root) {
    thisClass->error() << "Failed to retrieve root object /Event" << endmsg;
  }

  auto pObj = root->registry();
  if (!pObj) {
    thisClass->error() << "Failed to retrieve the root registry object" << endmsg;
  }
  std::vector<IRegistry*> leaves;
  StatusCode              sc = mgr->objectLeaves(pObj, leaves);
  if (!sc.isSuccess()) {
    thisClass->error() << "Failed to retrieve object leaves" << endmsg;
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
    auto wrapper = dynamic_cast<AnyDataWrapper<std::unique_ptr<podio::CollectionBase>>*>(p);
    if (!wrapper) {
      continue;
    }
    // Remove the leading /
    collectionNames.push_back(pReg->name().substr(1, pReg->name().size() - 1));
    if (idToName) {
      idToName->emplace(wrapper->getData()->getID(), pReg->name());
    }
  }
  return collectionNames;
}
