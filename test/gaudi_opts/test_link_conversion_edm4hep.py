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
    PseudoRecoAlgorithm,
    PseudoRecoFunctional,
    TrivialMCRecoLinker,
    MarlinProcessorWrapper,
    EDM4hep2LcioTool,
    EventDataSvc,
)

from k4FWCore import ApplicationMgr, IOSvc

from k4FWCore.parseArgs import parser

parser.add_argument("--inputfile", help="Input file")
parser.add_argument(
    "--iosvc", action="store_true", default=False, help="Use IOSvc instead of PodioDataSvc"
)
parser.add_argument(
    "--use-gaudi-algorithm",
    action="store_true",
    default=False,
    help="Use an algorithm based on Gaudi::Algorithm instead of a functional algorithm",
)
args = parser.parse_known_args()[0]

if args.iosvc:
    evtsvc = EventDataSvc("EventDataSvc")
    iosvc = IOSvc()
    iosvc.Input = args.inputfile
    iosvc.CollectionNames = ["EventHeader", "MCParticles"]
else:
    evtsvc = k4DataSvc("EventDataSvc")
    evtsvc.input = args.inputfile
    podioInput = PodioInput("InputReader")
    podioInput.collections = ["EventHeader", "MCParticles"]
    podioInput.OutputLevel = INFO


if args.use_gaudi_algorithm:
    PseudoRecoAlg = PseudoRecoAlgorithm(
        "PseudoRecoAlgorithm", InputMCs="MCParticles", OutputRecos="PseudoRecoParticles"
    )
else:
    PseudoRecoAlg = PseudoRecoFunctional(
        "PseudoRecoFunctional", InputMCs=["MCParticles"], OutputRecos=["PseudoRecoParticles"]
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
mcLinkConverter.convertAll = True
mcLinkConverter.collNameMapping = {
    "TrivialMCRecoLinks": "TrivialMCRecoLinks",
    "MCParticles": "MCParticles",
    "PseudoRecoParticles": "PseudoRecoParticles",
}
mcLinkConverter.OutputLevel = DEBUG
MarlinMCLinkChecker.EDM4hep2LcioTool = mcLinkConverter

algList = [PseudoRecoAlg, MCRecoLinker, MarlinMCLinkChecker]

if not args.iosvc:
    algList = [podioInput] + algList

ApplicationMgr(
    TopAlg=algList,
    ExtSvc=[evtsvc],
    EvtMax=-1,
    EvtSel="NONE",
    OutputLevel=INFO,
)
