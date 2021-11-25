#!/bin/bash
# set -eu

if [ ! -d $TEST_DIR/inputFiles/ ]; then
  mkdir $TEST_DIR/inputFiles
fi

if [ ! -f $TEST_DIR/inputFiles/muons.slcio ]; then
  wget https://github.com/AIDASoft/DD4hep/raw/master/DDTest/inputFiles/muons.slcio -P $TEST_DIR/inputFiles/
fi

../run k4run $TEST_DIR/gaudi_opts/same_num_io.py --num-events=-1

input_num_events=$(lcio_event_counter $TEST_DIR/inputFiles/muons.slcio)
output_num_events=$(lcio_event_counter Output_DST.slcio)

if [ "$input_num_events" == "$output_num_events" ]; then
  echo "Input and output have same number of events"
else
  echo "ERROR: different number of events for input and output"
fi
