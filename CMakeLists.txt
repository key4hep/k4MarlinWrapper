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
project(GMP)

# Enable compiler output highligh (for Ninja)
option (FORCE_COLORED_OUTPUT "Always produce ANSI-colored output (GNU/Clang only)." TRUE)
if (${FORCE_COLORED_OUTPUT})
    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
       add_compile_options (-fdiagnostics-color=always)
    elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
       add_compile_options (-fcolor-diagnostics)
    endif ()
endif ()

# we use a modified way to compile ilcsoft packages
# find_package(ILCSoft REQUIRED)
find_package(ROOT 6.0 REQUIRED)
find_package(LCIO REQUIRED)
find_package(Marlin REQUIRED)

list(APPEND CMAKE_MODULE_PATH ${Gaudi_DIR}/cmake)
# get_host_binary_tag.py is not executable in normal gaudi installations,
# which silently fails in the BinaryTagUtils from Gaudi
find_program(HOST_BINARY_TAG_COMMAND
             NAMES host-binary-tag get_host_binary_tag.py
             PATHS "${Gaudi_DIR}/cmake")
execute_process(COMMAND python ${HOST_BINARY_TAG_COMMAND}
   OUTPUT_VARIABLE HOST_BINARY_TAG
   RESULT_VARIABLE HOST_TAG_RETURN
   ERROR_VARIABLE  HOST_BINARY_ERROR
   OUTPUT_STRIP_TRAILING_WHITESPACE)
IF(NOT HOST_BINARY_TAG_COMMAND OR HOST_TAG_RETURN OR NOT HOST_BINARY_TAG)
  message(SEND_ERROR "Failed to execute ${HOST_BINARY_TAG_COMMAND}")
  message(SEND_ERROR "HOST_BINARY_TAG: ${HOST_BINARY_TAG}")
  message(SEND_ERROR "Return Code ${HOST_TAG_RETURN}")
  message(SEND_ERROR "Error Message ${HOST_BINARY_ERROR}")
ENDIF()
set(HOST_BINARY_TAG ${HOST_BINARY_TAG} CACHE STRING "BINARY_TAG of the host")
set(BINARY_TAG ${HOST_BINARY_TAG})

message(STATUS "Using      binary tag ${BINARY_TAG}")
message(STATUS "Using host binary tag ${HOST_BINARY_TAG}")

find_package(GaudiProject)

configure_file(cmake/gmprun.in ${CMAKE_BINARY_DIR}/gmprun)
install(FILES ${CMAKE_BINARY_DIR}/gmprun
        DESTINATION .
        PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ 
                    GROUP_EXECUTE GROUP_READ
                    WORLD_EXECUTE WORLD_READ
RENAME run)

enable_testing()

gaudi_project(GMP v0r1 USE Gaudi v33r1 )

