#!/bin/bash
##
## Copyright (c) 2019-2023 Key4hep-Project.
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

# Clone CLICPerformance for input files
bash $TEST_DIR/scripts/setup_clic_performance.sh

cd CLICPerformance/clicConfig

export INPUTFILE=ttbar_podio230830_edm4hep_frame.root

# Download root file if not present
if [ ! -f $TEST_DIR/inputFiles/$INPUTFILE ]; then
  echo "Input file not found. Getting it from key4hep..."
  wget https://key4hep.web.cern.ch/testFiles/ddsimOutput/$INPUTFILE -P $TEST_DIR/inputFiles/
fi

# Frame based I/O exits with a user requested stop here, that makes Gaudi exit
# with an exit code of 4. See https://github.com/key4hep/k4FWCore/issues/125
k4run $TEST_DIR/gaudi_opts/clicRec_e4h_input.py || true

input_num_events=$(python $TEST_DIR/python/root_num_events.py $TEST_DIR/inputFiles/$INPUTFILE)
output_num_events=$(python $TEST_DIR/python/root_num_events.py my_output.root)

# First check do we have the same number of events in input and output
if [ "$input_num_events" != "$output_num_events" ]; then
  echo "ERROR: different number of events for input and output"
  exit 1
fi

# Second check: contents (at least superficially)
echo "Comparing contents of Output_REC_e4h_input.slcio"
if ! diff <(anajob Output_REC_e4h_input.slcio) $TEST_DIR/inputFiles/anajob_Output_REC.expected; then
  echo "File contents of REC slcio file are not as expected"
  exit 1
fi

echo "Comparing contents of Output_DST_e4h_input.slcio"
if ! diff <(anajob Output_DST_e4h_input.slcio) $TEST_DIR/inputFiles/anajob_Output_DST.expected; then
  echo "File contents of DST slcio file are not as expected"
  exit 1
fi

# echo "Comparing contents of EDM4hep output file"
# if ! diff <(podio-dump -e 0:2 my_output.root) $TEST_DIR/inputFiles/podio-dump_my_output.expected; then
#   echo "File contents of EDM4hep file are not as expected"
#   exit 1
# fi
