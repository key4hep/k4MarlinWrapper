#
# Copyright (c) 2019-2023 Key4hep-Project.
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
import os

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
                    "DD4hepXMLFile": [os.path.join(os.environ["LCGEO"], "CLIC/compact/CLIC_o2_v04/CLIC_o2_v04.xml")],
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
