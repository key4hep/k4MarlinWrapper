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

# set -eu

if [ ! -d $TEST_DIR/inputFiles/ ]; then
  mkdir $TEST_DIR/inputFiles
fi

if [ ! -f $TEST_DIR/inputFiles/muons.slcio ]; then
  wget https://github.com/AIDASoft/DD4hep/raw/master/DDTest/inputFiles/muons.slcio -P $TEST_DIR/inputFiles/
fi

# Use converter over test file
python \
  $TEST_DIR/../k4MarlinWrapper/scripts/convertMarlinSteeringToGaudi.py \
  $TEST_DIR/inputFiles/converterConstants.xml \
  $TEST_DIR/gaudi_opts/converterConstants.py

k4run $TEST_DIR/gaudi_opts/converterConstants.py
