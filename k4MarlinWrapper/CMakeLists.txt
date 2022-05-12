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

gaudi_install(PYTHON)
gaudi_install(SCRIPTS)

# k4MarlinWrapperPlugins
gaudi_add_module(k4MarlinWrapperPlugins
  SOURCES
    src/components/LcioEventAlgo.cpp
    src/components/LcioEventOutput.cpp
  LINK
    Gaudi::GaudiAlgLib
    ${Marlin_LIBRARIES}
)

target_include_directories(k4MarlinWrapperPlugins PUBLIC
  ${CMAKE_SOURCE_DIR}/k4MarlinWrapper
  ${LCIO_INCLUDE_DIRS}
)

# MarlinWrapper
gaudi_add_module(MarlinWrapper
  SOURCES
    src/components/MarlinProcessorWrapper.cpp
  LINK
    Gaudi::GaudiAlgLib
    k4FWCore::k4FWCore
    ${Marlin_LIBRARIES}
    EDM4HEP::edm4hep
)

target_include_directories(MarlinWrapper PUBLIC
  ${CMAKE_SOURCE_DIR}/k4MarlinWrapper
  ${Marlin_INCLUDE_DIRS}
  ${LCIO_INCLUDE_DIRS}
)

# EDM4hep2lcio
gaudi_add_module(EDM4hep2Lcio
  SOURCES
    src/components/EDM4hep2Lcio.cpp
  LINK
    Gaudi::GaudiAlgLib
    ${LCIO_LIBRARIES}
    k4FWCore::k4FWCore
    EDM4HEP::edm4hep
)

target_include_directories(EDM4hep2Lcio PUBLIC
  ${CMAKE_SOURCE_DIR}/k4MarlinWrapper
  ${LCIO_INCLUDE_DIRS}
)

# LCIO2EDM4hep
gaudi_add_module(Lcio2EDM4hep
  SOURCES
    src/components/Lcio2EDM4hep.cpp
  LINK
    Gaudi::GaudiAlgLib
    EDM4HEP::edm4hep
    k4FWCore::k4FWCore
    k4LCIOReader::k4LCIOReader
)

target_include_directories(Lcio2EDM4hep PUBLIC
  ${CMAKE_SOURCE_DIR}/k4MarlinWrapper
  ${LCIO_INCLUDE_DIRS}
)

# Copy python parsing file to genConfDir in Gaudi
add_custom_command(
        TARGET MarlinWrapper POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_SOURCE_DIR}/k4MarlinWrapper/python/k4MarlinWrapper/parseConstants.py
                ${CMAKE_CURRENT_BINARY_DIR}/genConfDir/k4MarlinWrapper/parseConstants.py)