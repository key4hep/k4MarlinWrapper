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

import sys
from Configurables import LcioEvent, PodioInput

def create_reader(input_files, evtSvc):
    """Create the appropriate reader for the input files"""
    if input_files[0].endswith(".slcio"):
        if any(not f.endswith(".slcio") for f in input_files):
            print("All input files need to have the same format (LCIO)")
            sys.exit(1)

        read = LcioEvent()
        read.Files = input_files
    else:
        if any(not f.endswith(".root") for f in input_files):
            print("All input files need to have the same format (EDM4hep)")
            sys.exit(1)
        read = PodioInput("PodioInput")
        evtSvc.inputs = input_files

    return read
