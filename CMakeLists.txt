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

CMAKE_MINIMUM_REQUIRED(VERSION 3.8)

PROJECT(GMP)

LIST(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/../External/install/Gaudi/cmake)

SET(BINARY_TAG broadwell-slc6-gcc8-dbg)
SET(HOST_BINARY_TAG broadwell-slc6-gcc8-dbg)

FIND_PACKAGE(CLHEP)

# we use a modified way to compile ilcsoft packages
FIND_PACKAGE(ILCSoft REQUIRED)

find_package(GaudiProject)

configure_file(cmake/gmprun.in ${CMAKE_BINARY_DIR}/gmprun)
install(FILES ${CMAKE_BINARY_DIR}/gmprun
        DESTINATION .
        PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ 
                    GROUP_EXECUTE GROUP_READ
                    WORLD_EXECUTE WORLD_READ
RENAME run)

gaudi_project(GMP v0r1 USE Gaudi v32r0 )

# have to ensure our own DD4hep comes before that of the LCG view
gaudi_env(PREPEND LD_LIBRARY_PATH $ENV{DD4hepINSTALL/lib/})
