#!/bin/bash
# set -eu

if [ ! -d $k4MarlinWrapper_tests_DIR/inputFiles/ ]; then
  mkdir $k4MarlinWrapper_tests_DIR/inputFiles
fi

if [ ! -f $k4MarlinWrapper_tests_DIR/inputFiles/muons.slcio ]; then
  wget https://github.com/AIDASoft/DD4hep/raw/master/DDTest/inputFiles/muons.slcio -P $k4MarlinWrapper_tests_DIR/inputFiles/
fi

../run k4run $k4MarlinWrapper_tests_DIR/gaudi_opts/same_num_io.py --num-events=-1

input_num_events=$(lcio_event_counter $k4MarlinWrapper_tests_DIR/inputFiles/muons.slcio)
output_num_events=$(lcio_event_counter Output_DST.slcio)

if [ "$input_num_events" == "$output_num_events" ] || [ "$input_num_events" == 10 ]; then
  echo "Input and output have same number of events"
else
  echo "ERROR: different number of events for input and output"
fi
