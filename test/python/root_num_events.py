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

#########################################################
# Print the number of events of a ROOT file that has an "events" branch,
# as in edm4hep files
#########################################################

import ROOT
import sys

f = ROOT.TFile.Open(sys.argv[1], "READ")
tree = f.Get("events")
print(tree.GetEntries())
