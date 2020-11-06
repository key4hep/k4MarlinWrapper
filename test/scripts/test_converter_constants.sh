#!/bin/bash
# set -eu

if [ ! -d $k4MarlinWrapper_tests_DIR/inputFiles/ ]; then
  mkdir $k4MarlinWrapper_tests_DIR/inputFiles
fi

if [ ! -f $k4MarlinWrapper_tests_DIR/inputFiles/muons.slcio ]; then
  wget https://github.com/AIDASoft/DD4hep/raw/master/DDTest/inputFiles/muons.slcio -P $k4MarlinWrapper_tests_DIR/inputFiles/
fi

# Use converter over test file
python \
  $k4MarlinWrapper_tests_DIR/../k4MarlinWrapper/scripts/convertMarlinSteeringToGaudi.py \
  $k4MarlinWrapper_tests_DIR/inputFiles/testConverterConstants.xml \
  $k4MarlinWrapper_tests_DIR/gaudi_opts/testConverterConstants.py

../run gaudirun.py $k4MarlinWrapper_tests_DIR/gaudi_opts/testConverterConstants.py
