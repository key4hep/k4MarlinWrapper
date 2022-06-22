# How to run multithreading with k4MarlinWrapper (Gaudi)

**Supported**
- Reading LCIO events with `LcioEvent()`
- Writing LCIO events with `LcioEventOutput()`
- Running `MarlinProcessorWrapper`s with **no** converters
- Using `whiteboard` as `ExtSvc` (no k4DataSvc or EventDataSvc)

**Not supported**
- Using EDM converters `EDM4hep2LcioTool` and `Lcio2EDM4hepTool()`
- Reading EDM4hep events with `PodioInput()`
- Writing EDM4hep events with `PodioOutput()`
- Writing EDM4hep events with `MarlinProcessorWrapper` of type `LCIOOutputProcessor`
- Running non-thread algorithms/processors in parallel

## Running Gaudi with multithreading support

Gaudi uses [Intel TBB](https://oneapi-src.github.io/oneTBB/) under the hood for multithreading.

Gaudi exposes two main levels of parallelism:
- Inter-event parallelism: running multiple events in parallel
- Intra-event parallelism: running multiple algorithms in parallel, within an event

The two levels of parallelims can be combined: events can run in parallel, and algorithms within the events can run in parallel.

### How to run with inter-event parallelism

The following components are used to achieve parallelism:

```python 
from Configurables import (HiveWhiteBoard, HiveSlimEventLoopMgr, AvalancheSchedulerSvc)
```

These 3 components need to be configued to adapt the level of parallelism to the sequence, algorithms and hardware to be used.

- Event Data Service: `HiveWhiteBoard`
  + *EventSlots*: Number of events that may run in parallel, each with its own EventStore
  + This is the Event Data Service, which needs the number of EventSlots
- Event Loop Manager: `HiveSlimEventLoopMgr`
  + Event Loop Manager with parallelism support
- Thread Scheduling config: `AvalancheSchedulerSvc`
  + Scheduler to indicate the number of threads to use
  + In needs the total number of threads to use: this determines how many events and algorithms can be in flight (run in parallel)
  + Default value is `-1` which indicate TBB to take over the machine with what it decides to be the optimal configuration


All these components can be set as follows in the options file:

```python
evtslots = 4
threads = 4

whiteboard = HiveWhiteBoard("EventDataSvc", EventSlots=evtslots)
slimeventloopmgr = HiveSlimEventLoopMgr(SchedulerName="AvalancheSchedulerSvc", OutputLevel=DEBUG)
scheduler = AvalancheSchedulerSvc(ThreadPoolSize=threads, OutputLevel=WARNING)

from Configurables import ApplicationMgr
ApplicationMgr( TopAlg = [seq],
                EvtSel = 'NONE',
                EvtMax = 10,
                ExtSvc = [whiteboard],
                EventLoop=slimeventloopmgr,
                MessageSvcType="InertMessageSvc"
              )
```

To only run events in parallel, with no parallelism between the algorithms within each event, the cardinality of the algorithms must be set:

- Given a list of algorithms, set the cardinality of all to be 1
- Create a `GaudiSequencer` with all the algorithms as `Members`
- Set the `GaudiSequencer` sequential property to true
- Pass the created `GaudiSequencer` to the Application Manager in the `TopAlg`: `TopAlg = [seq]`

```python
from Configurables import MarlinProcessorWrapper
from Configurables import (GaudiSequencer)

cardinality = 1

alg1 = MarlinProcessorWrapper("alg1")
alg2 = MarlinProcessorWrapper("alg2")
alg3 = MarlinProcessorWrapper("alg3")
alg4 = MarlinProcessorWrapper("alg4")

algList = []

algList.append(alg1)
algList.append(alg2)
algList.append(alg3)
algList.append(alg4)

for algo in algList:
    algo.Cardinality = cardinality
    algo.OutputLevel = DEBUG

seq = GaudiSequencer(
    "createViewSeq",
    Members=algList,
    Sequential=True,
    OutputLevel=VERBOSE)

from Configurables import ApplicationMgr
ApplicationMgr( TopAlg = [seq],
                EvtSel = 'NONE',
                EvtMax = 10,
                ExtSvc = [whiteboard],
                EventLoop=slimeventloopmgr,
                MessageSvcType="InertMessageSvc"
              )
```

## Running example

A multi-threaded CLIC Reconstruction can be run in multi-threaded mode, for LCIO input and output.
After successful compilation, from the build location:

```sh
# Check available tests
ctest -N

# Run multi-threaded clicReconstruction test
ctest -R clicRec_lcio_mt
```
