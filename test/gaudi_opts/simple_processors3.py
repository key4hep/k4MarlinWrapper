import os

from Gaudi.Configuration import *

from Configurables import LcioEvent, EventDataSvc, MarlinProcessorWrapper
algList = []
evtsvc = EventDataSvc()

read = LcioEvent()
read.OutputLevel = DEBUG
# read.Files = ["$TEST_DIR/inputFiles/testSimulation.slcio"]
read.Files = ["../test/inputFiles/testSimulation.slcio"]

algList.append(read)


from Configurables import HepRndm__Engine_CLHEP__RanluxEngine_ as RndmEngine
rndmengine = RndmEngine('RndmGenSvc.Engine')
rndmengine.SetSingleton = True
rndmengine.Seeds = [ 234567 ] # default seed is 1234567


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
proc1.Parameters = {#"EncodingStringParameter": ["GlobalTrackerReadoutID"],
                     "DD4hepXMLFile": [os.path.join(os.environ.get('LCGEO'), 'CLIC/compact/CLIC_o2_v04/CLIC_o2_v04.xml')]
                    }
algList.append(proc1)


digiVxd = MarlinProcessorWrapper("VXDBarrelDigitiser")
digiVxd.OutputLevel = DEBUG
digiVxd.ProcessorType = "DDPlanarDigiProcessor"
digiVxd.Parameters = {
    "SubDetectorName": ["Vertex"],
    "IsStrip": ["false"],
    "ResolutionU": ["0.003", "0.003", "0.003", "0.003", "0.003", "0.003"],
    "ResolutionV": ["0.003", "0.003", "0.003", "0.003", "0.003", "0.003"],
    "SimTrackHitCollectionName": ["VertexBarrelCollection"],
    "SimTrkHitRelCollection": ["VXDTrackerHitRelations"],
    "TrackerHitCollectionName": ["VXDTrackerHits"],
    "Verbosity": ["DEBUG"],
                    }
algList.append(digiVxd)

digiVxd2 = MarlinProcessorWrapper("VXDBarrelDigitiser2")
digiVxd2.OutputLevel = DEBUG
digiVxd2.ProcessorType = "DDPlanarDigiProcessor"
digiVxd2.Parameters = {
    "SubDetectorName": ["Vertex"],
    "IsStrip": ["false"],
    "ResolutionU": ["0.002", "0.002", "0.002", "0.002", "0.002", "0.002"],
    "ResolutionV": ["0.001", "0.001", "0.001", "0.001", "0.001", "0.001"],
    "SimTrackHitCollectionName": ["VertexBarrelCollection2"],
    "SimTrkHitRelCollection": ["VXDTrackerHitRelations2"],
    "TrackerHitCollectionName": ["VXDTrackerHits2"],
    "Verbosity": ["DEBUG"],
                    }
algList.append(digiVxd2)

from Configurables import ApplicationMgr
ApplicationMgr( TopAlg = algList,
                EvtSel = 'NONE',
                EvtMax   = 4,
                ExtSvc = [evtsvc],
                OutputLevel=DEBUG
)
