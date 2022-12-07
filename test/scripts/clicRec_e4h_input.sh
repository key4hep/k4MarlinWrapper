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

# First check do we have the same number of events in input and output
if [ "$input_num_events" != "$output_num_events" ]; then
  echo "ERROR: different number of events for input and output"
  exit 1
fi

# Second check: contents (at least superficially)
echo "Comparing contents of Output_REC.slcio"
if ! diff <(anajob Output_REC.slcio) $TEST_DIR/inputFiles/anajob_Output_REC.expected; then
  echo "File contents of REC slcio file are not as expected"
  exit 1
fi

echo "Comparing contents of Output_DST.slcio"
if ! diff <(anajob Output_DST.slcio) $TEST_DIR/inputFiles/anajob_Output_DST.expected; then
  echo "File contents of DST slcio file are not as expected"
  exit 1
fi

echo "Comparing contents of EDM4hep output file"
if ! diff <(podio-dump -e 0:2 my_output.root) $TEST_DIR/inputFiles/podio-dump_my_output.expected; then
  echo "File contents of EDM4hep file are not as expected"
  exit 1
fi
