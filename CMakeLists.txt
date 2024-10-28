#[[
Copyright (c) 2019-2024 Key4hep-Project.

This file is part of Key4hep.
See https://key4hep.github.io/key4hep-doc/ for further info.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
]]

cmake_minimum_required(VERSION 3.14)
cmake_policy(SET CMP0074 NEW) # use package_ROOT env var to find packages
project(k4MarlinWrapper LANGUAGES CXX)

# project version
SET( ${PROJECT_NAME}_VERSION_MAJOR 0 )
SET( ${PROJECT_NAME}_VERSION_MINOR 10 )
SET( ${PROJECT_NAME}_VERSION_PATCH 0 )

SET( ${PROJECT_NAME}_VERSION  "${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_PATCH}" )

include(cmake/Key4hepConfig.cmake)

include(GNUInstallDirs)

find_package(Gaudi REQUIRED)
find_package(LCIO REQUIRED)
find_package(Marlin REQUIRED)
find_package(EDM4HEP 0.99 REQUIRED)
find_package(k4FWCore REQUIRED)
find_package(k4EDM4hep2LcioConv REQUIRED)

include(CTest)

add_subdirectory(k4MarlinWrapper)

if(BUILD_TESTING)
  add_subdirectory(test)
endif()

gaudi_install(CMAKE)
