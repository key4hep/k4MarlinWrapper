# Compile Gaudi algorithm for conversion testing
gaudi_add_module(TestE4H2L
  SOURCES
    src/TestE4H2L.cpp
  LINK
    GaudiAlgLib
    GaudiKernel
    ${LCIO_LIBRARIES}
    k4FWCore::k4FWCore
    EDM4HEP::edm4hep
    k4LCIOReader::k4LCIOReader
)

message(STATUS ${CMAKE_SOURCE_DIR}/k4MarlinWrapper/)

target_include_directories(TestE4H2L PUBLIC
  src/
  ${CMAKE_SOURCE_DIR}/k4MarlinWrapper/k4MarlinWrapper/
  k4FWCore
  EDM4HEP::edm4hep
  ${LCIO_INCLUDE_DIRS}
)

# Add test scripts

find_program(BASH_PROGRAM bash)

if (BASH_PROGRAM)
  # Test k4MarlinWrapper1
  add_test( test_k4MarlinWrapper1 ${BASH_PROGRAM} ${CMAKE_CURRENT_SOURCE_DIR}/scripts/test_k4MarlinWrapper1.sh )
  set_tests_properties ( test_k4MarlinWrapper1
    PROPERTIES
      ENVIRONMENT k4MarlinWrapper_tests_DIR=${CMAKE_CURRENT_SOURCE_DIR}
      PASS_REGULAR_EXPRESSION "INFO Application Manager Terminated successfully")

  # Test k4MarlinWrapper2
  add_test( test_k4MarlinWrapper2 ${BASH_PROGRAM} ${CMAKE_CURRENT_SOURCE_DIR}/scripts/test_k4MarlinWrapper2.sh )
  set_tests_properties ( test_k4MarlinWrapper2
    PROPERTIES
      ENVIRONMENT k4MarlinWrapper_tests_DIR=${CMAKE_CURRENT_SOURCE_DIR}
      PASS_REGULAR_EXPRESSION "INFO Application Manager Terminated successfully")

  # Test clicReconstruction
  add_test( test_clicReconstruction ${BASH_PROGRAM} ${CMAKE_CURRENT_SOURCE_DIR}/scripts/test_clicReconstruction.sh )
  set_tests_properties ( test_clicReconstruction
    PROPERTIES
      ENVIRONMENT k4MarlinWrapper_tests_DIR=${CMAKE_CURRENT_SOURCE_DIR}
      PASS_REGULAR_EXPRESSION "INFO Application Manager Terminated successfully")

  # Test converter constants
  add_test( test_converter_constants ${BASH_PROGRAM} ${CMAKE_CURRENT_SOURCE_DIR}/scripts/test_converter_constants.sh )
  set_tests_properties ( test_converter_constants
    PROPERTIES
      ENVIRONMENT k4MarlinWrapper_tests_DIR=${CMAKE_CURRENT_SOURCE_DIR}
      PASS_REGULAR_EXPRESSION "INFO Application Manager Terminated successfully")

  # Test the edm4hep to lcio converter
  add_test( test_edm_converters ${BASH_PROGRAM} ${CMAKE_CURRENT_SOURCE_DIR}/scripts/test_edm_converters.sh )
  set_tests_properties (test_edm_converters
    PROPERTIES
      ENVIRONMENT k4MarlinWrapper_tests_DIR=${CMAKE_CURRENT_SOURCE_DIR}
      PASS_REGULAR_EXPRESSION "INFO Application Manager Terminated successfully")

endif(BASH_PROGRAM)