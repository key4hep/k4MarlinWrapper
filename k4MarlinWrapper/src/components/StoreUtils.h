#include "GaudiKernel/AlgTool.h"

#include <string>
#include <vector>

std::vector<std::string> getAvailableCollectionsFromStore(const AlgTool*                                  thisClass,
                                                          std::optional<std::map<uint32_t, std::string>>& idToName,
                                                          bool returnFrameCollections = false);
