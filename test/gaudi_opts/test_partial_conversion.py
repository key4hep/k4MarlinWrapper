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

from Gaudi.Configurables import (
    MarlinProcessorWrapper,
    EventDataSvc,
    EDM4hep2LcioTool,
)
from Gaudi.Configuration import DEBUG

from k4FWCore import ApplicationMgr, IOSvc
from k4FWCore.parseArgs import parser

from k4MarlinWrapper.io_helpers import IOHandlerHelper

parser.add_argument("--inputfile", help="The name of the input file")

args = parser.parse_known_args()[0]

algList = []
evtsvc = EventDataSvc("EventDataSvc")
iosvc = IOSvc()

io_handler = IOHandlerHelper(algList, iosvc)

io_handler.add_reader([args.inputfile])

wrapped_alg = MarlinProcessorWrapper(
    "PseudoReco",
    ProcessorType="PseudoRecoProcessor",
    Parameters={"InputMCs": ["MCParticle"], "OutputRecos": ["PseudoRecoParticles"]},
    OutputLevel=DEBUG,
)
algList.append(wrapped_alg)

input_conv = EDM4hep2LcioTool("InputConversion")
input_conv.convertAll = False
input_conv.collNameMapping = {"MCParticles": "MCParticle"}
wrapped_alg.EDM4hep2LcioTool = input_conv

ApplicationMgr(TopAlg=algList, EvtSel="NONE", EvtMax=1, ExtSvc=[evtsvc], OutputLevel=DEBUG)
