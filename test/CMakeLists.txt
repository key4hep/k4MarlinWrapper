#[[
Copyright (c) 2019-2023 Key4hep-Project.

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

# Compile Gaudi algorithm for conversion testing
gaudi_add_module(TestE4H2L
  SOURCES
    src/TestE4H2L.cpp
  LINK
    Gaudi::GaudiAlgLib
    Gaudi::GaudiKernel
    lcio
    k4FWCore::k4FWCore
    EDM4HEP::edm4hep
)

target_include_directories(TestE4H2L PUBLIC
  src
  ${PROJECT_SOURCE_DIR}/k4MarlinWrapper
  k4FWCore::k4FWCore
  EDM4HEP::edm4hep
  ${LCIO_INCLUDE_DIRS}
)

# Add test scripts

find_program(BASH_PROGRAM bash)

if (BASH_PROGRAM)
  # Test simple processors
  add_test( simple_processors ${BASH_PROGRAM} ${CMAKE_CURRENT_SOURCE_DIR}/scripts/simple_processors.sh )

  # Test simple_processors2
  add_test( simple_processors2 ${BASH_PROGRAM} ${CMAKE_CURRENT_SOURCE_DIR}/scripts/simple_processors2.sh )

  # CLICPerformance setup is added as a simple test that will always succeed, on
  # which other tests that need it can depend, in order to make concurrent
  # running of those more easily possible
  add_test( CLICPerformance_setup ${BASH_PROGRAM} ${CMAKE_CURRENT_SOURCE_DIR}/scripts/setup_clic_performance.sh )

  # Test clicReconstruction
  add_test( clicRec ${BASH_PROGRAM} ${CMAKE_CURRENT_SOURCE_DIR}/scripts/clicRec.sh )

  # Test converter constants
  add_test( converter_constants ${BASH_PROGRAM} ${CMAKE_CURRENT_SOURCE_DIR}/scripts/converter_constants.sh )

  # Test the edm4hep to lcio converter
  add_test( edm_converters ${BASH_PROGRAM} ${CMAKE_CURRENT_SOURCE_DIR}/scripts/edm_converters.sh )

  # Test indicating -1 to go over all events
  add_test( all_events_bounds ${BASH_PROGRAM} ${CMAKE_CURRENT_SOURCE_DIR}/scripts/all_events_bounds.sh )
  set_tests_properties ( all_events_bounds
    PROPERTIES
      PASS_REGULAR_EXPRESSION "Application Manager Terminated successfully with a user requested ScheduledStop")

  # Test putting more events than available, stopping on last available event gracefully
  add_test( over_total_events ${BASH_PROGRAM} ${CMAKE_CURRENT_SOURCE_DIR}/scripts/over_total_events.sh )
  set_tests_properties ( over_total_events
    PROPERTIES
      PASS_REGULAR_EXPRESSION "Application Manager Terminated successfully with a user requested ScheduledStop")

  # Test putting more events than available, stopping on last available event gracefully
  add_test( same_num_io ${BASH_PROGRAM} ${CMAKE_CURRENT_SOURCE_DIR}/scripts/same_num_io.sh )
  set_tests_properties ( same_num_io
    PROPERTIES
      PASS_REGULAR_EXPRESSION "Input and output have same number of events")

  # Test clicReconstruction with EDM4hep input and output
  add_test( clicRec_edm4hep_input ${BASH_PROGRAM} ${CMAKE_CURRENT_SOURCE_DIR}/scripts/clicRec_e4h_input.sh )

  # Run clicReconstruction sequence with LCIO input and output, no converters, with inter-event parallelism
  add_test( clicRec_lcio_mt ${BASH_PROGRAM} ${CMAKE_CURRENT_SOURCE_DIR}/scripts/clicRec_lcio_mt.sh )

  set_tests_properties (
      simple_processors
      simple_processors2
      clicRec
      converter_constants
      edm_converters
      all_events_bounds
      over_total_events
      same_num_io
      clicRec_lcio_mt
      clicRec_edm4hep_input
    PROPERTIES
      ENVIRONMENT "TEST_DIR=${CMAKE_CURRENT_SOURCE_DIR};LD_LIBRARY_PATH=${CMAKE_INSTALL_PREFIX}/lib:${CMAKE_INSTALL_PREFIX}/lib64:$ENV{LD_LIBRARY_PATH};PYTHONPATH=${CMAKE_INSTALL_PREFIX}/python:$ENV{PYTHONPATH}"
      )

    set_tests_properties(
      clicRec clicRec_lcio_mt clicRec_edm4hep_input
      PROPERTIES
      DEPENDS CLICPerformance_setup
      )

endif(BASH_PROGRAM)
