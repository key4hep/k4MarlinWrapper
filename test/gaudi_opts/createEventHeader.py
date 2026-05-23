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
    EventHeaderCreator,
    MarlinProcessorWrapper,
    EDM4hep2LcioTool,
    EventDataSvc,
)
from k4FWCore import ApplicationMgr, IOSvc
from k4FWCore.parseArgs import parser

parser.add_argument(
    "--no-event-header",
    help="Do not create EventHeaders",
    default=False,
    action="store_true",
)

args = parser.parse_known_args()[0]

iosvc = IOSvc()

eventHeaderCreator = EventHeaderCreator("eventHeaderCreator", eventNumberOffset=42)

EDM4hep2Lcio = EDM4hep2LcioTool("EDM4hep2Lcio")
EDM4hep2Lcio.convertAll = False

out = MarlinProcessorWrapper("out")
out.ProcessorType = "LCIOOutputProcessor"
out.Parameters = {
    "LCIOOutputFile": ["test.slcio" if not args.no_event_header else "test_without_header.slcio"],
    "LCIOWriteMode": ["WRITE_NEW"],
}
out.EDM4hep2LcioTool = EDM4hep2Lcio

if args.no_event_header:
    alg_list = [out]
else:
    alg_list = [eventHeaderCreator, out]

ApplicationMgr(
    TopAlg=alg_list,
    EvtSel="NONE",
    EvtMax=2,
    ExtSvc=[EventDataSvc()],
    OutputLevel=INFO,
)
