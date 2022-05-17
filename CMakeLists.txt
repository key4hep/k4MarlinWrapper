#   Copyright 2019 CERN
#
#  Author: Andre Sailer <andre.philippe.sailer@cern.ch>
#
#   Licensed under the Apache License, Version 2.0 (the "License");
#   you may not use this file except in compliance with the License.
#   You may obtain a copy of the License at
#
#       http://www.apache.org/licenses/LICENSE-2.0
#
#   Unless required by applicable law or agreed to in writing, software
#   distributed under the License is distributed on an "AS IS" BASIS,
#   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#   See the License for the specific language governing permissions and
#   limitations under the License.
#
#   In applying this licence, CERN does not waive the privileges and immunities
#   granted to it by virtue of its status as an Intergovernmental Organization
#   or submit itself to any jurisdiction.
#

cmake_minimum_required(VERSION 3.14)
cmake_policy(SET CMP0074 NEW) # use package_ROOT env var to find packages
project(k4MarlinWrapper LANGUAGES CXX)

# RPATH settings 
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE CACHE BOOL "RPATH USE LINK PATH")
option(USE_RUNPATH_INSTEAD_OF_RPATH "Use runpath instead of rpath (allow specifying liked libraries via LD_LIBRARY_PATH)" OFF)
if(USE_RUNPATH_INSTEAD_OF_RPATH) 
  set(CMAKE_SHARED_LINKER_FLAGS "-Wl,--enable-new-dtags")
endif()

# options
option(USE_EXTERNAL_CATCH2 "If off, build the catch2 unittest framework locally" ON)

# project version
SET( ${PROJECT_NAME}_VERSION_MAJOR 0 )
SET( ${PROJECT_NAME}_VERSION_MINOR 4 )
SET( ${PROJECT_NAME}_VERSION_PATCH 1 )

SET( ${PROJECT_NAME}_VERSION  "${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_PATCH}" )

set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O3 -g -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g")

set(CMAKE_CXX_STANDARD 17 CACHE STRING "")

if(NOT CMAKE_CXX_STANDARD MATCHES "14|17|20")
  message(FATAL_ERROR "Unsupported C++ standard: ${CMAKE_CXX_STANDARD}")
endif()

# Ninja compiler output
include(cmake/compiler_output.cmake)

include(GNUInstallDirs)

find_package(Gaudi REQUIRED)
find_package(LCIO REQUIRED)
find_package(Marlin REQUIRED)
find_package(EDM4HEP REQUIRED)
find_package(k4FWCore REQUIRED)
find_package(k4LCIOReader REQUIRED)

include(CTest)

add_subdirectory(k4MarlinWrapper)

if(BUILD_TESTING)
  add_subdirectory(test)
endif()

gaudi_install(CMAKE)
