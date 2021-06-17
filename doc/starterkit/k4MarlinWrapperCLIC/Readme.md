# Using the Key4hep-Stack for CLIC Simulation and Reconstruction


This assumes that you have access to an installation of the Key4hep-stack, either via ``CVMFS`` or ``spack install``.
To setup the installation on cvmfs, do:

```
source /cvmfs/sw.hsf.org/key4hep/setup.sh
```

These commands will explain how one can run the CLIC detector simulation and reconstruction using the Key4hep-Stack.
First we will obtain all the necessary steering and input files for CLIC, simulate a few events and run the
reconstruction both with ``Marlin`` and ``k4run``. These steps can be adapted to simulate or run other ``Marlin``
processors as well.

The ``CLICPerformance`` repository contains the steering and input files.

```bash
git clone https://github.com/iLCSoft/CLICPerformance
```

## Simulation

Now we can already simulate a few events:

```bash
cd CLICPerformance/clicConfig

ddsim --compactFile $LCGEO/CLIC/compact/CLIC_o3_v14/CLIC_o3_v14.xml \
      --outputFile ttbar.slcio \
      --steeringFile clic_steer.py \
      --inputFiles ../Tests/yyxyev_000.stdhep \
      --numberOfEvents 3
```

## Reconstruction

### Reconstruction with Marlin

To run the reconstruction with ``Marlin``:

```bash
cd CLICPerformance/clicConfig

Marlin clicReconstruction.xml \
       --InitDD4hep.DD4hepXMLFile=$LCGEO/CLIC/compact/CLIC_o3_v14/CLIC_o3_v14.xml \
       --global.LCIOInputFiles=ttbar.slcio \
       --global.MaxRecordNumber=3
```

### Reconstruction with with Gaudi

We can convert the ``xml`` steering file to a Gaudi steering file

```bash
cd CLICPerformance/clicConfig

convertMarlinSteeringToGaudi.py clicReconstruction.xml clicReconstruction.py
```

Now we need to modify the ``clicReconstruction.py`` file to point to the ``ttbar.slcio`` input file, and change the
``DD4hepXMLFile`` parameter for the ``InitDD4hep`` algorithm.  In addition the two processors with the comment ``#
Config.OverlayFalse`` and ``# Config.TrackingConformal`` should be enabled by uncommenting their line in the ``algList``
at the end of the file.

```bash
cd CLICPerformance/clicConfig

sed -i 's;read.Files = \[".*"\];read.Files = \["ttbar.slcio"\];' clicReconstruction.py
sed -i 's;EvtMax   = 10,;EvtMax   = 3,;' clicReconstruction.py
sed -i 's;"MaxRecordNumber": ["10"],;"MaxRecordNumber": ["3"],;' clicReconstruction.py
sed -i 's;# algList.append(OverlayFalse);algList.append(OverlayFalse);' clicReconstruction.py
sed -i 's;# algList.append(MyConformalTracking);algList.append(MyConformalTracking);' clicReconstruction.py
sed -i 's;# algList.append(ClonesAndSplitTracksFinder);algList.append(ClonesAndSplitTracksFinder);' clicReconstruction.py
sed -i 's;# algList.append(RenameCollection);algList.append(RenameCollection);' clicReconstruction.py
sed -i 's;"DD4hepXMLFile", ".*",; "DD4hepXMLFile", os.environ["LCGEO"]+"/CLIC/compact/CLIC_o3_v14/CLIC_o3_v14.xml",;' clicReconstruction.py

```

Then the reconstruction using the wrapper can be run with

```bash
cd CLICPerformance/clicConfig

k4run clicReconstruction.py
```
