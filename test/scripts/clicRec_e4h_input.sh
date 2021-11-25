#!/bin/bash
# exit if command or variable fails
set -eu

# Clone CLICPerformance for input files
if [ ! -d CLICPerformance ]; then
  git clone https://github.com/iLCSoft/CLICPerformance
fi

cd CLICPerformance/clicConfig

# Download root file if not present
if [ ! -f $TEST_DIR/inputFiles/ttbar1_edm4hep.root ]; then
  echo "Input file not found. Getting it from key4hep..."
  wget https://key4hep.web.cern.ch/testFiles/ddsimOutput/ttbar1_edm4hep.root -P $TEST_DIR/inputFiles/
fi

k4run $TEST_DIR/gaudi_opts/clicRec_e4h_input.py

input_num_events=$(python $TEST_DIR/python/root_num_events.py $TEST_DIR/inputFiles/ttbar1_edm4hep.root)
output_num_events=$(python $TEST_DIR/python/root_num_events.py my_output.root)

if [ "$input_num_events" == "$output_num_events" ]; then
  echo "Input and output have same number of events"
else
  echo "ERROR: different number of events for input and output"
fi