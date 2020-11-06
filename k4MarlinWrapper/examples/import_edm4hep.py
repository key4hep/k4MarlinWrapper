from Gaudi.Configuration import *

from Configurables import EventDataSvc, MarlinProcessorWrapper

# evtsvc = EventDataSvc()
algList = []

END_TAG = "END_TAG"

from Configurables import K4DataSvc
podioevent = K4DataSvc("EventDataSvc")
podioevent.input = "output_k4test_exampledata.root"

from Configurables import PodioInput
inp = PodioInput("InputReader")
inp.collections = ["MCParticles", "SimTrackerHit"]

algList.append(inp)

# assign parameters by hand, in future parse Marlin.xml file in python
# and convert to list of processors and parameters
procA = MarlinProcessorWrapper("TestProcessor")
procA.OutputLevel = DEBUG
procA.ProcessorType = "AIDAProcessor"
procA.Parameters = ["FileName", "histograms", END_TAG,
                    "FileType", "root", END_TAG,
                    "Compress", "1", END_TAG,
                    "Verbosity", "DEBUG", END_TAG,
                    ]
procA.Input = "/home/plfernan/workspace/k4MarlinWrapper_test/scripts_test/edm4hep_input/events_001720714.root"
algList.append(procA)

from Configurables import ApplicationMgr
ApplicationMgr( TopAlg=algList,
                EvtSel="NONE",
                EvtMax=10,
                ExtSvc=[podioevent],
                OutputLevel=DEBUG,
                )