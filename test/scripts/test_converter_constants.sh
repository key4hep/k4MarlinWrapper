#!/bin/bash
# set -eu

if [ ! -d $GMP_tests_DIR/inputFiles/ ]; then
  mkdir $GMP_tests_DIR/inputFiles
fi

if [ ! -f $GMP_tests_DIR/inputFiles/muons.slcio ]; then
  wget https://github.com/AIDASoft/DD4hep/raw/master/DDTest/inputFiles/muons.slcio -P $GMP_tests_DIR/inputFiles/
fi

# Use converter over test file
python \
  $GMP_tests_DIR/../GMPWrapper/scripts/convertMarlinSteeringToGaudi.py \
  $GMP_tests_DIR/inputFiles/testConverterConstants.xml \
  $GMP_tests_DIR/gaudi_opts/testConverterConstants.py

../run gaudirun.py $GMP_tests_DIR/gaudi_opts/testConverterConstants.py
