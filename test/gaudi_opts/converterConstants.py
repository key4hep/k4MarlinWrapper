#
# Copyright (c) 2019-2024 Key4hep-Project.
#
# This file is part of Key4hep.
# See https://key4hep.github.io/key4hep-doc/ for further info.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
from Gaudi.Configuration import *

from Configurables import LcioEvent, EventDataSvc, MarlinProcessorWrapper
from k4MarlinWrapper.parseConstants import *

algList = []
evtsvc = EventDataSvc()


CONSTANTS = {
    "DetectorModel": "CLIC_o2_v04",
    "MyCompact": "compact",
    "DD4hepXMLFile_subPath": "$K4GEO/CLIC/%(MyCompact)s/%(DetectorModel)s/%(DetectorModel)s.xml",
    "MyResTest": "0.003",
    "MyResTest2": "003",
    "MyResTest3": "0.",
    "MyResArrayTest": ["0.002", "%(MyResTest)s", "0.004", "0.005"],
}

parseConstants(CONSTANTS)

read = LcioEvent()
read.OutputLevel = DEBUG
read.Files = ["$TEST_DIR/inputFiles/muons.slcio"]
algList.append(read)

AidaProcessor = MarlinProcessorWrapper("AidaProcessor")
AidaProcessor.OutputLevel = DEBUG
AidaProcessor.ProcessorType = "AIDAProcessor"
AidaProcessor.Parameters = {"Compress": ["1"], "FileName": ["histograms"], "FileType": ["root"]}

EventNumber = MarlinProcessorWrapper("EventNumber")
EventNumber.OutputLevel = DEBUG
EventNumber.ProcessorType = "Statusmonitor"
EventNumber.Parameters = {"HowOften": ["1"]}

InitDD4hep = MarlinProcessorWrapper("InitDD4hep")
InitDD4hep.OutputLevel = DEBUG
InitDD4hep.ProcessorType = "InitializeDD4hep"
InitDD4hep.Parameters = {"DD4hepXMLFile": ["%(DD4hepXMLFile_subPath)s" % CONSTANTS]}

VXDBarrelDigitiser = MarlinProcessorWrapper("VXDBarrelDigitiser")
VXDBarrelDigitiser.OutputLevel = DEBUG
VXDBarrelDigitiser.ProcessorType = "DDPlanarDigiProcessor"
VXDBarrelDigitiser.Parameters = {
    "IsStrip": ["false"],
    "ResolutionU": [
        "%(MyResTest)s" % CONSTANTS,
        "0.%(MyResTest2)s" % CONSTANTS,
        "0.003",
        "%(MyResTest3)s%(MyResTest2)s" % CONSTANTS,
        "%(MyResTest)s" % CONSTANTS,
        "%(MyResTest)s" % CONSTANTS,
    ],
    "ResolutionV": ["0.003", "%(MyResArrayTest)s" % CONSTANTS, "0.003"],
    "SimTrackHitCollectionName": ["VertexBarrelCollection"],
    "SimTrkHitRelCollection": ["VXDTrackerHitRelations"],
    "SubDetectorName": ["Vertex"],
    "TrackerHitCollectionName": ["VXDTrackerHits"],
}

algList.append(AidaProcessor)
algList.append(EventNumber)
algList.append(InitDD4hep)
algList.append(VXDBarrelDigitiser)

from Configurables import ApplicationMgr

ApplicationMgr(TopAlg=algList, EvtSel="NONE", EvtMax=10, ExtSvc=[evtsvc], OutputLevel=DEBUG)
