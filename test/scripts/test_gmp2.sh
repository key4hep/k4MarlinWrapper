#!/bin/bash

if [ ! -d $GMP_tests_DIR/inputFiles/ ]; then
  mkdir $GMP_tests_DIR/inputFiles/
fi


if [ ! -f $GMP_tests_DIR/inputFiles/testSimulation.slcio ]; then
  echo "Input file not found. Generating one..."
  ddsim \
    --steeringFile $ILCSOFT/ClicPerformance/HEAD/clicConfig/clic_steer.py \
    --inputFiles $ILCSOFT/ClicPerformance/HEAD/Tests/yyxyev_000.stdhep -N 4 \
    --compactFile $ILCSOFT/lcgeo/HEAD/CLIC/compact/CLIC_o3_v14/CLIC_o3_v14.xml \
    --outputFile $GMP_tests_DIR/inputFiles/testSimulation.slcio
fi

../run $GMP_tests_DIR/../runit/gmprun $GMP_tests_DIR/gaudi_opts/test_gmp2.py
