/*
 * Copyright (c) 2019-2023 Key4hep-Project.
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

namespace edm4hep {
  class Track;
  class TrackerHit;
  class TrackerHitPlane;
  class SimTrackerHit;
  class CalorimeterHit;
  class RawCalorimeterHit;
  class SimCalorimeterHit;
  class RawTimeSeries;
  class Cluster;
  class Vertex;
  class ReconstructedParticle;
  class MCParticle;
  class ParticleID;
}  // namespace edm4hep

namespace k4MarlinWrapper {

  /**
   * Fill all key value pairs from updates into map
   */
  template <typename Map, typename UpdateMap> void updateMap(Map& map, const UpdateMap& updates) {
    for (const auto& [k, v] : updates) {
      k4EDM4hep2LcioConv::detail::mapInsert(k, v, map);
    }
  }

  namespace detail {
    /// Detectors for checking whether T has a trackerHitPlanes and a particleIDs map
    template <typename T> using has_trkhit_plane = decltype(std::declval<T>().trackerHitPlanes);
    template <typename T> using has_particle_id  = decltype(std::declval<T>().particleIDs);
  }  // namespace detail

  /**
   * The LCIO <-> EDM4hep object mapping that holds the relations between all
   * converted objects from all converters that are running.
   */
  struct GlobalConvertedObjectsMap {
    template <typename K, typename V> using ObjectMapT = k4EDM4hep2LcioConv::VecMapT<K, V>;

    ObjectMapT<EVENT::Track*, edm4hep::Track>                                 tracks{};
    ObjectMapT<EVENT::TrackerHit*, edm4hep::TrackerHit>                       trackerHits{};
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

    /**
     * Get the singleton instance
     */
    static GlobalConvertedObjectsMap& get() {
      static GlobalConvertedObjectsMap globalMap{};
      return globalMap;
    }

    // TODO: clear

    /**
     * Update the map with contents from one converter run
     */
    template <typename ObjectMap> static void update(const ObjectMap& localMap) {
      auto& globalMap = get();
      updateMap(globalMap.tracks, localMap.tracks);
      updateMap(globalMap.trackerHits, localMap.trackerHits);
      updateMap(globalMap.simTrackerHits, localMap.simTrackerHits);
      updateMap(globalMap.caloHits, localMap.caloHits);
      updateMap(globalMap.rawCaloHits, localMap.rawCaloHits);
      updateMap(globalMap.simCaloHits, localMap.simCaloHits);
      updateMap(globalMap.tpcHits, localMap.tpcHits);
      updateMap(globalMap.clusters, localMap.clusters);
      updateMap(globalMap.vertices, localMap.vertices);
      updateMap(globalMap.recoParticles, localMap.recoParticles);
      updateMap(globalMap.mcParticles, localMap.mcParticles);

      if constexpr (det::is_detected_v<detail::has_trkhit_plane, ObjectMap>) {
        updateMap(globalMap.trackerHitPlanes, localMap.trackerHitPlanes);
      }
      if constexpr (det::is_detected_v<detail::has_particle_id, ObjectMap>) {
        updateMap(globalMap.particleIDs, localMap.particleIDs);
      }
    }

  private:
    GlobalConvertedObjectsMap() = default;
  };
}  // namespace k4MarlinWrapper

#endif
