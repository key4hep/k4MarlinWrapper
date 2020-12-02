# get_host_binary_tag.py is not executable in normal gaudi installations,
# which silently fails in the BinaryTagUtils from Gaudi

if( NOT DEFINED ENV{LCG_VERSION})
  set(HOST_BINARY_TAG x86_64-linux-gcc9-opt)
endif()
find_package(Gaudi)
