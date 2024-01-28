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

python \
  $TEST_DIR/../k4MarlinWrapper/scripts/convertMarlinSteeringToGaudi.py \
  clicReconstruction.xml \
  clicReconstruction.py

# Generate slcio file if not present
if [ ! -f $TEST_DIR/inputFiles/testSimulation.slcio ]; then
  echo "Input file not found. Getting it from key4hep..."
  wget https://key4hep.web.cern.ch/testFiles/ddsimOutput/testSimulation.slcio -P $TEST_DIR/inputFiles/
fi


echo "Modifying clicReconstruction.py file..."
# Replace SLCIO file path
# sed -i 's|/run/simulation/with/ctest/to/create/a/file.slcio|testSimulation.slcio|g' clicReconstruction.py
sed -i '1s/^/import os\n/' clicReconstruction.py
sed -i 's|/run/simulation/with/ctest/to/create/a/file.slcio|$TEST_DIR/inputFiles/testSimulation.slcio|g' clicReconstruction.py
# Uncomment selected optional processors
sed -i 's;EvtMax   = 10,;EvtMax   = 3,;' clicReconstruction.py
sed -i 's;"MaxRecordNumber": ["10"],;"MaxRecordNumber": ["3"],;' clicReconstruction.py
sed -i 's;# algList.append(OverlayFalse);algList.append(OverlayFalse);' clicReconstruction.py
sed -i 's;# algList.append(MyConformalTracking);algList.append(MyConformalTracking);' clicReconstruction.py
sed -i 's;# algList.append(ClonesAndSplitTracksFinder);algList.append(ClonesAndSplitTracksFinder);' clicReconstruction.py
sed -i 's;# algList.append(RenameCollection);algList.append(RenameCollection);' clicReconstruction.py
sed -i 's;"DD4hepXMLFile": \[".*"\],; "DD4hepXMLFile": \[os.environ["K4GEO"]+"/CLIC/compact/CLIC_o3_v14/CLIC_o3_v14.xml"\],;' clicReconstruction.py
# Change output level for correct test confirmation
sed -i 's;OutputLevel=WARNING; OutputLevel=DEBUG;' clicReconstruction.py

k4run clicReconstruction.py
