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
    k4DataSvc,
    ApplicationMgr,
    PseudoRecoAlgorithm,
    TrivialMCRecoLinker,
    MarlinProcessorWrapper,
    EDM4hep2LcioTool,
)

from k4FWCore.parseArgs import parser

parser.add_argument("--inputfile", help="Input file")
my_args = parser.parse_known_args()[0]

evtsvc = k4DataSvc("EventDataSvc")
evtsvc.input = my_args.inputfile

podioInput = PodioInput("InputReader")
podioInput.collections = ["MCParticles"]
podioInput.OutputLevel = INFO


PseudoRecoAlg = PseudoRecoAlgorithm(
    "PseudoRecoAlgorithm", InputMCs=["MCParticles"], OutputRecos=["PseudoRecoParticles"]
)

MCRecoLinker = TrivialMCRecoLinker(
    InputMCs=["MCParticles"],
    InputRecos=["PseudoRecoParticles"],
    OutputLinks=["TrivialMCRecoLinks"],
)


MarlinMCLinkChecker = MarlinProcessorWrapper(
    "MarlinMCRecoLinkChecker",
    ProcessorType="MarlinMCRecoLinkChecker",
    Parameters={
        "MCRecoLinks": ["TrivialMCRecoLinks"],
        "InputMCs": ["MCParticles"],
        "InputRecos": ["PseudoRecoParticles"],
    },
)

mcLinkConverter = EDM4hep2LcioTool("MCLinkConverterToEDM4hep")
mcLinkConverter.convertAll = False
mcLinkConverter.collNameMapping = {
    "TrivialMCRecoLinks": "TrivialMCRecoLinks",
    "MCParticles": "MCParticles",
    "PseudoRecoParticles": "PseudoRecoParticles",
}
mcLinkConverter.OutputLevel = DEBUG
MarlinMCLinkChecker.EDM4hep2LcioTool = mcLinkConverter


ApplicationMgr(
    TopAlg=[podioInput, PseudoRecoAlg, MCRecoLinker, MarlinMCLinkChecker],
    ExtSvc=[evtsvc],
    EvtMax=-1,
    EvtSel="NONE",
    OutputLevel=INFO,
)
