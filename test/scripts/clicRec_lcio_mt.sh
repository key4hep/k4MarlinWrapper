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

if [ ! -f clicReconstruction_mt.py ]; then
  ln -s $TEST_DIR/gaudi_opts/clicReconstruction_mt.py clicReconstruction_mt.py
fi

k4run clicReconstruction_mt.py --LcioEvent.Files $1

input_num_events=$(lcio_event_counter $1)
output_num_events=$(lcio_event_counter Output_REC_mt_lcio.slcio)

echo $input_num_events
echo $output_num_events

if [ "$input_num_events" == "$output_num_events" ]; then
  echo "Input and output have same number of events"
else
  echo "ERROR: different number of events for input and output"
fi
