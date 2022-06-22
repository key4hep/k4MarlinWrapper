# Event Data Model converters

The converters are implemented as Gaudi Tools: these can be attached to any Gaudi Algorithm defined in the options file.

- [Reading events](#reading-events)
- [Writing events](#writing-events)
- [Event Data Model converters](#Event-Data-Model-converters)
  + [EDM4hep to LCIO converter](#EDM4hep-to-LCIO-converter)
  + [LCIO to EDM4hep converter](#LCIO-to-EDM4hep-converter)

## Reading events

Read **EDM4hep** events:

1. Instantiate the Event Service from k4FWCore: `k4DataSvc`
2. Indicate the event file to read.
3. Then use the algorithm `PodioInput` to select the collections to read by their name.
4. Add the algorithm `PodioInput` to the algorithm list.

```python
from Configurables import k4DataSvc, PodioInput

# 1
evtsvc = k4DataSvc('EventDataSvc')
# 2
evtsvc.input = 'path/to/file.root' 

# 3
inp = PodioInput('InputReader')
inp.collections = [
    'ReconstructedParticles',
    'EFlowTrack'
]
inp.OutputLevel = DEBUG

# 4
algList.append(inp)
```

===

Read **LCIO** events:

1. Instantiate the Event Data Service.
2. Use `LcioEvent` and indicate the event file to read.
3. Add the algorithm `LcioEvent` to the algorithm list.

```python
from Configurables import EventDataSvc, LcioEvent

# 1
evtsvc = EventDataSvc()

# 2
read = LcioEvent()
read.OutputLevel = DEBUG
read.Files = ["path/to/file.slcio"]

# 3
algList.append(read)
```

## Writing events

Write **EDM4hep** events:

1. Instantiate `PodioOutput` and indicate event file to write.
2. Select command for `PodioOutput`.
3. Add the `PodioOutput` to the algorithm list.

```python
from Configurables import PodioOutput

# 1
out = PodioOutput("PodioOutput", filename = "my_output.root")
# 2
out.outputCommands = ["keep *"]

# 3
algList.append(out)
```

===

Write **LCIO** events: 

1. Instantiate a `MarlinProcessorWrapper` with a relevant name.
2. Indicate the `ProcessorType` to be `LCIOOutputProcessor`.
3. Indicate the relevant parameters for the Marlin Processor.
4. Add the `MarlinProcessorWrapper` to the algorithm list.

```python
from Configurables import MarlinProcessorWrapper

# 1
Output_DST = MarlinProcessorWrapper("Output_DST")
Output_DST.OutputLevel = WARNING
# 2
Output_DST.ProcessorType = "LCIOOutputProcessor"
# 3
Output_DST.Parameters = {
                         "DropCollectionNames": [],
                         "DropCollectionTypes": ["MCParticle", "LCRelation", "SimCalorimeterHit"],
                         ...
                         }

# 4                         
algList.append(Output_DST)                         
```


## Event Data Model converters

Note and review the following when using the converters:

- Collection names must be unique.
- If using the `PodioInput` to read events: collection names must be indicated both in the `PodioInput` and the `EDM4hep2LcioTool`.
- Collections not indicated to be converted **will not** be converted even if its a dependency from an indicated collection to be converted.
- If a converted collection is used later by a Gaudi Algorithm, and this Gaudi Algorithm indicates the use of that collection in the `Parameters`, the converted collection name must match the name indicated in the Gaudi Algorithm `Parameters`.
  + For example: A collection may be converted with the following parameters: `ReconstructedParticles", "ReconstructedParticleLCIO"`
  + A Gaudi Algorithm may indicate in their `Parameters`: `"PFOCollection": ["ReconstructedParticleLCIO"]`

###  EDM4hep to LCIO converter

Collections from events that are already read, or are produced by a Gaudi Algorithm can be converted from EDM4hep to LCIO format:

1. Instantiate the `EDM4hep2LcioTool` Gaudi Tool.
2. Indicate the collections to convert in `Parameters`.
  + To convert all available collections write an asterisk, like follows: `edmConvTool.Parameters = ["*"]`
  + Arguments are read in groups of 2: name of the EDM4hep collection, name of the LCIO converted collection.
3. Select the Gaudi Algorithm that will convert the indicated collections.
4. Add the Tool to the Gaudi Algorithm.

```python
from Configurables import ToolSvc, EDM4hep2LcioTool

# 1
edmConvTool = EDM4hep2LcioTool("EDM4hep2lcio")
# 2
edmConvTool.Parameters = [
    "EFlowTrack", "EFlowTrack_LCIO",
    "ReconstructedParticles", "ReconstructedParticle_LCIO"
]
edmConvTool.OutputLevel = DEBUG

# 3
InitDD4hep = MarlinProcessorWrapper("InitDD4hep")

# 4
InitDD4hep.EDM4hep2LcioTool=edmConvTool
```

### LCIO to EDM4hep converter

Collections from events that are already read, or are produced by a gaudi Algorithm can be converted from LCIO to EDM4hep format:

1. Instantiate the `Lcio2EDM4hepTool` Gaudi Tool.
2. Indicate the collections to convert in `Parameters`.
  + To convert all available collections write an asterisk, like follows: `lcioConvTool.Parameters = ["*"]`
  + Arguments are read in groups of 2: name of the LCIO collection, name of the EDM4hep converted collection.
3. Select the Gaudi Algorithm that will convert the indicated collections.
4. Add the Tool to the Gaudi Algorithm.

```python
from Configurables import ToolSvc, Lcio2EDM4hepTool

# 1
lcioConvTool = Lcio2EDM4hepTool("LCIO2EDM4hep")
# 2
lcioConvTool.Parameters = [
    "EFlowTrackConv", "EFlowTrackEDM4hep",
    "ReconstructedParticle", "ReconstructedParticlesEDM4hep",
    "BuildUpVertices", "BuildUpVerticesEDM4hep",
    "PrimaryVertices", "PrimaryVerticesEDM4hep"
]
lcioConvTool.OutputLevel = DEBUG

# 3
JetClusteringAndRefiner = MarlinProcessorWrapper("JetClusteringAndRefiner")

# 4
JetClusteringAndRefiner.Lcio2EDM4hepTool=lcioConvTool
```


