<!--
Copyright (c) 2019-2024 Key4hep-Project.

This file is part of Key4hep.
See https://key4hep.github.io/key4hep-doc/ for further info.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
-->
# Running an Event Display with EDM4hep or LCIO input

It is possible to run the [C Event Display (CED)](https://github.com/iLCSoft/CED) via a wrapped [CEDViewer](https://github.com/iLCSoft/CEDViewer) Marlin Processor. This makes it possible to run the Event Display with EDM4hep input files using an on the fly conversion to LCIO for CED. This introduction shows the basic concepts and also provides a options file that should work for most use cases. This example will be using the CLIC detector but should also work for other DD4hep detector models. The example is fully self contained, if you already have everything set up you can jump directly to [running the event display](#running-the-event-display).

## Setting up an environment

The following steps have been tested with the latest Key4hep release which can be setup using
```bash
source /cvmfs/sw.hsf.org/key4hep/setup.sh
```

To get the CLIC detector description we clone the `CLICPerformance` repository
```bash
git clone https://github.com/iLCSoft/CLICPerformance
```

All the following steps assume that the environment is setup like above and that the detector description is in the `CLICPerformance` directory. All commands start from the directory from which `git clone` has been executed.

## Creating an input file

To create an input file for the event display we run a simple detector simulation using `ddsim` and a particle gun that shoots photons. The input file that we create here for illustration purposes has only 10 events, which also means that the creation should only take a few minutes. The steps to create this file are the following

```bash
ddsim --steeringFile CLICPerformance/clicConfig/clic_steer.py \
      --compactFile $K4GEO/CLIC/compact/CLIC_o3_v14/CLIC_o3_v14.xml \
      --enableGun \
      --gun.distribution uniform \
      --gun.particle gamma \
      --gun.energy 10*GeV \
      --outputFile gamma_10GeV_edm4hep.root \
      --numberOfEvents 10
```

You should now have a `gamma_10GeV_edm4hep.root` file containing 10 events.

## Running the event display

In order to run the event display via the `DDCEDViewer` we use the Marlin wrapper. Here we simply present the most important steps, but do not go over all details of the `DDCEDViewer` configuration, for that it is probably best to directly look at the [CEDViewer repository](https://github.com/iLCSoft/CEDViewer) directly. The complete Gaudi configuration can be found in [`k4MarlinWrapper/examples/event_display.py`](https://github.com/key4hep/k4MarlinWrapper/blob/master/k4MarlinWrapper/examples/event_display.py) which is also installed at `$K4MARLINWRAPPER/examples/event_display.py`

In order to run the event display we first have to start the `glced` server program to which the wrapped `CEDViewer` processor will then connect. Starting the server and running the wrapped processor can be done via
```bash
glced &

k4run $K4MARLINWRAPPER/examples/event_display.py --inputFiles=gamma_10GeV_edm4hep.root
```

If you want to run the event display for a different geometry you can do so with the `--compactFile` argument. However, depending on your detector model you might also need to change some of the `DDCEDViewer` parameters. The default compact file is `"CLICPerformance/Visualisation/CLIC_o3_v06_CED/CLIC_o3_v06_CED.xml"`.


## Details

The main work is done by the `DDCEDViewer`, which we use via the `MarlinProcessorWrapper`. It is the following part of example `event_display.py`.

```python
from Configurables import MarlinProcessorWrapper

MyCEDViewer = MarlinProcessorWrapper("MyCEDViewer")
MyCEDViewer.ProcessorType = "DDCEDViewer"
MyCEDViewer.Parameters = {
                          # ... lots of CEDViewer configuration ...
                          }
```
Some of the more commonly used parameters have self explanatory names.

## Troubleshooting / Using other detectors

When running this for a detector different than CLD, CLIC or ILD (or a derivative of one of them) you might not see anything in the event display.

If you are not seeing the detector make sure that it has the proper visualisation attributes, e.g. by comparing it to the compact file used above.
If you are not seeing any hits, tracks etc. from your event, make sure to add their collection names to the `DrawInLayer` list in the `DDCEDViewer` parameters.
