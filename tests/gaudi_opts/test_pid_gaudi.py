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

from Configurables import (
    PIDProducer,
    MarlinProcessorWrapper,
    EDM4hep2LcioTool,
    EventDataSvc,
    MetadataSvc,
)
from k4FWCore import ApplicationMgr, IOSvc

iosvc = IOSvc()

metadataSvc = MetadataSvc("MetadataSvc")

pidProd = PIDProducer(
    "PIDProducer",
    OutputRecoColl="RecoParticles",
    OutputFilledPIDColl="ParticleIDs",
    OutputPIDColl="EmptyParticleIDs",
    PIDAlgoName="testPIDAlgo",
    PIDParamNames=["param1", "param2"],
)

EDM4hep2Lcio = EDM4hep2LcioTool("EDM4hep2Lcio")
EDM4hep2Lcio.convertAll = True

out = MarlinProcessorWrapper("out")
out.ProcessorType = "LCIOOutputProcessor"
out.Parameters = {
    "LCIOOutputFile": ["test_pid.slcio"],
    "LCIOWriteMode": ["WRITE_NEW"],
}
out.EDM4hep2LcioTool = EDM4hep2Lcio

algList = [pidProd, out]

ApplicationMgr(
    TopAlg=algList,
    EvtSel="NONE",
    EvtMax=1,
    ExtSvc=[EventDataSvc(), metadataSvc],
    OutputLevel=INFO,
)
