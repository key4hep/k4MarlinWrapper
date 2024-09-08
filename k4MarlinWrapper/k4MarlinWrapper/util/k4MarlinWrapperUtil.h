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
#ifndef K4MARLINWRAPPER_UTIL_H
#define K4MARLINWRAPPER_UTIL_H

#include <regex>
#include <string>
#include <vector>

#include "marlin/Global.h"
#include "marlin/StringParameters.h"

namespace k4MW::util {

  // Split a string by a regex
  std::vector<std::string> split(const std::string& subject, const std::regex& re) {
    std::vector<std::string> container{std::sregex_token_iterator(subject.begin(), subject.end(), re, -1),
                                       std::sregex_token_iterator()};
    return container;
  }

  // Split a string by any whitespace characters
  std::vector<std::string> split(const std::string& subject) {
    std::regex re{"\\s+"};
    return split(subject, re);
  }

  /// singleton helper to initialize global Marlin parameters exactly once. This
  marlin::StringParameters* marlinGlobalParameters() {
    static marlin::StringParameters    p{};
    [[maybe_unused]] const static auto initMarlinGlobal = []() {
      marlin::Global::parameters = &p;
      return true;  // need a non-void return type
    }();

    return marlin::Global::parameters;
  }

  // Make sure that the initialization is done before any possible accesses to
  // marlin::Global::parameters
  const static auto* marlin_global_parameters = marlinGlobalParameters();

}  // namespace k4MW::util

#endif
