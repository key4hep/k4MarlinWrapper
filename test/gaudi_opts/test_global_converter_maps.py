#!/usr/bin/env python3
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

from Configurables import (
    PodioInput,
    MarlinProcessorWrapper,
    k4DataSvc,
    Lcio2EDM4hepTool,
    EDM4hep2LcioTool,
    MCRecoLinkChecker,
    ApplicationMgr,
)

evtsvc = k4DataSvc("EventDataSvc")
evtsvc.input = os.path.join(
    "$TEST_DIR/inputFiles/", "ttbar_podio230830_edm4hep_frame.root"
)

podioInput = PodioInput("InputReader")
podioInput.collections = ["MCParticles"]
podioInput.OutputLevel = INFO

PseudoRecoProc = MarlinProcessorWrapper("PseudoReco")
PseudoRecoProc.ProcessorType = "PseudoRecoProcessor"
PseudoRecoProc.Parameters = {
    "InputMCs": ["MCParticles"],
    "OutputRecos": ["PseudoRecoParticles"],
}

inputConverter = EDM4hep2LcioTool("InputConverter")
inputConverter.convertAll = False
inputConverter.collNameMapping = {"MCParticles": "MCParticles"}
inputConverter.OutputLevel = DEBUG

PseudoRecoProc.EDM4hep2LcioTool = inputConverter

pseudoRecConverter = Lcio2EDM4hepTool("PseudoRecoConverter")
pseudoRecConverter.convertAll = False
pseudoRecConverter.collNameMapping = {"PseudoRecoParticles": "PseudoRecoParticles"}
pseudoRecConverter.OutputLevel = DEBUG

PseudoRecoProc.Lcio2EDM4hepTool = pseudoRecConverter

TrivialMCTruthLinkerProc = MarlinProcessorWrapper("TvivialMCTruthLinker")
TrivialMCTruthLinkerProc.ProcessorType = "TrivialMCTruthLinkerProcessor"
TrivialMCTruthLinkerProc.Parameters = {
    "InputMCs": ["MCParticles"],
    "InputRecos": ["PseudoRecoParticles"],
    "OutputMCRecoLinks": ["TrivialMCRecoLinks"],
}

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
    PseudoRecoProc,
    TrivialMCTruthLinkerProc,
    mcLinkChecker,
]

ApplicationMgr(
    TopAlg=algList, EvtSel="NONE", EvtMax=3, ExtSvc=[evtsvc], OutputLevel=INFO
)
