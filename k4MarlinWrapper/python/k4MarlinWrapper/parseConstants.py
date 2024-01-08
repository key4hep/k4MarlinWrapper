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
import re

def parseConstants(constants):
  """Keeps looping over constants dictionary,
  replacing constants until no more are found
  """
  again = True
  while again:
    again = False
    for key, value in constants.items():
      if isinstance(value, list):
        for idx, val in enumerate(value):
          value[idx] = val % constants
          again = again or (value[idx] != val)
        constants[key] = " ".join(value)
      elif value:
        if (value):
          constants[key] = value % constants
          again = again or (constants[key] != value)
