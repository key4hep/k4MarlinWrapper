#!/bin/bash

if [ ! -d $k4MarlinWrapper_tests_DIR/inputFiles/ ]; then
  mkdir $k4MarlinWrapper_tests_DIR/inputFiles/
fi


if [ ! -f $k4MarlinWrapper_tests_DIR/inputFiles/testSimulation.slcio ]; then
  echo "Input file not found. Getting it from key4hep..."
  wget https://key4hep.web.cern.ch/testFiles/ddsimOutput/testSimulation.slcio -P $k4MarlinWrapper_tests_DIR/inputFiles/
fi

k4run $k4MarlinWrapper_tests_DIR/gaudi_opts/test_k4MarlinWrapper2.py
