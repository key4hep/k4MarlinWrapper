
from Gaudi.Configuration import *

from Configurables import LcioEvent, EventDataSvc, MarlinProcessorWrapper
algList = []
evtsvc = EventDataSvc()

read = LcioEvent()
read.OutputLevel = DEBUG
read.Files = ["../test/inputFiles/muons.slcio"]
algList.append(read)

END_TAG = "END_TAG"

# assign parameters by hand, in future parse Marlin.xml file in python
# and convert to list of processors and parameters
procA = MarlinProcessorWrapper("AidaProcessor")
procA.OutputLevel = DEBUG
procA.ProcessorType = "AIDAProcessor"
procA.Parameters = {"FileName": ["histograms"],
                    "FileType": ["root"],
                    "Compress": ["1"],
                    "Verbosity": ["DEBUG"],
                    }
algList.append(procA)


proc0 = MarlinProcessorWrapper("EventNumber")
proc0.OutputLevel = DEBUG
proc0.ProcessorType = "Statusmonitor"
proc0.Parameters = {"HowOften": ["1"],
                    "Verbosity": ["DEBUG"],
                    }
algList.append(proc0)


proc1 = MarlinProcessorWrapper("InitDD4hep")
proc1.OutputLevel = DEBUG
proc1.ProcessorType = "InitializeDD4hep"
proc1.Parameters = {#"EncodingStringParameter", "GlobalTrackerReadoutID"},
                    #"DD4hepXMLFile", "/cvmfs/clicdp.cern.ch/iLCSoft/builds/nightly/x86_64-slc6-gcc62-opt/lcgeo/HEAD/CLIC/compact/CLIC_o3_v13/CLIC_o3_v13.xml"},
                    "DD4hepXMLFile": ["/cvmfs/clicdp.cern.ch/iLCSoft/builds/nightly/x86_64-slc6-gcc62-opt/lcgeo/HEAD/CLIC/compact/CLIC_o2_v04/CLIC_o2_v04.xml"],
                    }
algList.append(proc1)


digiVxd = MarlinProcessorWrapper("VXDBarrelDigitiser")
digiVxd.OutputLevel = DEBUG
digiVxd.ProcessorType = "DDPlanarDigiProcessor"
digiVxd.Parameters = {"SubDetectorName": ["Vertex"],
                      "IsStrip": ["false"],
                      "ResolutionU": ["0.003", "0.003", "0.003", "0.003", "0.003", "0.003"],
                      "ResolutionV": ["0.003", "0.003", "0.003", "0.003", "0.003", "0.003"],
                      "SimTrackHitCollectionName": ["VertexBarrelCollection"],
                      "SimTrkHitRelCollection": ["VXDTrackerHitRelations"],
                      "TrackerHitCollectionName": ["VXDTrackerHits"],
                      "Verbosity": ["DEBUG"],
                      }
algList.append(digiVxd)


from Configurables import ApplicationMgr
ApplicationMgr( TopAlg = algList,
                EvtSel = 'NONE',
                EvtMax   = 10,
                ExtSvc = [evtsvc],
                OutputLevel=DEBUG
)
