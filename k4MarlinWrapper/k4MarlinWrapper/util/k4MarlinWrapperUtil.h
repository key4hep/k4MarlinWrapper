#ifndef K4MARLINWRAPPER_UTIL_H
#define K4MARLINWRAPPER_UTIL_H

#include <iostream>
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
    static marlin::StringParameters p{};
    const static auto               initMarlinGlobal = []() {
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
