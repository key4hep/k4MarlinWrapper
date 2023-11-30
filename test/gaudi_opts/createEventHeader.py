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
from Gaudi.Configuration import WARNING

from Configurables import (
    EventHeaderCreator,
    k4DataSvc,
    MarlinProcessorWrapper,
    ApplicationMgr,
    EDM4hep2LcioTool,
)

eventHeaderCreator = EventHeaderCreator("eventHeaderCreator", eventNumberOffset=42)

podioevent = k4DataSvc("EventDataSvc")

EDM4hep2Lcio = EDM4hep2LcioTool("EDM4hep2Lcio")
EDM4hep2Lcio.convertAll = False

out = MarlinProcessorWrapper("out")
out.ProcessorType = "LCIOOutputProcessor"
out.Parameters = {"LCIOOutputFile": ["test.slcio"], "LCIOWriteMode": ["WRITE_NEW"]}
out.EDM4hep2LcioTool = EDM4hep2Lcio

ApplicationMgr(
    TopAlg=[
        eventHeaderCreator,
        out,
    ],
    EvtSel="NONE",
    EvtMax=2,
    ExtSvc=[podioevent],
    OutputLevel=WARNING,
)
