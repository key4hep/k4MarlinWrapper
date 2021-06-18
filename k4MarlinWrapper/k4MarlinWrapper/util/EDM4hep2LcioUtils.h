#ifndef EDM4HEP2LCIOUTILS_H
#define EDM4HEP2LCIOUTILS_H

namespace k4MW::util {

// Reorder parameters based on dependencies between collections
// This avoids (partly) the need for FillMissingCollections
template <typename T>
void optimizeOrderParams(
  T& params)
{
  // Get view of only the types in the i%3==0 index
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
    // Look for instances of target
    for (int t_idx=0; t_idx<params_view.size(); ++t_idx) {
      if (params_view[t_idx] == target) {
        // Look for instances of dependency
        for (int d_idx=0; d_idx<params_view.size(); ++d_idx) {
          if (params_view[d_idx] == dependency) {
            // If target is before the dependency swap them
            if (t_idx < d_idx) {
              const auto real_t_idx = 3 * t_idx;
              const auto real_d_idx = 3 * d_idx;

              std::swap(params[real_t_idx], params[real_d_idx]);
              std::swap(params[real_t_idx+1], params[real_d_idx+1]);
              std::swap(params[real_t_idx+2], params[real_d_idx+2]);
              // swap also the view
              std::swap(params_view[t_idx], params_view[d_idx]);
              // reset target index until everything is sorted
              t_idx = 0;

              break;
            }
          }
        }
      }
    }
  };

  // A depends on B converted before to not need FillMissingCollections()
  // Make two rounds for collections that may go up again in the list in the first round
  for (int i=0; i < 2; ++i) {
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