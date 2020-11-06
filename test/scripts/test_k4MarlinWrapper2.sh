#!/bin/bash

if [ ! -d $k4MarlinWrapper_tests_DIR/inputFiles/ ]; then
  mkdir $k4MarlinWrapper_tests_DIR/inputFiles/
fi


if [ ! -f $k4MarlinWrapper_tests_DIR/inputFiles/testSimulation.slcio ]; then
  echo "Input file not found. Generating one..."
  if [ -z ${ILCSOFT} ]; then
    ILCSOFT="/cvmfs/clicdp.cern.ch/iLCSoft/lcg/97/nightly/x86_64-centos7-gcc8-opt"
  fi
  ddsim \
    --steeringFile $ILCSOFT/ClicPerformance/HEAD/clicConfig/clic_steer.py \
    --inputFiles $ILCSOFT/ClicPerformance/HEAD/Tests/yyxyev_000.stdhep -N 4 \
    --compactFile $ILCSOFT/lcgeo/HEAD/CLIC/compact/CLIC_o3_v14/CLIC_o3_v14.xml \
    --outputFile $k4MarlinWrapper_tests_DIR/inputFiles/testSimulation.slcio
fi

../run gaudirun.py $k4MarlinWrapper_tests_DIR/gaudi_opts/test_k4MarlinWrapper2.py
