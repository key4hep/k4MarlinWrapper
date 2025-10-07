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

gaudi_install(PYTHON)
gaudi_install(SCRIPTS)

# k4MarlinWrapperPlugins
gaudi_add_module(k4MarlinWrapperPlugins
  SOURCES
    src/components/LcioEventAlgo.cpp
    src/components/LcioEventOutput.cpp
    src/components/MarlinProcessorWrapper.cpp
    src/components/TrackingCellIDEncodingSvc.cpp
  LINK
    Gaudi::GaudiKernel
    ${Marlin_LIBRARIES}
    k4FWCore::k4FWCore
    k4FWCore::k4Interface
    EDM4HEP::edm4hep
    LCIO::lcio
)

target_include_directories(k4MarlinWrapperPlugins PUBLIC
  ${PROJECT_SOURCE_DIR}/k4MarlinWrapper
  ${Marlin_INCLUDE_DIRS}
)

# k4MarlinWrapperUtils
gaudi_add_library(k4MarlinWrapperUtils SHARED
  SOURCES
    src/components/StoreUtils.cpp
  LINK
    k4FWCore::k4FWCore
    k4EDM4hep2LcioConv::k4EDM4hep2LcioConv
)

# EDM4hep2lcio
gaudi_add_module(EDM4hep2Lcio
  SOURCES
    src/components/EDM4hep2Lcio.cpp
  LINK
    k4EDM4hep2LcioConv::k4EDM4hep2LcioConv
    k4FWCore::k4FWCore
    ${LCIO_LIBRARIES}
    ${Marlin_LIBRARIES}
    EDM4HEP::edm4hep
    k4MarlinWrapperUtils
)

target_include_directories(EDM4hep2Lcio PUBLIC
  ${PROJECT_SOURCE_DIR}/k4MarlinWrapper
  ${LCIO_INCLUDE_DIRS}
  ${Marlin_INCLUDE_DIRS}
)

# LCIO2EDM4hep
gaudi_add_module(Lcio2EDM4hep
  SOURCES
    src/components/Lcio2EDM4hep.cpp
  LINK
    EDM4HEP::edm4hep
    k4FWCore::k4FWCore
    k4EDM4hep2LcioConv::k4EDM4hep2LcioConv
    k4MarlinWrapperUtils
)

target_include_directories(Lcio2EDM4hep PUBLIC
  ${PROJECT_SOURCE_DIR}/k4MarlinWrapper
  ${LCIO_INCLUDE_DIRS}
)

# Copy python parsing file to genConfDir in Gaudi
add_custom_command(
        TARGET k4MarlinWrapperPlugins POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${PROJECT_SOURCE_DIR}/k4MarlinWrapper/python/k4MarlinWrapper/parseConstants.py
                ${CMAKE_CURRENT_BINARY_DIR}/genConfDir/k4MarlinWrapper/parseConstants.py)

# Install the example options files to share to make them easily accessible
install(DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/examples DESTINATION ${CMAKE_INSTALL_DATADIR}/${CMAKE_PROJECT_NAME})
