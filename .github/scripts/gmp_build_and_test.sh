#!/bin/bash

yum -y install make git wget

source /cvmfs/sw.hsf.org/key4hep/setup.sh

# Compile GMP pointing to Gaudi
cd /GMP
mkdir build install; cd build
cmake \
  -DCMAKE_INSTALL_PREFIX=../install \
  .. && \
make -j 2 && \
make install && \
ctest --verbose --output-on-failure
