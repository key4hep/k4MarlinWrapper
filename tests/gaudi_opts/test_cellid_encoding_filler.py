#!/usr/bin/env python3
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

from Gaudi.Configuration import INFO

from Configurables import EventDataSvc, MetadataSvc
from Configurables import CellIDEncodingFiller, CellIDEncodingChecker

from k4FWCore import ApplicationMgr

TEST_ENCODINGS = {
    "SiVertexBarrelCollection": "system:5,side:0-0,layer:1-4,module:1-8,sensor:0-3",
    "SiTrackerBarrelCollection": "system:5,side:0-0,layer:1-5,module:1-12,sensor:0-1",
}

filler = CellIDEncodingFiller("CellIDEncodingFiller")
filler.CellIDEncodings = TEST_ENCODINGS

checker = CellIDEncodingChecker("CellIDEncodingChecker")
checker.ExpectedEncodings = TEST_ENCODINGS

ApplicationMgr(
    TopAlg=[filler, checker],
    EvtSel="NONE",
    EvtMax=1,
    ExtSvc=[EventDataSvc(), MetadataSvc()],
    OutputLevel=INFO,
)
