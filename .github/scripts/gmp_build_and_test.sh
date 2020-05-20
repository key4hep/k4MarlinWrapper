#!/bin/bash

yum -y install make git

source /cvmfs/clicdp.cern.ch/iLCSoft/lcg/97/nightly/x86_64-centos7-gcc8-opt/init_ilcsoft.sh

# Get Gaudi and compile it
git clone https://gitlab.cern.ch/gaudi/Gaudi.git
cd Gaudi
mkdir build install; cd build
cmake \
  -DCMAKE_INSTALL_PREFIX=../install \
  -DBoost_NO_BOOST_CMAKE=TRUE \
  ..
make -j 2
make install

cd ../../

# Compile GMP pointing to Gaudi
cd /GMP
mkdir build install; cd build
cmake \
  -DCMAKE_INSTALL_PREFIX=../install \
  -DGaudi_DIR="/Gaudi/install" \
  -DGaudiProject_DIR="/Gaudi/install/cmake" \
  ..
make -j 2
make install

ctest -R test_gmp2 -V