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
#include "GaudiKernel/AlgTool.h"

#include <string>
#include <vector>

// This functionality is used in the Writer from k4FWCore and is reimplemented
// here with some additions to make it useful for converting both from EDM4hep
// to LCIO and vice versa
std::vector<std::string> getAvailableCollectionsFromStore(const AlgTool* thisClass,
                                                          std::optional<std::map<uint32_t, std::string>>& idToName,
                                                          bool returnFrameCollections = false);
