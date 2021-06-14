#!/bin/bash

yum -y install make git wget

# source /cvmfs/sw.hsf.org/key4hep/setup.sh
source /cvmfs/sw-nightlies.hsf.org/key4hep/setup.sh

# Compile k4MarlinWrapper
cd /k4MarlinWrapper
mkdir build install; cd build
cmake \
  -DCMAKE_INSTALL_PREFIX=../install \
  -DUSE_EXTERNAL_CATCH2=OFF \
  .. && \
make -j 2 && \
make install && \
ctest --verbose --output-on-failure
