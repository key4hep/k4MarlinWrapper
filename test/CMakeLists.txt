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
    src/MCRecoLinkCheckerFunctional.cc
    src/PseudoRecoAlgorithm.cc
    src/PseudoRecoFunctional.cc
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
  src/PseudoRecoProcessor.cc
)
target_link_libraries(MarlinTestProcessors PUBLIC ${Marlin_LIBRARIES})
target_include_directories(MarlinTestProcessors PUBLIC ${Marlin_INCLUDE_DIRS})

# Test simple processors
ExternalData_Add_Test( marlinwrapper_tests NAME simple_processors COMMAND ${K4RUN} ${CMAKE_CURRENT_SOURCE_DIR}/gaudi_opts/simple_processors.py --LcioEvent.Files DATA{${PROJECT_SOURCE_DIR}/test/input_files/muons.slcio})

# Test simple_processors2
ExternalData_Add_Test( marlinwrapper_tests NAME simple_processors2 COMMAND ${K4RUN} ${CMAKE_CURRENT_SOURCE_DIR}/gaudi_opts/simple_processors2.py --LcioEvent.Files DATA{${PROJECT_SOURCE_DIR}/test/input_files/testSimulation.slcio})

ExternalData_Add_Test( marlinwraper_tests NAME convert_empty_parameters
  COMMAND bash -c "${CMAKE_CURRENT_SOURCE_DIR}/scripts/convert_empty_parameters.sh DATA{${PROJECT_SOURCE_DIR}/test/input_files/testSimulation.slcio}"
)

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

add_test( skip_events bash -c ${CMAKE_CURRENT_SOURCE_DIR}/scripts/skip_events.sh )
set_tests_properties( skip_events
  PROPERTIES
    PASS_REGULAR_EXPRESSION "Output has expected number of events")

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
ExternalData_Add_Test( marlinwrapper_tests
  NAME global_converter_maps
  COMMAND ${K4RUN} ${CMAKE_CURRENT_SOURCE_DIR}/gaudi_opts/test_global_converter_maps.py --no-iosvc --EventDataSvc.input DATA{${PROJECT_SOURCE_DIR}/test/input_files/ttbar_20240223_edm4hep.root}
)
ExternalData_Add_Test( marlinwrapper_tests
  NAME global_converter_maps_algorithm
  COMMAND ${K4RUN} ${CMAKE_CURRENT_SOURCE_DIR}/gaudi_opts/test_global_converter_maps.py --no-iosvc --use-gaudi-algorithm --OutputWriter.filename=global_converter_maps_algorithm.root --EventDataSvc.input DATA{${PROJECT_SOURCE_DIR}/test/input_files/ttbar_20240223_edm4hep.root}
)

ExternalData_Add_Test( marlinwrapper_tests
  NAME global_converter_maps_iosvc
  COMMAND ${K4RUN} ${CMAKE_CURRENT_SOURCE_DIR}/gaudi_opts/test_global_converter_maps.py --IOSvc.Input DATA{${PROJECT_SOURCE_DIR}/test/input_files/ttbar_20240223_edm4hep.root}
)
ExternalData_Add_Test( marlinwrapper_tests
  NAME global_converter_maps_iosvc_functional
  COMMAND ${K4RUN} ${CMAKE_CURRENT_SOURCE_DIR}/gaudi_opts/test_global_converter_maps.py --use-functional-checker --IOSvc.Input DATA{${PROJECT_SOURCE_DIR}/test/input_files/ttbar_20240223_edm4hep.root}
)
ExternalData_Add_Test( marlinwrapper_tests
  NAME global_converter_maps_iosvc_algorithm
  COMMAND ${K4RUN} ${CMAKE_CURRENT_SOURCE_DIR}/gaudi_opts/test_global_converter_maps.py --use-gaudi-algorithm --IOSvc.Input DATA{${PROJECT_SOURCE_DIR}/test/input_files/ttbar_20240223_edm4hep.root} --IOSvc.Output=global_converter_maps_iosvc_algorithm.root
)
ExternalData_Add_Test( marlinwrapper_tests
  NAME global_converter_maps_iosvc_algorithm_functional
  COMMAND ${K4RUN} ${CMAKE_CURRENT_SOURCE_DIR}/gaudi_opts/test_global_converter_maps.py --use-gaudi-algorithm --use-functional-checker --IOSvc.Input DATA{${PROJECT_SOURCE_DIR}/test/input_files/ttbar_20240223_edm4hep.root} --IOSvc.Output=global_converter_maps_iosvc_algorithm_functional.root
)

ExternalData_Add_Test( marlinwrapper_tests
  NAME link_conversion_edm4hep_to_lcio
  COMMAND ${K4RUN} ${CMAKE_CURRENT_SOURCE_DIR}/gaudi_opts/test_link_conversion_edm4hep.py --no-iosvc --inputfile DATA{${PROJECT_SOURCE_DIR}/test/input_files/ttbar_20240223_edm4hep.root}
)
ExternalData_Add_Test( marlinwrapper_tests
  NAME link_conversion_edm4hep_to_lcio_iosvc
  COMMAND ${K4RUN} ${CMAKE_CURRENT_SOURCE_DIR}/gaudi_opts/test_link_conversion_edm4hep.py --inputfile DATA{${PROJECT_SOURCE_DIR}/test/input_files/ttbar_20240223_edm4hep.root}
)
ExternalData_Add_Test( marlinwrapper_tests
  NAME link_conversion_edm4hep_to_lcio_iosvc_gaudi_algorithm
  COMMAND ${K4RUN} ${CMAKE_CURRENT_SOURCE_DIR}/gaudi_opts/test_link_conversion_edm4hep.py --use-gaudi-algorithm --inputfile DATA{${PROJECT_SOURCE_DIR}/test/input_files/ttbar_20240223_edm4hep.root}
)

# ---- IO Helpers tests
ExternalData_Add_Test( marlinwrapper_tests
  NAME io_helpers_lcio_marlin_only
  COMMAND ${K4RUN} ${CMAKE_CURRENT_SOURCE_DIR}/gaudi_opts/test_io_utilities.py --inputfile DATA{${PROJECT_SOURCE_DIR}/test/input_files/muons.slcio} --output-type lcio --outputbase lcio_marlin_only
)

ExternalData_Add_Test( marlinwrapper_tests
  NAME io_helpers_edm4hep_marlin_only
  COMMAND ${K4RUN} ${CMAKE_CURRENT_SOURCE_DIR}/gaudi_opts/test_io_utilities.py --inputfile DATA{${PROJECT_SOURCE_DIR}/test/input_files/ttbar_20240223_edm4hep.root} --output-type edm4hep --outputbase edm4hep_marlin_only
)

ExternalData_Add_Test( marlinwrapper_tests
  NAME io_helpers_lcio_in_edm4hep_out_marlin_only
  COMMAND ${K4RUN} ${CMAKE_CURRENT_SOURCE_DIR}/gaudi_opts/test_io_utilities.py --inputfile DATA{${PROJECT_SOURCE_DIR}/test/input_files/muons.slcio} --output-type edm4hep --outputbase lcio_in_edm4hep_out_marlin_only
)

ExternalData_Add_Test( marlinwrapper_tests
  NAME io_helpers_edm4hep_in_lcio_out_marlin_only
  COMMAND ${K4RUN} ${CMAKE_CURRENT_SOURCE_DIR}/gaudi_opts/test_io_utilities.py --inputfile DATA{${PROJECT_SOURCE_DIR}/test/input_files/ttbar_20240223_edm4hep.root} --output-type lcio --outputbase edm4hep_in_lcio_out_marlin_only
)

ExternalData_Add_Test( marlinwrapper_tests
  NAME io_helpers_lcio_marlin_gaudi
  COMMAND ${K4RUN} ${CMAKE_CURRENT_SOURCE_DIR}/gaudi_opts/test_io_utilities.py --inputfile DATA{${PROJECT_SOURCE_DIR}/test/input_files/muons.slcio} --with-gaudi-algorithm --output-type lcio --outputbase lcio_marlin_gaudi
)

ExternalData_Add_Test( marlinwrapper_tests
  NAME io_helpers_edm4hep_marlin_gaudi
  COMMAND ${K4RUN} ${CMAKE_CURRENT_SOURCE_DIR}/gaudi_opts/test_io_utilities.py --inputfile DATA{${PROJECT_SOURCE_DIR}/test/input_files/ttbar_20240223_edm4hep.root} --with-gaudi-algorithm --output-type edm4hep --outputbase edm4hep_marlin_gaudi
)

ExternalData_Add_Test( marlinwrapper_tests
  NAME io_helpers_edm4hep_both_out_marlin_gaudi
  COMMAND ${K4RUN} ${CMAKE_CURRENT_SOURCE_DIR}/gaudi_opts/test_io_utilities.py --inputfile DATA{${PROJECT_SOURCE_DIR}/test/input_files/ttbar_20240223_edm4hep.root} --with-gaudi-algorithm --output-type both --outputbase edm4hep_both_out_marlin_gaudi
)

ExternalData_Add_Test( marlinwrapper_tests
  NAME lcio_event_only
  COMMAND ${K4RUN} ${CMAKE_CURRENT_SOURCE_DIR}/gaudi_opts/lcio_event_only.py --inputfile DATA{${PROJECT_SOURCE_DIR}/test/input_files/muons.slcio}
)

ExternalData_Add_Test( marlinwrapper_tests
  NAME partial_edm4hep_to_lcio
  COMMAND ${K4RUN} ${CMAKE_CURRENT_SOURCE_DIR}/gaudi_opts/test_partial_conversion.py --inputfile DATA{${PROJECT_SOURCE_DIR}/test/input_files/ttbar_20240223_edm4hep.root}
)

add_test(check_lcio_output_partial_conversion
  bash -c "${CMAKE_CURRENT_SOURCE_DIR}/scripts/check_lcio_output_partial_conversion.sh partially_converted_lcio_output.slcio"
)

set_tests_properties(
  check_lcio_output_partial_conversion
  PROPERTIES
  DEPENDS partial_edm4hep_to_lcio
)

add_test( event_header_conversion bash -c "k4run ${CMAKE_CURRENT_SOURCE_DIR}/gaudi_opts/createEventHeader.py && anajob test.slcio | grep 'EVENT: 42'" )

add_test(event_header_conversion_no_header ${K4RUN} ${CMAKE_CURRENT_SOURCE_DIR}/gaudi_opts/createEventHeader.py --no-event-header )
set_tests_properties(event_header_conversion_no_header
  PROPERTIES
  PASS_REGULAR_EXPRESSION "WARNING The EventHeader collection is not available. Not converting it."
)

ExternalData_Add_Target(marlinwrapper_tests)

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


# For LD_LIBRARY_PATH
# ${PROJECT_BINARY_DIR}                                         # The files <plugins>.components and <plugins>.confdb are here
# ${PROJECT_BINARY_DIR}/${PROJECT_NAME}                         # The plugins (.so) are here
# ${CMAKE_CURRENT_BINARY_DIR}                                   # The tests (.so) are here if there are algorithms in the tests
# ${CMAKE_CURRENT_BINARY_DIR}/genConfDir/${CURRENT_DIR_NAME}    # The files <test>.components and <test>.confdb are here
# For PYTHONPATH
# ${PROJECT_BINARY_DIR}/${PROJECT_NAME}/genConfDir              # The genConfDir of the plugins
# ${CMAKE_CURRENT_BINARY_DIR}/genConfDir                        # The genConfDir of the tests if there are algorithms in the tests

get_filename_component(CURRENT_DIR_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)

set(test_environment "\
LD_LIBRARY_PATH=\
${PROJECT_BINARY_DIR}:\
${PROJECT_BINARY_DIR}/${PROJECT_NAME}:\
${CMAKE_CURRENT_BINARY_DIR}:\
${CMAKE_CURRENT_BINARY_DIR}/genConfDir/${CURRENT_DIR_NAME}:\
$ENV{LD_LIBRARY_PATH};\
\
PYTHONPATH=\
${PROJECT_BINARY_DIR}/${PROJECT_NAME}/genConfDir:\
${CMAKE_CURRENT_BINARY_DIR}/genConfDir:\
$ENV{PYTHONPATH};\
\
TEST_DIR=${CMAKE_CURRENT_SOURCE_DIR};\
EXAMPLE_DIR=${PROJECT_SOURCE_DIR}/k4MarlinWrapper/examples;\
MARLIN_DLL=$ENV{MARLIN_DLL}:${CMAKE_CURRENT_BINARY_DIR}/libMarlinTestProcessors.so\
"
)
get_property(test_names DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY TESTS)
set_tests_properties(${test_names} PROPERTIES ENVIRONMENT "${test_environment}")

# The following is done to make the tests work without installing the files in
# the installation directory. The k4MarlinWrapper in the build directory is populated by
# Gaudi so we can't just make a k4MarlinWrapper folder and put the python files there.
# We put everything in one of the folders generated by Gaudi so that importing from
# k4MarlinWrapper will give the right result
add_custom_target(copy_directory ALL
  COMMAND ${CMAKE_COMMAND} -E copy_directory
  ${PROJECT_SOURCE_DIR}/k4MarlinWrapper/python/k4MarlinWrapper/ ${PROJECT_BINARY_DIR}/k4MarlinWrapper/genConfDir/k4MarlinWrapper
  COMMENT "Copying k4MarlinWrapper python package after building all the libraries and plugins"
  DEPENDS MarlinTestProcessors
)
