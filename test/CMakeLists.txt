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

gaudi_add_module(GaudiTestAlgorithms
  SOURCES
    src/MCRecoLinkChecker.cc
    src/PseudoRecoAlgorithm.cc
    src/TrivialMCRecoLinker.cc
  LINK
    Gaudi::GaudiKernel
    k4FWCore::k4FWCore
    EDM4HEP::edm4hep
)

find_program(K4RUN k4run)

include(ExternalData)
set(ExternalData_URL_TEMPLATES
 "https://key4hep.web.cern.ch:443/testFiles/ddsimOutput/%(hash)"
 )

# Compile the Marlin test processors into a shared library
find_package(Marlin REQUIRED)

add_library(MarlinTestProcessors SHARED
  src/TrivialMCTruthLinkerProcessor.cc
  src/MarlinMCRecoLinkChecker.cc
)
target_link_libraries(MarlinTestProcessors PUBLIC ${Marlin_LIBRARIES})
target_include_directories(MarlinTestProcessors PUBLIC ${Marlin_INCLUDE_DIRS})

# Test simple processors
ExternalData_Add_Test( marlinwrapper_tests NAME simple_processors COMMAND ${K4RUN} ${CMAKE_CURRENT_SOURCE_DIR}/gaudi_opts/simple_processors.py --LcioEvent.Files DATA{${PROJECT_SOURCE_DIR}/test/input_files/muons.slcio})

# Test simple_processors2
ExternalData_Add_Test( marlinwrapper_tests NAME simple_processors2 COMMAND ${K4RUN} ${CMAKE_CURRENT_SOURCE_DIR}/gaudi_opts/simple_processors2.py --LcioEvent.Files DATA{${PROJECT_SOURCE_DIR}/test/input_files/testSimulation.slcio})

# CLICPerformance setup is added as a simple test that will always succeed, on
# which other tests that need it can depend, in order to make concurrent
# running of those more easily possible
add_test( CLICPerformance_setup bash -c "test -d CLICPerformance || git clone --depth=1 https://github.com/iLCSoft/CLICPerformance")

# Test clicReconstruction
ExternalData_Add_Test( marlinwrapper_tests NAME clicRec COMMAND bash -c "${CMAKE_CURRENT_SOURCE_DIR}/scripts/clicRec.sh DATA{${PROJECT_SOURCE_DIR}/test/input_files/testSimulation.slcio}" )
# Test converter constants
add_test( converter_constants bash -c ${CMAKE_CURRENT_SOURCE_DIR}/scripts/converter_constants.sh )

# Test indicating -1 to go over all events
ExternalData_Add_Test( marlinwrapper_tests NAME all_events_bounds COMMAND ${K4RUN} ${CMAKE_CURRENT_SOURCE_DIR}/gaudi_opts/simple_processors.py --num-events=-1 --LcioEvent.Files DATA{${PROJECT_SOURCE_DIR}/test/input_files/muons.slcio})

# Test putting more events than available, stopping on last available event gracefully
ExternalData_Add_Test( marlinwrapper_tests NAME over_total_events COMMAND ${K4RUN} ${CMAKE_CURRENT_SOURCE_DIR}/gaudi_opts/simple_processors.py --num-events=15 --LcioEvent.Files DATA{${PROJECT_SOURCE_DIR}/test/input_files/muons.slcio})
set_tests_properties ( over_total_events
  PROPERTIES
    PASS_REGULAR_EXPRESSION "Application Manager Terminated successfully with a user requested ScheduledStop")

# Test putting more events than available, stopping on last available event gracefully
add_test( same_num_io bash -c ${CMAKE_CURRENT_SOURCE_DIR}/scripts/same_num_io.sh )
set_tests_properties ( same_num_io
  PROPERTIES
    PASS_REGULAR_EXPRESSION "Input and output have same number of events")

# Test clicReconstruction with EDM4hep input and output
ExternalData_Add_Test( marlinwrapper_tests NAME clicRec_edm4hep_input COMMAND bash -c "${CMAKE_CURRENT_SOURCE_DIR}/scripts/clicRec_e4h_input.sh DATA{${PROJECT_SOURCE_DIR}/test/input_files/ttbar_20240223_edm4hep.root} --no-iosvc")
ExternalData_Add_Test( marlinwrapper_tests NAME clicRec_edm4hep_input_iosvc COMMAND bash -c "${CMAKE_CURRENT_SOURCE_DIR}/scripts/clicRec_e4h_input.sh DATA{${PROJECT_SOURCE_DIR}/test/input_files/ttbar_20240223_edm4hep.root} --iosvc")
add_test( clicRec_edm4hep_input_compare_output bash -c "diff <(podio-dump CLICPerformance/clicConfig/my_output.root | grep -v 'input file:') <(podio-dump CLICPerformance/clicConfig/my_output_iosvc.root | grep -v 'input file:')")

# Run clicReconstruction sequence with LCIO input and output, no converters, with inter-event parallelism
ExternalData_Add_Test( marlinwrapper_tests NAME clicRec_lcio_mt COMMAND bash -c "${CMAKE_CURRENT_SOURCE_DIR}/scripts/clicRec_lcio_mt.sh DATA{${PROJECT_SOURCE_DIR}/test/input_files/testSimulation.slcio}")

# Test the GeoSvc and TrackingCellIDEncodingSvc
add_test( clic_geo_test ${K4RUN} ${CMAKE_CURRENT_SOURCE_DIR}/gaudi_opts/geoTest_cld.py )

# Test for checking whether the converters can resolve relations across
# multiple processors
ExternalData_Add_Test( marlinwrapper_tests NAME global_converter_maps COMMAND ${K4RUN} ${CMAKE_CURRENT_SOURCE_DIR}/gaudi_opts/test_global_converter_maps.py --EventDataSvc.input DATA{${PROJECT_SOURCE_DIR}/test/input_files/ttbar_20240223_edm4hep.root})
ExternalData_Add_Test( marlinwrapper_tests NAME global_converter_maps_iosvc COMMAND ${K4RUN} ${CMAKE_CURRENT_SOURCE_DIR}/gaudi_opts/test_global_converter_maps.py --iosvc --IOSvc.Input DATA{${PROJECT_SOURCE_DIR}/test/input_files/ttbar_20240223_edm4hep.root})

ExternalData_Add_Test( marlinwrapper_tests
  NAME link_conversion_edm4hep_to_lcio
  COMMAND ${K4RUN} ${CMAKE_CURRENT_SOURCE_DIR}/gaudi_opts/test_link_conversion_edm4hep.py --inputfile DATA{${PROJECT_SOURCE_DIR}/test/input_files/ttbar_20240223_edm4hep.root}
)
ExternalData_Add_Test( marlinwrapper_tests
  NAME link_conversion_edm4hep_to_lcio_iosvc
  COMMAND ${K4RUN} ${CMAKE_CURRENT_SOURCE_DIR}/gaudi_opts/test_link_conversion_edm4hep.py --iosvc --inputfile DATA{${PROJECT_SOURCE_DIR}/test/input_files/ttbar_20240223_edm4hep.root}
)

add_test( event_header_conversion bash -c "k4run ${CMAKE_CURRENT_SOURCE_DIR}/gaudi_opts/createEventHeader.py && anajob test.slcio | grep 'EVENT: 42'" )

ExternalData_Add_Target(marlinwrapper_tests)

get_property(test_names DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY TESTS)
set_tests_properties(${test_names} PROPERTIES ENVIRONMENT
"TEST_DIR=${CMAKE_CURRENT_SOURCE_DIR};LD_LIBRARY_PATH=${CMAKE_INSTALL_PREFIX}/lib:${CMAKE_INSTALL_PREFIX}/lib64:$ENV{LD_LIBRARY_PATH};PYTHONPATH=${CMAKE_INSTALL_PREFIX}/python:$ENV{PYTHONPATH};EXAMPLE_DIR=${PROJECT_SOURCE_DIR}/k4MarlinWrapper/examples;MARLIN_DLL=$ENV{MARLIN_DLL}:${CMAKE_CURRENT_BINARY_DIR}/libMarlinTestProcessors.so"
)

set_tests_properties(
  clicRec clicRec_lcio_mt clicRec_edm4hep_input clicRec_edm4hep_input_iosvc
  PROPERTIES
  DEPENDS CLICPerformance_setup
)

set_tests_properties(
  clicRec_edm4hep_input_compare_output
  PROPERTIES
  DEPENDS "clicRec_edm4hep_input;clicRec_edm4hep_input_iosvc"
)

