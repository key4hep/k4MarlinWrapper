#!/bin/bash

yum -y install make git

source /cvmfs/clicdp.cern.ch/iLCSoft/lcg/97/nightly/x86_64-centos7-gcc8-opt/init_ilcsoft.sh

# Compile GMP pointing to Gaudi
cd /GMP
mkdir build install; cd build
cmake \
  -DCMAKE_INSTALL_PREFIX=../install \
  -DGaudi_DIR="/shared_gaudi_volume" \
  -DGaudiProject_DIR="/shared_gaudi_volume/cmake" \
  -DHOST_BINARY_TAG=skylake-centos7-gcc8-opt \
  .. && \
make -j 2 && \
make install && \
ctest --verbose --output-on-failure
