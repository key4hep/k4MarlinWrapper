#!/bin/bash

source /cvmfs/sft.cern.ch/lcg/releases/clang/10.0.0-62e61/x86_64-centos7/setup.sh

clang-format -Werror --verbose --dry-run --style=file  $(find ./GMPWrapper/ -name "*.cpp" -o -name "*.h" ) 

exit $?
