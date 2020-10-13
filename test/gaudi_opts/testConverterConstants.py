from Gaudi.Configuration import *

from Configurables import LcioEvent, EventDataSvc, MarlinProcessorWrapper
from GMPWrapper.parseConstants import *
algList = []
evtsvc = EventDataSvc()

END_TAG = "END_TAG"


CONSTANTS = {
    'MyCompact': 'compact',
    'DD4hepXMLFile_subPath': '/cvmfs/clicdp.cern.ch/iLCSoft/builds/nightly/x86_64-slc6-gcc62-opt/lcgeo/HEAD/CLIC/%(MyCompact)s/%(DetectorModel)s/%(DetectorModel)s.xml',
    'DetectorModel': 'CLIC_o2_v04',
}

parseConstants(CONSTANTS)

read = LcioEvent()
read.OutputLevel = DEBUG
read.Files = ["$GMP_tests_DIR/inputFiles/muons.slcio"]
algList.append(read)

AidaProcessor = MarlinProcessorWrapper("AidaProcessor")
AidaProcessor.OutputLevel = DEBUG 
AidaProcessor.ProcessorType = "AIDAProcessor" 
AidaProcessor.Parameters = [
                            "Compress", "1", END_TAG,
                            "FileName", "histograms", END_TAG,
                            "FileType", "root", END_TAG
                            ]

EventNumber = MarlinProcessorWrapper("EventNumber")
EventNumber.OutputLevel = DEBUG 
EventNumber.ProcessorType = "Statusmonitor" 
EventNumber.Parameters = [
                          "HowOften", "1", END_TAG
                          ]

InitDD4hep = MarlinProcessorWrapper("InitDD4hep")
InitDD4hep.OutputLevel = DEBUG 
InitDD4hep.ProcessorType = "InitializeDD4hep" 
InitDD4hep.Parameters = [
                         "DD4hepXMLFile", "%(DD4hepXMLFile_subPath)s" % CONSTANTS, END_TAG
                         ]

VXDBarrelDigitiser = MarlinProcessorWrapper("VXDBarrelDigitiser")
VXDBarrelDigitiser.OutputLevel = DEBUG 
VXDBarrelDigitiser.ProcessorType = "DDPlanarDigiProcessor" 
VXDBarrelDigitiser.Parameters = [
                                 "IsStrip", "false", END_TAG,
                                 "ResolutionU", "0.003", "0.003", "0.003", "0.003", "0.003", "0.003", END_TAG,
                                 "ResolutionV", "0.003", "0.003", "0.003", "0.003", "0.003", "0.003", END_TAG,
                                 "SimTrackHitCollectionName", "VertexBarrelCollection", END_TAG,
                                 "SimTrkHitRelCollection", "VXDTrackerHitRelations", END_TAG,
                                 "SubDetectorName", "Vertex", END_TAG,
                                 "TrackerHitCollectionName", "VXDTrackerHits", END_TAG
                                 ]

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
