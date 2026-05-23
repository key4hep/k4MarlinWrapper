#!/usr/bin/env bash
##
## Copyright (c) 2019-2024 Key4hep-Project.
##
## This file is part of Key4hep.
## See https://key4hep.github.io/key4hep-doc/ for further info.
##
## Licensed under the Apache License, Version 2.0 (the "License");
## you may not use this file except in compliance with the License.
## You may obtain a copy of the License at
##
##     http://www.apache.org/licenses/LICENSE-2.0
##
## Unless required by applicable law or agreed to in writing, software
## distributed under the License is distributed on an "AS IS" BASIS,
## WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
## See the License for the specific language governing permissions and
## limitations under the License.
##

# This script confirms that the input file has only two LCIO collections which
# are named MCParticle and PseudoRecoParticles respectively.

set -e

# The following command extracts the collection names from the table that is
# printed by anajob using the first awk invocation. The rest is then simply
# turning the list of collection names into a colon separated list which we can
# then compare against our expectations
collections=$(anajob ${1} | awk '/COLLECTION NAME/ {table=1; next} /^======/ {next} /^------/ {table=0} table' | awk '{print $1}' | tr '\n' ':')

if [ ${collections} != "MCParticle:PseudoRecoParticles:" ]; then
    echo "Expected collections MCParticle:PseudoRecoParticles: but got ${collections}"
    exit 1
fi
