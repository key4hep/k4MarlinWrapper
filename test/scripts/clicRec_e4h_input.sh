#!/bin/bash
##
## Copyright (c) 2019-2024 Key4hep-Project.
##
## This file is part of Key4hep.
## See https://key4hep.github.io/key4hep-doc/ for further info.
##
## Licensed under the Apache License, Version 2.0 (the "License");
## you may not use this file except in compliance with the License.
## You may obtain a copy of the License at
##
##     http://www.apache.org/licenses/LICENSE-2.0
##
## Unless required by applicable law or agreed to in writing, software
## distributed under the License is distributed on an "AS IS" BASIS,
## WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
## See the License for the specific language governing permissions and
## limitations under the License.
##

# exit if command or variable fails
set -eu

cd CLICPerformance/clicConfig

if [ "$2" = "--iosvc" ]; then
  iosvc="_iosvc"
  echo "Running with IO service"
  file_arg="--IOSvc.Input=$1"
elif [ "$2" = "--no-iosvc" ]; then
  iosvc=""
  echo "Running without IO service"
  file_arg="--no-iosvc --EventDataSvc.input=$1"
else
  echo "Wrong argument $2"
  return 1
fi
k4run $EXAMPLE_DIR/clicRec_e4h_input.py ${file_arg} --rec-output Output_REC_e4h_input$iosvc.slcio --dst-output Output_DST_e4h_input$iosvc.slcio --gaudi-output my_output$iosvc.root

input_num_events=$(python $TEST_DIR/python/root_num_events.py $1)
output_num_events=$(python $TEST_DIR/python/root_num_events.py my_output$iosvc.root)

# First check do we have the same number of events in input and output
if [ "$input_num_events" != "$output_num_events" ]; then
  echo "ERROR: different number of events for input and output"
  exit 1
fi

# Second check: contents (at least superficially)
# Exclude the file name since it is different when using the IOSvc
echo "Comparing contents of Output_REC_e4h_input$iosvc.slcio"
if ! diff -I "Output_REC_e4h_input.*\.slcio" <(anajob Output_REC_e4h_input$iosvc.slcio) $TEST_DIR/inputFiles/anajob_Output_REC.expected; then
  echo "File contents of REC slcio file are not as expected"
  exit 1
fi

echo "Comparing contents of Output_DST_e4h_input$iosvc.slcio"
if ! diff -I "Output_DST_e4h_input.*\.slcio" <(anajob Output_DST_e4h_input$iosvc.slcio) $TEST_DIR/inputFiles/anajob_Output_DST.expected; then
  echo "File contents of DST slcio file are not as expected"
  exit 1
fi

# echo "Comparing contents of EDM4hep output file"
# if ! diff <(podio-dump -e 0:2 my_output.root) $TEST_DIR/inputFiles/podio-dump_my_output.expected; then
#   echo "File contents of EDM4hep file are not as expected"
#   exit 1
# fi
