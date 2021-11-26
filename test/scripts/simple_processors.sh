#!/bin/bash
# set -eu

if [ ! -d $TEST_DIR/inputFiles/ ]; then
  mkdir $TEST_DIR/inputFiles
fi

if [ ! -f $TEST_DIR/inputFiles/muons.slcio ]; then
  wget https://github.com/AIDASoft/DD4hep/raw/master/DDTest/inputFiles/muons.slcio -P $TEST_DIR/inputFiles/
fi

k4run $TEST_DIR/gaudi_opts/simple_processors.py
