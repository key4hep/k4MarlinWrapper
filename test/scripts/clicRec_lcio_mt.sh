#!/bin/bash
# exit if command or variable fails
set -eu

# Clone CLICPerformance for input files
if [ ! -d CLICPerformance ]; then
  git clone https://github.com/iLCSoft/CLICPerformance
fi

cd CLICPerformance/clicConfig

# Generate slcio file if not present
if [ ! -f $TEST_DIR/inputFiles/testSimulation.slcio ]; then
  echo "Input file not found. Getting it from key4hep..."
  wget https://key4hep.web.cern.ch/testFiles/ddsimOutput/testSimulation.slcio -P $TEST_DIR/inputFiles/
fi

if [ ! -f clicReconstruction_mt.py ]; then
  ln -s $TEST_DIR/gaudi_opts/clicReconstruction_mt.py clicReconstruction_mt.py
fi

../../../run k4run clicReconstruction_mt.py

input_num_events=$(lcio_event_counter $TEST_DIR/inputFiles/testSimulation.slcio)
output_num_events=$(lcio_event_counter MT_Output_REC.slcio)

echo $input_num_events 
echo $output_num_events

if [ "$input_num_events" == "$output_num_events" ]; then
  echo "Input and output have same number of events"
else
  echo "ERROR: different number of events for input and output"
fi