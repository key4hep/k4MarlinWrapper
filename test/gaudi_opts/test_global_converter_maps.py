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


from Gaudi.Configuration import INFO, DEBUG

from Configurables import (
    PodioInput,
    MarlinProcessorWrapper,
    k4DataSvc,
    Lcio2EDM4hepTool,
    EDM4hep2LcioTool,
    MCRecoLinkChecker,
    ApplicationMgr,
    PseudoRecoAlgorithm,
)

evtsvc = k4DataSvc("EventDataSvc")

podioInput = PodioInput("InputReader")
podioInput.collections = ["MCParticles"]
podioInput.OutputLevel = INFO

PseudoRecoAlg = PseudoRecoAlgorithm("PseudoRecoAlgorithm",
                                     InputMCs=["MCParticles"],
                                     OutputRecos=["PseudoRecoParticles"])

inputConverter = EDM4hep2LcioTool("InputConverter")
inputConverter.convertAll = False
inputConverter.collNameMapping = {"MCParticles": "MCParticles",
                                  "PseudoRecoParticles": "PseudoRecoParticles"
                                  }
inputConverter.OutputLevel = DEBUG

TrivialMCTruthLinkerProc = MarlinProcessorWrapper("TrivialMCTruthLinker")
TrivialMCTruthLinkerProc.ProcessorType = "TrivialMCTruthLinkerProcessor"
TrivialMCTruthLinkerProc.Parameters = {
    "InputMCs": ["MCParticles"],
    "InputRecos": ["PseudoRecoParticles"],
    "OutputMCRecoLinks": ["TrivialMCRecoLinks"],
}

TrivialMCTruthLinkerProc.EDM4hep2LcioTool = inputConverter

mcTruthConverter = Lcio2EDM4hepTool("TrivialMCTruthLinkerConverter")
mcTruthConverter.convertAll = False
mcTruthConverter.collNameMapping = {"TrivialMCRecoLinks": "TrivialMCRecoLinks"}
mcTruthConverter.OutputLevel = DEBUG

TrivialMCTruthLinkerProc.Lcio2EDM4hepTool = mcTruthConverter

mcLinkChecker = MCRecoLinkChecker("MCRecoLinkChecker")
mcLinkChecker.InputMCRecoLinks = "TrivialMCRecoLinks"
mcLinkChecker.InputMCs = "MCParticles"
mcLinkChecker.InputRecos = "PseudoRecoParticles"
mcLinkChecker.OutputLevel = DEBUG

algList = [
    podioInput,
    PseudoRecoAlg,
    TrivialMCTruthLinkerProc,
    mcLinkChecker,
]

ApplicationMgr(
    TopAlg=algList, EvtSel="NONE", EvtMax=3, ExtSvc=[evtsvc], OutputLevel=DEBUG
)
