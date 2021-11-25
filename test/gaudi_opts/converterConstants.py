from Gaudi.Configuration import *

from Configurables import LcioEvent, EventDataSvc, MarlinProcessorWrapper
from k4MarlinWrapper.parseConstants import *
algList = []
evtsvc = EventDataSvc()


CONSTANTS = {
             'DetectorModel': "CLIC_o2_v04",
             'MyCompact': "compact",
             'DD4hepXMLFile_subPath': "/cvmfs/sw.hsf.org/spackages/linux-centos7-x86_64/gcc-8.3.0/lcgeo-0.16.6-vbidketl5esynx6pmkrdnqrnfyf3m2vf/share/lcgeo/compact/CLIC/%(MyCompact)s/%(DetectorModel)s/%(DetectorModel)s.xml",
             'MyResTest': "0.003",
             'MyResTest2': "003",
             'MyResTest3': "0.",
             'MyResArrayTest': ["0.002", "%(MyResTest)s", "0.004", "0.005"],
}

parseConstants(CONSTANTS)

read = LcioEvent()
read.OutputLevel = DEBUG
read.Files = ["$TEST_DIR/inputFiles/muons.slcio"]
algList.append(read)

AidaProcessor = MarlinProcessorWrapper("AidaProcessor")
AidaProcessor.OutputLevel = DEBUG 
AidaProcessor.ProcessorType = "AIDAProcessor" 
AidaProcessor.Parameters = {
                            "Compress": ["1"],
                            "FileName": ["histograms"],
                            "FileType": ["root"]
                            }

EventNumber = MarlinProcessorWrapper("EventNumber")
EventNumber.OutputLevel = DEBUG 
EventNumber.ProcessorType = "Statusmonitor" 
EventNumber.Parameters = {
                          "HowOften": ["1"]
                          }

InitDD4hep = MarlinProcessorWrapper("InitDD4hep")
InitDD4hep.OutputLevel = DEBUG 
InitDD4hep.ProcessorType = "InitializeDD4hep" 
InitDD4hep.Parameters = {
                         "DD4hepXMLFile": ["%(DD4hepXMLFile_subPath)s" % CONSTANTS]
                         }

VXDBarrelDigitiser = MarlinProcessorWrapper("VXDBarrelDigitiser")
VXDBarrelDigitiser.OutputLevel = DEBUG 
VXDBarrelDigitiser.ProcessorType = "DDPlanarDigiProcessor" 
VXDBarrelDigitiser.Parameters = {
                                 "IsStrip": ["false"],
                                 "ResolutionU": ["%(MyResTest)s" % CONSTANTS, "0.%(MyResTest2)s" % CONSTANTS, "0.003", "%(MyResTest3)s%(MyResTest2)s" % CONSTANTS, "%(MyResTest)s" % CONSTANTS, "%(MyResTest)s" % CONSTANTS],
                                 "ResolutionV": ["0.003", "%(MyResArrayTest)s" % CONSTANTS, "0.003"],
                                 "SimTrackHitCollectionName": ["VertexBarrelCollection"],
                                 "SimTrkHitRelCollection": ["VXDTrackerHitRelations"],
                                 "SubDetectorName": ["Vertex"],
                                 "TrackerHitCollectionName": ["VXDTrackerHits"]
                                 }

algList.append(AidaProcessor)
algList.append(EventNumber)
algList.append(InitDD4hep)
algList.append(VXDBarrelDigitiser)

from Configurables import ApplicationMgr
ApplicationMgr( TopAlg = algList,
                EvtSel = 'NONE',
                EvtMax   = 10,
                ExtSvc = [evtsvc],
                OutputLevel=DEBUG
              )
