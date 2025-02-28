<!--
Copyright (c) 2019-2024 Key4hep-Project.

This file is part of Key4hep.
See https://key4hep.github.io/key4hep-doc/ for further info.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
-->
# k4MarlinWrapper tests

The test cases for the k4MarlinWrapper are somewhat involved in some cases.
Hence, we try to describe their intent and their high level layout a bit here.
We will be referring to the test cases by name as they are defined in the
[`CMakeLists.txt`](./CMakeLists.txt) file in here.

## `global_converter_maps`

The main purpose of this test is to make sure that the EDM conversions work as
expected, even if the event is converted piece-by-piece. In order to make that
work the individual converter tool instances have to share a mapping of all
corresponding LCIO and EDM4hep objects that have been converted (in either
direction). This map is referred to as the *global object map*.

### Setup

The test is using the
[`global_converter_maps.sh`](./scripts/global_converter_maps.sh) script which
effectively simply runs the
[`test_global_converter_maps.py`](./gaudi_opts/test_global_converter_maps.py)
options file after downloading some input data. This options file uses one
wrapped MarlinProcessor, one GaudiAlgorithm and one Gaudi Functional algorithm
plus some converters in-between them:
- `PodioInput` to read the *MCParticles* collection from the input file (in
  EDM4hep format)
- [`PseudoRecoFunctional`](./src/PseudoRecoFunctional.cc) creates a reco
  particle for every MC particle in the input collection (`PseudoRecoAlgorithm`
  does the same but implementing a `Gaudi::Algorithm` instead of a functional
  algorithm
  - An EDM4hep to LCIO converter converts the input MC particles and the
    reconstructed particles that are created by the algorithm
- [`TrivalMCTruthLinkerProcessor`](./src/TrivialMCTruthLinkerProcessor.cc)
  creates trivial links from the MC particles to the reco particles.
  - An LCIO to EDM4hep converter converts the output collection to EDM4hep
- [`MCRecoLinkChecker`](./src/MCRecoLinkChecker.cc) is a Gaudi algorithm that
  simply checks whether the MC-reco links are as expected.

Without the global converter map, the second LCIO to EDM4hep converter that is
run after the MC - reco linker will not produce valid MC - reco links, as it
does not have any knowledge about the input MCs and recos.
