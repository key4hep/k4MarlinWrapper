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
#ifndef K4MARLINWRAPPER_CONVEREROBJECT_MAP_H
#define K4MARLINWRAPPER_CONVEREROBJECT_MAP_H

#include "k4EDM4hep2LcioConv/MappingUtils.h"

#include "edm4hep/CalorimeterHit.h"
#include "edm4hep/Cluster.h"
#include "edm4hep/MCParticle.h"
#include "edm4hep/ParticleID.h"
#include "edm4hep/RawCalorimeterHit.h"
#include "edm4hep/RawTimeSeries.h"
#include "edm4hep/ReconstructedParticle.h"
#include "edm4hep/SimCalorimeterHit.h"
#include "edm4hep/SimTrackerHit.h"
#include "edm4hep/Track.h"
#include "edm4hep/TrackerHit3D.h"
#include "edm4hep/TrackerHitPlane.h"
#include "edm4hep/Vertex.h"

#include <string_view>

namespace EVENT {
  class Track;
  class TrackerHit;
  class TrackerHitPlane;
  class SimTrackerHit;
  class CalorimeterHit;
  class RawCalorimeterHit;
  class SimCalorimeterHit;
  class TPCHit;
  class Cluster;
  class Vertex;
  class ReconstructedParticle;
  class MCParticle;
  class ParticleID;
}  // namespace EVENT

namespace k4MarlinWrapper {

  /**
   * Fill all key value pairs from updates into map
   */
  template <typename Map, typename UpdateMap> void updateMap(Map& map, const UpdateMap& updates) {
    for (const auto& [k, v] : updates) {
      k4EDM4hep2LcioConv::detail::mapInsert(k, v, map);
    }
  }

  /**
   * The LCIO <-> EDM4hep object mapping that holds the relations between all
   * converted objects from all converters that are running.
   */
  struct GlobalConvertedObjectsMap {
    template <typename K, typename V> using ObjectMapT = k4EDM4hep2LcioConv::VecMapT<K, V>;

    ObjectMapT<EVENT::Track*, edm4hep::Track>                                 tracks{};
    ObjectMapT<EVENT::TrackerHit*, edm4hep::TrackerHit3D>                     trackerHits{};
    ObjectMapT<EVENT::SimTrackerHit*, edm4hep::SimTrackerHit>                 simTrackerHits{};
    ObjectMapT<EVENT::CalorimeterHit*, edm4hep::CalorimeterHit>               caloHits{};
    ObjectMapT<EVENT::RawCalorimeterHit*, edm4hep::RawCalorimeterHit>         rawCaloHits{};
    ObjectMapT<EVENT::SimCalorimeterHit*, edm4hep::SimCalorimeterHit>         simCaloHits{};
    ObjectMapT<EVENT::TPCHit*, edm4hep::RawTimeSeries>                        tpcHits{};
    ObjectMapT<EVENT::Cluster*, edm4hep::Cluster>                             clusters{};
    ObjectMapT<EVENT::Vertex*, edm4hep::Vertex>                               vertices{};
    ObjectMapT<EVENT::ReconstructedParticle*, edm4hep::ReconstructedParticle> recoParticles{};
    ObjectMapT<EVENT::MCParticle*, edm4hep::MCParticle>                       mcParticles{};
    ObjectMapT<EVENT::TrackerHitPlane*, edm4hep::TrackerHitPlane>             trackerHitPlanes{};
    ObjectMapT<EVENT::ParticleID*, edm4hep::ParticleID>                       particleIDs{};

    constexpr static auto TESpath = std::string_view{"/Event/EDMConvGlobalObjMap"};

    /**
     * Update the map with contents from one converter run
     */
    template <typename ObjectMap> void update(const ObjectMap& localMap) {
      updateMap(tracks, localMap.tracks);
      updateMap(trackerHits, localMap.trackerHits);
      updateMap(simTrackerHits, localMap.simTrackerHits);
      updateMap(caloHits, localMap.caloHits);
      updateMap(rawCaloHits, localMap.rawCaloHits);
      updateMap(simCaloHits, localMap.simCaloHits);
      updateMap(tpcHits, localMap.tpcHits);
      updateMap(clusters, localMap.clusters);
      updateMap(vertices, localMap.vertices);
      updateMap(recoParticles, localMap.recoParticles);
      updateMap(mcParticles, localMap.mcParticles);
      updateMap(trackerHitPlanes, localMap.trackerHitPlanes);
      updateMap(particleIDs, localMap.particleIDs);
    }
  };
}  // namespace k4MarlinWrapper

#endif
