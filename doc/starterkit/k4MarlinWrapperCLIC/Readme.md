# Using the Key4hep-Stack for CLIC Simulation and Reconstruction


This assumes that you have access to an installation of the Key4hep-stack, either via ``CVMFS`` or ``spack install``.
To setup the installation on cvmfs, do:

```bash
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

Simulating a few events with `ddsim` can produce output in EDM4hep or LCIO format.

- To produce events in **EDM4hep** format one can run indicating `--outputFile <name>_edm4hep.root` to produce the
output in such format:


```bash
cd CLICPerformance/clicConfig

ddsim --compactFile $LCGEO/CLIC/compact/CLIC_o3_v14/CLIC_o3_v14.xml \
      --outputFile ttbar_edm4hep.root \
      --steeringFile clic_steer.py \
      --inputFiles ../Tests/yyxyev_000.stdhep \
      --numberOfEvents 3
```

- To produce events in **LCIO** format one can run indicating `--outputFile <name>.slcio` to produce the output file
in such format:

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

### Reconstruction with Gaudi through k4MarlinWrapper

We can convert the ``xml`` steering file to a Gaudi steering file (python):

```bash
cd CLICPerformance/clicConfig

convertMarlinSteeringToGaudi.py clicReconstruction.xml clicReconstruction.py
```

Reconstruction can be performed with LCIO or EDM4hep input, depending on the output format of the events produced
during [Simulation](#simulation).

#### Reconstruction with LCIO input

- When using **LCIO** format for the input events to be used in reconstruction:
  + Modify the ``clicReconstruction.py`` file to point to the ``ttbar.slcio`` input file, and change the
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
sed -i 's;"DD4hepXMLFile": \[".*"\],; "DD4hepXMLFile": \[os.environ["LCGEO"]+"/CLIC/compact/CLIC_o3_v14/CLIC_o3_v14.xml"\],;' clicReconstruction.py

```

Then the reconstruction using the k4MarlinWrapper can be run with

```bash
cd CLICPerformance/clicConfig

k4run clicReconstruction.py
```


#### Reconstruction with EDM4hep input

- When using **EDM4hep** format for the input events to be used in reconstruction, refer to the [**EDM converters**](https://github.com/key4hep/k4MarlinWrapper/blob/master/doc/edmConverters.md)
included with k4MarlinWrapper. Note that:
  + *MarlinProcessorWrappers* need input in LCIO format: EDM4hep collections need to be converted to LCIO
  + The output collections of *MarlinProcessorWrappers* may be used later by other algorithms:
    * Output collections of *MarlinProcessorWrappers* will be in LCIO format unless these are explicitly converted
    * Some *MarlinProcessorWrappers* may modify collections instead of producing new ones: the original EDM4hep collection wont be updated in this case and would need conversion from LCIO to EDM4hep.

- To run *clicReconstruction* with EDM4hep format, use the steering file found in the `test` folder of k4MarlinWrapper:
`test/gaudi_opts/clicRec_e4h_input.py`
  + Change line `evtsvc.input = '$TEST_DIR/inputFiles/ttbar1_edm4hep.root'` to point to the location of your input file.
  + At the bottom of the file, in the `ApplicationMgr` parameters, change `EvtMax   = 3,` to the number of events to run.


This can be run with:

```bash
cd CLICPerformance/clicConfig

cp ../../../test/gaudi_opts/clicRec_e4h_input.py .

k4run clicRec_e4h_input.py --EventDataSvc.input ttbar_edm4hep.root
```

