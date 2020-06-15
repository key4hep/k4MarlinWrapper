# get_host_binary_tag.py is not executable in normal gaudi installations,
# which silently fails in the BinaryTagUtils from Gaudi

if (NOT HOST_BINARY_TAG)

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

else()

  set(BINARY_TAG ${HOST_BINARY_TAG})

endif()

