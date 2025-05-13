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
#include "k4MarlinWrapper/converters/Lcio2EDM4hep.h"
#include "GlobalConvertedObjectsMap.h"
#include "StoreUtils.h"

#include <EVENT/LCCollection.h>
#include <Exceptions.h>

#include "k4EDM4hep2LcioConv/k4Lcio2EDM4hepConv.h"

#include <edm4hep/utils/ParticleIDUtils.h>

#include <k4FWCore/DataHandle.h>
#include <k4FWCore/FunctionalUtils.h>
#include <k4FWCore/MetaDataHandle.h>
#include <k4FWCore/PodioDataSvc.h>

#include "GaudiKernel/AnyDataWrapper.h"

#include <memory>

DECLARE_COMPONENT(Lcio2EDM4hepTool);

using namespace k4MarlinWrapper;

Lcio2EDM4hepTool::Lcio2EDM4hepTool(const std::string& type, const std::string& name, const IInterface* parent)
    : AlgTool(type, name, parent), m_eventDataSvc("EventDataSvc", "Lcio2EDM4hepTool") {
  declareInterface<IEDMConverter>(this);

  StatusCode sc = m_eventDataSvc.retrieve();
  if (sc.isFailure()) {
    error() << "Could not retrieve EventDataSvc" << endmsg;
  }
}

StatusCode Lcio2EDM4hepTool::initialize() {
  m_podioDataSvc = dynamic_cast<PodioDataSvc*>(m_eventDataSvc.get());

  if (!m_podioDataSvc) {
    m_metadataSvc = service("MetadataSvc", false);
    if (!m_metadataSvc) {
      error() << "Could not retrieve MetadataSvc" << endmsg;
      return StatusCode::FAILURE;
    }
  }

  return AlgTool::initialize();
}

StatusCode Lcio2EDM4hepTool::finalize() { return AlgTool::finalize(); }

// **********************************
// Check if a collection was already registered to skip it
// **********************************
bool Lcio2EDM4hepTool::collectionExist(const std::string& collection_name) {
  std::vector<std::string> collections;
  if (m_podioDataSvc) {
    collections = m_podioDataSvc->getEventFrame().getAvailableCollections();
  } else {
    std::optional<std::map<uint32_t, std::string>> dummy = std::nullopt;
    collections = getAvailableCollectionsFromStore(this, dummy, true);
  }
  if (std::find(collections.begin(), collections.end(), collection_name) != collections.end()) {
    debug() << "Collection named " << collection_name << " already registered, skipping conversion." << endmsg;
    return true;
  }
  return false;
}

void Lcio2EDM4hepTool::registerCollection(
    std::tuple<const std::string&, std::unique_ptr<podio::CollectionBase>> namedColl, EVENT::LCCollection* lcioColl) {
  auto& [name, e4hColl] = namedColl;
  if (!e4hColl) {
    error() << "Could not convert collection " << name << endmsg;
    return;
  }

  e4hColl->setID(k4FWCore::details::getCollectionID(name));
  debug() << fmt::format("Adding collection '{}' with collection id: {:0>8x}", name, e4hColl->getID()) << endmsg;
  auto wrapper = new AnyDataWrapper<std::unique_ptr<podio::CollectionBase>>(std::move(e4hColl));
  // No need to check for pre-existing collections, since we only ever end up
  // here if that is not the case
  auto sc = m_eventDataSvc->registerObject("/Event", "/" + std::string(name), wrapper);
  if (sc == StatusCode::FAILURE) {
    error() << "Could not register collection " << name << endmsg;
  }

  // Convert metadata
  if (lcioColl != nullptr) {
    std::vector<std::string> string_keys = {};
    lcioColl->getParameters().getStringKeys(string_keys);

    for (auto& elem : string_keys) {
      if (elem == edm4hep::labels::CellIDEncoding) {
        const auto& lcio_coll_cellid_str = lcioColl->getParameters().getStringVal(lcio::LCIO::CellIDEncoding);
        if (m_podioDataSvc) {
          auto& mdFrame = m_podioDataSvc->getMetaDataFrame();
          mdFrame.putParameter(podio::collMetadataParamName(name, edm4hep::labels::CellIDEncoding),
                               lcio_coll_cellid_str);
        } else {
          k4FWCore::putParameter(podio::collMetadataParamName(name, edm4hep::labels::CellIDEncoding),
                                 lcio_coll_cellid_str);
        }
        debug() << "Storing CellIDEncoding " << podio::collMetadataParamName(name, edm4hep::labels::CellIDEncoding)
                << " value: " << lcio_coll_cellid_str << endmsg;
      } else {
        // TODO: figure out where this actually needs to go
      }
    }
  }
}

namespace {
template <typename K, typename V>
using ObjMapT = k4EDM4hep2LcioConv::VecMapT<K, V>;

struct ObjectMappings {
  ObjMapT<lcio::Track*, edm4hep::MutableTrack> tracks{};
  ObjMapT<lcio::TrackerHit*, edm4hep::MutableTrackerHit3D> trackerHits{};
  ObjMapT<lcio::SimTrackerHit*, edm4hep::MutableSimTrackerHit> simTrackerHits{};
  ObjMapT<lcio::CalorimeterHit*, edm4hep::MutableCalorimeterHit> caloHits{};
  ObjMapT<lcio::RawCalorimeterHit*, edm4hep::MutableRawCalorimeterHit> rawCaloHits{};
  ObjMapT<lcio::SimCalorimeterHit*, edm4hep::MutableSimCalorimeterHit> simCaloHits{};
  ObjMapT<lcio::TPCHit*, edm4hep::MutableRawTimeSeries> tpcHits{};
  ObjMapT<lcio::Cluster*, edm4hep::MutableCluster> clusters{};
  ObjMapT<lcio::Vertex*, edm4hep::MutableVertex> vertices{};
  ObjMapT<lcio::ReconstructedParticle*, edm4hep::MutableReconstructedParticle> recoParticles{};
  ObjMapT<lcio::MCParticle*, edm4hep::MutableMCParticle> mcParticles{};
  ObjMapT<lcio::TrackerHitPlane*, edm4hep::MutableTrackerHitPlane> trackerHitPlanes{};
  ObjMapT<lcio::ParticleID*, edm4hep::MutableParticleID> particleIDs{};
};
} // namespace

StatusCode Lcio2EDM4hepTool::convertCollections(lcio::LCEventImpl* the_event) {
  debug() << "Converting from EDM4hep to LCIO" << endmsg;
  // Convert event parameters
  if (m_podioDataSvc) {
    debug() << "Converting event parameters directly into the Frame of the PodioDataSvc" << endmsg;
    LCIO2EDM4hepConv::convertObjectParameters(the_event, m_podioDataSvc->m_eventframe);
  } else {
    DataObject* p;
    StatusCode code = m_eventDataSvc->retrieveObject("/Event" + k4FWCore::frameLocation, p);
    if (code.isSuccess()) {
      auto* frameWrapper = dynamic_cast<AnyDataWrapper<podio::Frame>*>(p);
      debug() << "Converting event parameters into the Frame stored in the TES" << endmsg;
      LCIO2EDM4hepConv::convertObjectParameters(the_event, frameWrapper->getData());
    } else {
      warning() << "Could not retrieve the event frame; event parameters will not be converted. This is a known "
                   "limitation when running with IOSvc without an input file."
                << endmsg;
    }
  }

  // Convert Event Header outside the collections loop
  if (!collectionExist(edm4hep::labels::EventHeader)) {
    debug() << "Converting the EventHeader" << endmsg;
    registerCollection(edm4hep::labels::EventHeader, LCIO2EDM4hepConv::createEventHeader(the_event));
  }

  // Start off with the pre-defined collection name mappings
  auto collsToConvert{m_collNames.value()};
  if (m_convertAll) {
    info() << "Converting all collections from LCIO to EDM4hep" << endmsg;
    const auto* collections = the_event->getCollectionNames();
    for (const auto& collName : *collections) {
      // And simply add the rest, exploiting the fact that emplace will not
      // replace existing entries with the same key
      debug() << "Adding '" << collName << "' to be converted from the LCIO Event" << endmsg;
      collsToConvert.emplace(collName, collName);
    }
  }

  auto lcio2edm4hepMaps = ::ObjectMappings{};

  std::vector<std::pair<std::string, EVENT::LCCollection*>> lcRelationColls{};
  std::vector<std::tuple<std::string, EVENT::LCCollection*, std::string>> subsetColls{};

  // If we convert any SimCalorimeterHit collection we also need the
  // CaloHitContributions in the end, even if all SimCalorimeterHit collections
  // were empty
  bool needCaloHitContribs = false;

  std::map<std::string, edm4hep::utils::ParticleIDMeta> pidInfos{};

  for (const auto& [lcioName, edm4hepName] : collsToConvert) {
    try {
      auto* lcio_coll = the_event->getCollection(lcioName);
      debug() << "Converting collection " << lcioName << " (storing it as " << edm4hepName << "). " << endmsg;
      if (collectionExist(edm4hepName)) {
        debug() << "Collection already exists, skipping." << endmsg;
        continue; // No need to convert again
      }
      const auto& lcio_coll_type_str = lcio_coll->getTypeName();
      debug() << "LCIO type of the collection is " << lcio_coll_type_str << endmsg;

      // We deal with subset collections and LCRelations once we have all data
      // converted
      if (lcio_coll->isSubset()) {
        subsetColls.emplace_back(edm4hepName, lcio_coll, lcio_coll_type_str);
        continue;
      }
      if (lcio_coll_type_str == "LCRelation") {
        lcRelationColls.emplace_back(edm4hepName, lcio_coll);
      }
      if (lcio_coll_type_str == "ReconstructedParticle") {
        // Collect the ParticleID meta information because that has to go to the
        // ParticleID collections
        for (const auto& pidInfo : LCIO2EDM4hepConv::getPIDMetaInfo(lcio_coll)) {
          pidInfos.try_emplace(LCIO2EDM4hepConv::getPIDCollName(lcioName, pidInfo.algoName), pidInfo);
        }
      }

      needCaloHitContribs = (lcio_coll_type_str == "SimCalorimeterHit") && !lcio_coll->isSubset();

      for (auto&& e4hColl : LCIO2EDM4hepConv::convertCollection(edm4hepName, lcio_coll, lcio2edm4hepMaps)) {
        if (std::get<1>(e4hColl)) {
          registerCollection(std::move(e4hColl), lcio_coll);
        } else {
          error() << "Could not convert collection " << lcioName << " (type: " << lcio_coll_type_str << ")" << endmsg;
        }
      }
    } catch (const lcio::DataNotAvailableException&) {
      warning() << "LCIO Collection " << lcioName << " not found in the event, skipping conversion to EDM4hep"
                << endmsg;
      continue;
    }
  }

  // Set the ParticleID meta information
  if (m_podioDataSvc) {
    auto& metadataFrame = m_podioDataSvc->getMetaDataFrame();
    for (const auto& [collName, pidInfo] : pidInfos) {
      edm4hep::utils::PIDHandler::setAlgoInfo(metadataFrame, collName, pidInfo);
    }
  } else {
    for (const auto& [collName, pidInfo] : pidInfos) {
      m_metadataSvc->put(collName, pidInfo);
    }
  }

  auto& globalObjMap = getGlobalObjectMap(this);

  globalObjMap.update(lcio2edm4hepMaps);

  // Now we can resolve relations, subset collections and LCRelations
  LCIO2EDM4hepConv::resolveRelations(lcio2edm4hepMaps, globalObjMap);

  for (const auto& [name, coll, type] : subsetColls) {
    registerCollection(name, LCIO2EDM4hepConv::fillSubset(coll, globalObjMap, type), coll);
  }

  for (auto&& assocColl : LCIO2EDM4hepConv::createLinks(globalObjMap, lcRelationColls)) {
    registerCollection(std::move(assocColl)); // TODO: Potentially handle metadata here?
  }

  if (needCaloHitContribs) {
    registerCollection(name() + "_CaloHitContributions",
                       LCIO2EDM4hepConv::createCaloHitContributions(
                           lcio2edm4hepMaps.simCaloHits,
                           globalObjMap.mcParticles)); // TODO: Can we do something about meta data here?
  }

  return StatusCode::SUCCESS;
}
