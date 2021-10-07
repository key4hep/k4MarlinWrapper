#ifndef K4MARLINWRAPPER_UTIL_H
#define K4MARLINWRAPPER_UTIL_H

#include <iostream>
#include <regex>
#include <string>
#include <vector>

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
}  // namespace k4MW::util

#endif