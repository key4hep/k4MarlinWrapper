# Running an Event Display with EDM4hep input

It is possible to run the [C Event Display (CED)](https://github.com/iLCSoft/CED) via a wrapped [CEDViewer](https://github.com/iLCSoft/CEDViewer) Marlin Processor. This makes it possible to run the Event Dispaly with EDM4hep input files using an on the fly conversion to LCIO for CED. This introduction shows the basic concepts and also provides a options file that should work for most use cases. This example will be using the CLIC detector but should also work for other DD4hep detector models. The example is fully self contained, if you already have everything set up you can jump directly to [running the event display](#running-the-event-display).

## Setting up an environment

The following steps have been tested with the Key4hep nightly builds release which can be setup using
```bash
source /cvmfs/sw-nightlies.hsf.org/key4hep/setup.sh
```

To get the CLIC detecor description we clone the `CLICPerformance` repository
```bash
git clone https://github.com/iLCSoft/CLICPerformance
```

All the following steps assume that the environment is setup like above and that the detector description is in the `CLICPerformance` directory. All commands start from the diretory from which `git clone` has been executed.

## Creating an input file

To create an input file for the event display we run a simple detector simulation using `ddsim` and a particle gun that shoots photons. The input file that we create here for illustration purposes has only 10 events, which also means that the creation should only take a few minutes. The steps to create this file are the following 

```
ddsim --steeringFile CLICPerformance/clicConfig/clic_steer.py \
      --compactFile $LCGEO/CLIC/compact/CLIC_o3_v14/CLIC_o3_v14.xml \
      --enableGun \
      --gun.distribution uniform \
      --gun.particle gamma \
      --gun.energy 10*GeV \
      --outputFile gamma_10GeV_edm4hep.root \
      --numberOfEvents 10
```

You should now have a `gamma_10GeV_edm4hep.root` file containing 10 events.

## Runing the event display

In order to run the event display via the `DDCEDViewer` we use the Marlin wrapper and attach an EDM4hep to LCIO converter to the wrapped processor. In the following we will build the complete Gaudi options file step by step. Here we simply present the most important steps, but do not go over all details of the `DDCEDViewer` configuration, for that it is probably best to directly look at the [CEDViewer repository](https://github.com/iLCSoft/CEDViewer) directly. The complete Gaudi configuration can be found in [`k4MarlinWrapper/examples/event_display.py`](https://github.com/key4hep/k4MarlinWrapper/blob/master/k4MarlinWrapper/examples/event_display.py) which is also installed at `$K4MARLINWRAPPER/examples/event_display.py`

To read EDM4hep input we use the `PodioInput` module and the `k4DataSvc`
```python
from Gaudi.Configuration import *
from Configurables import k4DataSvc, PodioInput

algList = []

evtsvc = k4DataSvc('EventDataSvc')
evtsvc.input = ''

inp = PodioInput('InputReader')
inp.collections = [
   # ... all collections that should be read ...
]

algList.append(inp)
```

The `DDCEDViewer` also requires setting up a DD4hep geometry, which can simply be done by wrapping the `InitializeDD4hep` Marlin Processor
```python
from Configurables import MarlinProcessorWrapper

MyInitializeDD4hep = MarlinProcessorWrapper("MyInitializeDD4hep")
MyInitializeDD4hep.OutputLevel = INFO
MyInitializeDD4hep.ProcessorType = "InitializeDD4hep"
MyInitializeDD4hep.Parameters = {
                                 "DD4hepXMLFile": ["CLICPerformance/Visualisation/CLIC_o3_v06_CED/CLIC_o3_v06_CED.xml"]
                                 }
                                 
algList.append(MyInitializeDD4hep)
```
Note that in this case we already have passed in the geometry that we want to use for the event display.

Finally, the main work is done by the `DDCEDViewer`, which we again use via the `MarlinProcessorWrapper` (omitting a lot of the detailed configuration here). In order to convert the EDM4hep inputs to LCIO we attach an `EDM4hep2LcioTool` to this algorithm
```python
from Configurables import EDM4hep2LcioTool

MyCEDViewer = MarlinProcessorWrapper("MyCEDViewer")
MyCEDViewer.OutputLevel = INFO
MyCEDViewer.ProcessorType = "DDCEDViewer"
MyCEDViewer.Parameters = {
                          # ... lots of CEDViewer configuration ...
                          }
                          
# EDM4hep to LCIO converter
edmConvTool = EDM4hep2LcioTool("EDM4hep2lcio")
edmConvTool.Parameters = ["*"]
edmConvTool.OutputLevel = DEBUG
MyCEDViewer.EDM4hep2LcioTool = edmConvTool

algList.append(MyCEDViewer)
```

The only thing that is left to do now is to put everything into the `ApplicationManager` in order to run things
```python
from Configurables import ApplicationMgr
ApplicationMgr( TopAlg = algList,
                EvtSel = 'NONE',
                EvtMax   = 10,
                ExtSvc = [evtsvc],
                OutputLevel=INFO
              )
```

With all these pieces put together (as in `examples/event_display.py`) it is no possible to run the event display. In order to run the event display we first have to start the `glced` server program to which the wrapped `CEDViewer` processor will then connect. Starting the server and running the wrapped processor can be done via
```bash
glced &

k4run $K4MARLINWRAPPER/examples/event_display.py --EventDataSvc.input=gamma_10GeV_edm4hep.root
```

## Creating a Gaudi options file

The `event_display.py` options file that is used above and that is present in the examples has been produced following these steps:
- Using an `.slcio` input file and the desired geometry, run `ced2go` with the desired arguments.
  - This produces a Marlin xml steering file on the fly and stores it at `/tmp/ced2go_${USER}_steering.xml`
- Using the `convertMarlinSteeringToGaudi.py` converter script convert this into a gaudi options file
- Exchange the LCIO input reading by the podio input reading (see above)
- Attach the `EDM4hep2LcioTool` to the wrapped `CEDViewer` processor

This should allow to arrive at a similar steering file even for slightly different configurations. One potential pitfal is the slightly different naming of the `ddsim` outputs between LCIO and EDM4hep (see [this issue](https://github.com/AIDASoft/DD4hep/issues/921)). For the `event_display.py` in the `examples` drawing the `MCParticle`s in the event display required to change the name of the collection that is used in the `DrawInLayers` configuration parameter of the `CEDViewer`:

```diff
@@ -156,7 +156,7 @@ MyCEDViewer.Parameters = {
                               "MarlinTrkTracks", "0", "6", "7",
                               "PandoraClusters", "0", "3", "8",
                               "PandoraPFOs", "0", "3", "9",
-                              "MCParticle", "0", "3", "0",
+                              "MCParticles", "0", "3", "0",
                               "VertexBarrelHits", "0", "5", "11",
                               "VertexEndcapHits", "0", "5", "11",
                               "InnerTrackerBarrelHits", "0", "5", "11",
```
