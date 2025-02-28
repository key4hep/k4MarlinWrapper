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
    PodioOutput,
    MarlinProcessorWrapper,
    k4DataSvc,
    Lcio2EDM4hepTool,
    EDM4hep2LcioTool,
    MCRecoLinkChecker,
    PseudoRecoFunctional,
    EventDataSvc,
)

from k4FWCore import ApplicationMgr, IOSvc

from k4FWCore.parseArgs import parser

parser.add_argument(
    "--iosvc", action="store_true", default=False, help="Use IOSvc instead of PodioDataSvc"
)
parser.add_argument(
    "--use-functional-checker", action="store_true", default=False, help="Use functional checker"
)

args = parser.parse_known_args()[0]

if args.use_functional_checker:
    from Configurables import MCRecoLinkCheckerFunctional as MCRecoLinkChecker

if args.iosvc:
    evtsvc = EventDataSvc("EventDataSvc")
else:
    evtsvc = k4DataSvc("EventDataSvc")

if args.iosvc:
    iosvc = IOSvc()
    iosvc.CollectionNames = ["EventHeader", "MCParticles"]
    if not args.use_functional_checker:
        iosvc.Output = "global_converter_maps_iosvc.root"
    else:
        iosvc.Output = "global_converter_maps_iosvc_functional.root"
else:
    podioInput = PodioInput("InputReader")
    podioInput.collections = ["EventHeader", "MCParticles"]
    podioInput.OutputLevel = INFO
    podioOutput = PodioOutput("OutputWriter")
    podioOutput.filename = "global_converter_maps.root"

PseudoRecoAlg = PseudoRecoFunctional(
    "PseudoRecoFunctional", InputMCs=["MCParticles"], OutputRecos=["PseudoRecoParticles"]
)

inputConverter = EDM4hep2LcioTool("InputConverter")
inputConverter.convertAll = False
inputConverter.collNameMapping = {
    "MCParticles": "MCParticles",
    "PseudoRecoParticles": "PseudoRecoParticles",
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
mcLinkChecker.InputMCRecoLinks = (
    "TrivialMCRecoLinks" if not args.use_functional_checker else ["TrivialMCRecoLinks"]
)
mcLinkChecker.InputMCs = "MCParticles" if not args.use_functional_checker else ["MCParticles"]
mcLinkChecker.InputRecos = (
    "PseudoRecoParticles" if not args.use_functional_checker else ["PseudoRecoParticles"]
)
mcLinkChecker.OutputLevel = DEBUG

algList = [
    PseudoRecoAlg,
    TrivialMCTruthLinkerProc,
    mcLinkChecker,
]

if not args.iosvc:
    algList = [podioInput] + algList + [podioOutput]

ApplicationMgr(TopAlg=algList, EvtSel="NONE", EvtMax=1, ExtSvc=[evtsvc], OutputLevel=DEBUG)
