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

  # # Test the lcio to edm4hep converter
  # add_test( test_edm4hep2lcio_conversion ${BASH_PROGRAM} ${CMAKE_CURRENT_SOURCE_DIR}/scripts/test_edm4hep2lcio_converter.sh )
  # set_tests_properties (test_edm4hep2lcio_conversion
  #   PROPERTIES
  #     ENVIRONMENT k4MarlinWrapper_tests_DIR=${CMAKE_CURRENT_SOURCE_DIR}
  #     PASS_REGULAR_EXPRESSION "INFO Application Manager Terminated successfully"
  #     REQUIRED_FILES ${CMAKE_CURRENT_SOURCE_DIR}/inputFiles/edm4hep2lcio.root
  #     TIMEOUT 60)

endif(BASH_PROGRAM)