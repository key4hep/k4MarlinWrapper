#ifndef EDM4HEP2LCIOUTILS_H
#define EDM4HEP2LCIOUTILS_H

namespace k4MW::util {

// Reorder parameters based on dependencies between collections
// This avoids (partly) the need for FillMissingCollections
template <typename T>
void optimizeOrderParams(
  T& params)
{

  if (params.size() % 3 == 0) {
    // Get only to the typesin the i%3==0 index
    std::vector<std::string> params_view;
    for (int i=0; i < params.size(); i=i+3) {
      params_view.emplace_back(params[i]);
    }

    // Can't run in parallel
    // Dependency should be converted before the target, so swap them (and their names) if out of order.
    // search for the target and its dependency string, get indexes,
    // if target before dependency swap all related elements. Multiply indexes by 3 to get correct index from view.
    auto swap_if_before = [&params_view, &params](const std::string& target, const std::string& dependency)
    {
      auto target_found_it = std::find(params_view.begin(), params_view.end(), target);
      if (target_found_it != params_view.end()) {

        auto target_index = 3 * std::distance(params_view.begin(), target_found_it);
        auto dependency_found_it = std::find(params_view.begin(), params_view.end(), dependency);

        if (dependency_found_it != params_view.end()) {

          auto dependency_index = 3 * std::distance(params_view.begin(), dependency_found_it);

          if (target_index < dependency_index) {
            std::swap(params[target_index], params[dependency_index]);
            std::swap(params[target_index+1], params[dependency_index+1]);
            std::swap(params[target_index+2], params[dependency_index+2]);
          }
        }
      }
    };

    // A depends on B converted before to not need FillMissingCollections()
    swap_if_before("Track", "TrackerHit");
    swap_if_before("SimTrackerHit", "MCParticle");
    swap_if_before("SimCalorimeterHit", "MCParticle");
    swap_if_before("Cluster", "CalorimeterHit");
    // swap_if_before("Vertex", "ReconstructedParticle");
    swap_if_before("ReconstructedParticle", "Track");
    // This generates a cycle, but that's fine
    // swap_if_before("ReconstructedParticle", "Vertex");
    swap_if_before("ReconstructedParticle", "Cluster");
  }
}

}

#endif