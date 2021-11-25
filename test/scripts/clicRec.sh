#!/bin/bash
# exit if command or variable fails
set -eu

# Clone CLICPerformance for input files
if [ ! -d CLICPerformance ]; then
  git clone https://github.com/iLCSoft/CLICPerformance
fi

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
sed -i 's|/run/simulation/with/ctest/to/create/a/file.slcio|$TEST_DIR/inputFiles/testSimulation.slcio|g' clicReconstruction.py
# Uncomment selected optional processors
sed -i 's;EvtMax   = 10,;EvtMax   = 3,;' clicReconstruction.py
sed -i 's;"MaxRecordNumber": ["10"],;"MaxRecordNumber": ["3"],;' clicReconstruction.py
sed -i 's;# algList.append(OverlayFalse);algList.append(OverlayFalse);' clicReconstruction.py
sed -i 's;# algList.append(MyConformalTracking);algList.append(MyConformalTracking);' clicReconstruction.py
sed -i 's;# algList.append(ClonesAndSplitTracksFinder);algList.append(ClonesAndSplitTracksFinder);' clicReconstruction.py
sed -i 's;# algList.append(RenameCollection);algList.append(RenameCollection);' clicReconstruction.py
sed -i 's;"DD4hepXMLFile": \[".*"\],; "DD4hepXMLFile": \[os.environ["LCGEO"]+"/CLIC/compact/CLIC_o3_v14/CLIC_o3_v14.xml"\],;' clicReconstruction.py
# Change output level for correct test confirmation
sed -i 's;OutputLevel=WARNING; OutputLevel=DEBUG;' clicReconstruction.py

k4run clicReconstruction.py
