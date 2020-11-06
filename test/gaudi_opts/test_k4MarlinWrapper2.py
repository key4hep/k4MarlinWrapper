
from Gaudi.Configuration import *

from Configurables import LcioEvent, EventDataSvc, MarlinProcessorWrapper
algList = []
evtsvc = EventDataSvc()

read = LcioEvent()
read.OutputLevel = DEBUG
read.Files = ["$k4MarlinWrapper_tests_DIR/inputFiles/testSimulation.slcio"]
algList.append(read)

END_TAG = "END_TAG"

# assign parameters by hand, in future parse Marlin.xml file in python
# and convert to list of processors and parameters
procA = MarlinProcessorWrapper("AidaProcessor")
procA.OutputLevel = DEBUG
procA.ProcessorType = "AIDAProcessor"
procA.Parameters = ["FileName", "histograms", END_TAG,
                    "FileType", "root", END_TAG,
                    "Compress", "1", END_TAG,
                    "Verbosity", "DEBUG", END_TAG,
                    ]
algList.append(procA)


proc0 = MarlinProcessorWrapper("EventNumber")
proc0.OutputLevel = DEBUG
proc0.ProcessorType = "Statusmonitor"
proc0.Parameters = ["HowOften", "1", END_TAG,
                    "Verbosity", "DEBUG", END_TAG,
                    ]
algList.append(proc0)


proc1 = MarlinProcessorWrapper("InitDD4hep")
proc1.OutputLevel = DEBUG
proc1.ProcessorType = "InitializeDD4hep"
proc1.Parameters = [#"EncodingStringParameter", "GlobalTrackerReadoutID", END_TAG,
                    #"DD4hepXMLFile", "/cvmfs/clicdp.cern.ch/iLCSoft/builds/nightly/x86_64-slc6-gcc62-opt/lcgeo/HEAD/CLIC/compact/CLIC_o3_v13/CLIC_o3_v13.xml", END_TAG,
                    "DD4hepXMLFile", "/cvmfs/clicdp.cern.ch/iLCSoft/builds/nightly/x86_64-slc6-gcc62-opt/lcgeo/HEAD/CLIC/compact/CLIC_o2_v04/CLIC_o2_v04.xml", END_TAG,
                    ]
algList.append(proc1)


digiVxd = MarlinProcessorWrapper("VXDBarrelDigitiser")
digiVxd.OutputLevel = DEBUG
digiVxd.ProcessorType = "DDPlanarDigiProcessor"
digiVxd.Parameters = [
    "SubDetectorName", "Vertex", END_TAG,
    "IsStrip", "false", END_TAG,
    "ResolutionU", "0.003", "0.003", "0.003", "0.003", "0.003", "0.003", END_TAG,
    "ResolutionV", "0.003", "0.003", "0.003", "0.003", "0.003", "0.003", END_TAG,
    "SimTrackHitCollectionName", "VertexBarrelCollection", END_TAG,
    "SimTrkHitRelCollection", "VXDTrackerHitRelations", END_TAG,
    "TrackerHitCollectionName", "VXDTrackerHits", END_TAG,
    "Verbosity" , "DEBUG", END_TAG,
                    ]
algList.append(digiVxd)

digiVxd2 = MarlinProcessorWrapper("VXDBarrelDigitiser2")
digiVxd2.OutputLevel = DEBUG
digiVxd2.ProcessorType = "DDPlanarDigiProcessor"
digiVxd2.Parameters = [
    "SubDetectorName", "Vertex", END_TAG,
    "IsStrip", "false", END_TAG,
    "ResolutionU", "0.002", "0.002", "0.002", "0.002", "0.002", "0.002", END_TAG,
    "ResolutionV", "0.001", "0.001", "0.001", "0.001", "0.001", "0.001", END_TAG,
    "SimTrackHitCollectionName", "VertexBarrelCollection2", END_TAG,
    "SimTrkHitRelCollection", "VXDTrackerHitRelations2", END_TAG,
    "TrackerHitCollectionName", "VXDTrackerHits2", END_TAG,
    "Verbosity" , "DEBUG", END_TAG,
                    ]
algList.append(digiVxd2)

from Configurables import ApplicationMgr
ApplicationMgr( TopAlg = algList,
                EvtSel = 'NONE',
                EvtMax   = 4,
                ExtSvc = [evtsvc],
                OutputLevel=DEBUG
)
