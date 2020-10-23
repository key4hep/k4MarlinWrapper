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
project(GMPWrapper)

# project version
SET( ${PROJECT_NAME}_VERSION_MAJOR 0 )
SET( ${PROJECT_NAME}_VERSION_MINOR 2 )
SET( ${PROJECT_NAME}_VERSION_PATCH 1 )

SET( ${PROJECT_NAME}_VERSION  "${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_PATCH}" )

set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O3 -g -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g")

# Ninja compiler output
include(cmake/compiler_output.cmake)

find_package(ROOT 6.0 REQUIRED)
find_package(LCIO REQUIRED)
find_package(Marlin REQUIRED)

# To be removed with future Gaudi
include(cmake/gaudi_binary_tag.cmake)

include(GNUInstallDirs)

find_package(GaudiProject)


enable_testing()

gaudi_project(${PROJECT_NAME} v0r1 USE Gaudi v33r1 )

