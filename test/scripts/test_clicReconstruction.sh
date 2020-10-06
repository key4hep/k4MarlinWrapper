#!/bin/bash
# exit if command or variable fails
set -eu

# Clone CLICPerformance for input files
if [ ! -d CLICPerformance ]; then
  git clone https://github.com/iLCSoft/CLICPerformance
fi

cd CLICPerformance/clicConfig

python \
  $GMP_tests_DIR/../GMPWrapper/scripts/convertMarlinSteeringToGaudi.py \
  clicReconstruction.xml \
  clicReconstruction.py

# Generate slcio file if not present
if [ ! -f testSimulation.slcio ]; then
  echo "Input file not found. Generating one..."
  ddsim \
    --steeringFile clic_steer.py \
    --inputFiles ../Tests/yyxyev_000.stdhep -N 4 \
    --compactFile $ILCSOFT/lcgeo/HEAD/CLIC/compact/CLIC_o3_v14/CLIC_o3_v14.xml \
    --outputFile testSimulation.slcio
fi


echo "Modifying clicReconstruction.py file..."
# Replace SLCIO file path
sed -i 's|/run/simulation/with/ctest/to/create/a/file.slcio|testSimulation.slcio|g' clicReconstruction.py
# Uncomment selected optional processors
sed -i 's;EvtMax   = 10,;EvtMax   = 3,;' clicReconstruction.py
sed -i 's;"MaxRecordNumber", "10", END_TAG,;"MaxRecordNumber", "3", END_TAG,;' clicReconstruction.py
sed -i 's;# algList.append(OverlayFalse);algList.append(OverlayFalse);' clicReconstruction.py
sed -i 's;# algList.append(MyConformalTracking);algList.append(MyConformalTracking);' clicReconstruction.py
sed -i 's;# algList.append(ClonesAndSplitTracksFinder);algList.append(ClonesAndSplitTracksFinder);' clicReconstruction.py
sed -i 's;# algList.append(RenameCollection);algList.append(RenameCollection);' clicReconstruction.py
sed -i 's;"DD4hepXMLFile", ".*",; "DD4hepXMLFile", os.environ["ILCSOFT"]+"/lcgeo/HEAD/CLIC/compact/CLIC_o3_v14/CLIC_o3_v14.xml",;' clicReconstruction.py
# Change output level for correct test confirmation
sed -i 's;OutputLevel=WARNING; OutputLevel=DEBUG;' clicReconstruction.py

../../../run gaudirun.py clicReconstruction.py