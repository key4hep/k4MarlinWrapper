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

from Gaudi.Configuration import DEBUG
from Configurables import EventDataSvc
from k4FWCore import IOSvc, ApplicationMgr
from k4FWCore.parseArgs import parser
from k4MarlinWrapper.io_helpers import IOHandlerHelper

parser.add_argument("--inputfile", help="The name of the input file")

args = parser.parse_known_args()[0]

io_svc = IOSvc()

alg_list = []
io_handler = IOHandlerHelper(alg_list, io_svc)
io_handler.add_reader([args.inputfile])

io_handler.finalize_converters()
ApplicationMgr(
    TopAlg=alg_list,
    EvtSel="NONE",
    EvtMax=3,
    ExtSvc=[EventDataSvc()],
    OutputLevel=DEBUG,
)
