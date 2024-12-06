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
import os

from Gaudi.Configuration import *

from Configurables import LcioEvent, EventDataSvc, MarlinProcessorWrapper

algList = []
evtsvc = EventDataSvc()

read = LcioEvent()
read.OutputLevel = DEBUG
read.Files = ["$TEST_DIR/inputFiles/muons.slcio"]
algList.append(read)

Output_DST = MarlinProcessorWrapper("Output_DST")
Output_DST.OutputLevel = WARNING
Output_DST.ProcessorType = "LCIOOutputProcessor"
Output_DST.Parameters = {
    "DropCollectionNames": [],
    "DropCollectionTypes": [
        "MCParticle",
        "LCRelation",
        "SimCalorimeterHit",
        "CalorimeterHit",
        "SimTrackerHit",
        "TrackerHit",
        "TrackerHitPlane",
        "Track",
        "ReconstructedParticle",
        "LCFloatVec",
        "Clusters",
    ],
    "FullSubsetCollections": [
        "EfficientMCParticles",
        "InefficientMCParticles",
        "MCPhysicsParticles",
    ],
    "KeepCollectionNames": [
        "MCParticlesSkimmed",
        "MCPhysicsParticles",
        "RecoMCTruthLink",
        "SiTracks",
        "SiTracks_Refitted",
        "PandoraClusters",
        "PandoraPFOs",
        "SelectedPandoraPFOs",
        "LooseSelectedPandoraPFOs",
        "TightSelectedPandoraPFOs",
        "LE_SelectedPandoraPFOs",
        "LE_LooseSelectedPandoraPFOs",
        "LE_TightSelectedPandoraPFOs",
        "LumiCalClusters",
        "LumiCalRecoParticles",
        "BeamCalClusters",
        "BeamCalRecoParticles",
        "MergedRecoParticles",
        "MergedClusters",
        "RefinedVertexJets",
        "RefinedVertexJets_rel",
        "RefinedVertexJets_vtx",
        "RefinedVertexJets_vtx_RP",
        "BuildUpVertices",
        "BuildUpVertices_res",
        "BuildUpVertices_RP",
        "BuildUpVertices_res_RP",
        "BuildUpVertices_V0",
        "BuildUpVertices_V0_res",
        "BuildUpVertices_V0_RP",
        "BuildUpVertices_V0_res_RP",
        "PrimaryVertices",
        "PrimaryVertices_res",
        "PrimaryVertices_RP",
        "PrimaryVertices_res_RP",
        "RefinedVertices",
        "RefinedVertices_RP",
    ],
    "LCIOOutputFile": ["Output_DST.slcio"],
    "LCIOWriteMode": ["WRITE_NEW"],
}

algList.append(Output_DST)

from Configurables import ApplicationMgr

ApplicationMgr(TopAlg=algList, EvtSel="NONE", EvtMax=10, ExtSvc=[evtsvc], OutputLevel=DEBUG)
