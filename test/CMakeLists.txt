find_program(BASH_PROGRAM bash)
if (BASH_PROGRAM)
  add_test( test_gmp1 ${BASH_PROGRAM} ${CMAKE_CURRENT_SOURCE_DIR}/scripts/test_gmp1.sh )
  set_tests_properties ( test_gmp1 
    PROPERTIES 
      ENVIRONMENT GMP_tests_DIR=${CMAKE_CURRENT_SOURCE_DIR}
      PASS_REGULAR_EXPRESSION "INFO Application Manager Terminated successfully")
endif(BASH_PROGRAM) 