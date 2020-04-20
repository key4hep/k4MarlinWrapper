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

gaudi_subdir(GMPWrapper v1r0)

# this declaration will not be needed in the future
# APS not sure when that future is
gaudi_depends_on_subdirs(GaudiAlg GaudiKernel)
gaudi_install_python_modules()

gaudi_add_module(GMPWrapperPlugins
  src/components/Lcio*.cpp
  LINK_LIBRARIES
    GaudiAlgLib
    GaudiKernel
    ROOT::Core
    ${LCIO_LIBRARIES}
    ${Marlin_LIBRARIES}
  INCLUDE_DIRS
    GMPWrapper
    ${LCIO_INCLUDE_DIRS}
    ${Marlin_INCLUDE_DIRS}
  )

gaudi_add_module(MarlinWrapper
  src/components/Marlin*.cpp
  LINK_LIBRARIES
    GaudiAlgLib
    GaudiKernel
    ${LCIO_LIBRARIES}
    ${Marlin_LIBRARIES}
  INCLUDE_DIRS
    ${Marlin_INCLUDE_DIRS}
    ${LCIO_INCLUDE_DIRS}
  )

target_compile_options(GMPWrapperPlugins PUBLIC "-Wno-suggest-override")
gaudi_env(PREPEND LD_LIBRARY_PATH ${ILCSoft_ROOT}/lib/)
gaudi_env(PREPEND LD_LIBRARY_PATH ${DD4hep_ROOT}/lib/)
gaudi_env(PREPEND LD_LIBRARY_PATH ${Gaudi_DIR}/lib/)
gaudi_env(SET DD4hepINSTALL ${DD4hep_ROOT})
