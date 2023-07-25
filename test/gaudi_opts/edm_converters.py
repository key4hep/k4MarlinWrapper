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
from Gaudi.Configuration import *

from Configurables import k4DataSvc, TestE4H2L, EDM4hep2LcioTool, Lcio2EDM4hepTool

algList = []

END_TAG = "END_TAG"

evtsvc = k4DataSvc("EventDataSvc")

# EDM4hep2lcio Tool
edmConvTool = EDM4hep2LcioTool("EDM4hep2lcio")
edmConvTool.convertAll = False
edmConvTool.collNameMapping = {
    "E4H_CaloHitCollection": "LCIO_CaloHitCollection",
    "E4H_RawCaloHitCollection": "LCIO_RawCaloHitCollection",
    "E4H_TPCHitCollection": "LCIO_TPCHitCollection",
    "E4H_TrackCollection": "LCIO_TrackCollection",
    "E4H_SimTrackerHitCollection": "LCIO_SimTrackerHitCollection",
    "E4H_TrackerHitCollection": "LCIO_TrackerHitCollection",
    "E4H_MCParticleCollection": "LCIO_MCParticleCollection",
    "E4H_SimCaloHitCollection": "LCIO_SimCaloHitCollection"
}

# LCIO2EDM4hep Tool
lcioConvTool = Lcio2EDM4hepTool("Lcio2EDM4hep")
lcioConvTool.convertAll = False
lcioConvTool.collNameMapping = {
    "LCIO_CaloHitCollection": "E4H_CaloHitCollection_conv",
    "LCIO_TrackerHitCollection": "E4H_TrackerHitCollection_conv",
    "LCIO_SimTrackerHitCollection": "E4H_SimTrackerHitCollection_conv",
    "LCIO_TrackCollection": "E4H_TrackCollection_conv",
    "LCIO_MCParticleCollection": "E4H_MCParticleCollection_conv",
    "LCIO_SimCaloHitCollection": "E4H_SimCaloHitCollection_conv"
}

TestConversion = TestE4H2L("TestConversion")
TestConversion.EDM4hep2LcioTool=edmConvTool
TestConversion.Lcio2EDM4hepTool=lcioConvTool

# Output_DST = MarlinProcessorWrapper("Output_DST")
# Output_DST.OutputLevel = WARNING
# Output_DST.ProcessorType = "LCIOOutputProcessor"
# Output_DST.Parameters = [
#                          "DropCollectionNames", END_TAG,
#                          "DropCollectionTypes", "MCParticle", "LCRelation", "SimCalorimeterHit", "CalorimeterHit", "SimTrackerHit", "TrackerHit", "TrackerHitPlane", "Track", "ReconstructedParticle", "LCFloatVec", "Clusters", END_TAG,
#                          "FullSubsetCollections", "EfficientMCParticles", "InefficientMCParticles", "MCPhysicsParticles", END_TAG,
#                          "KeepCollectionNames", "MCParticlesSkimmed", "MCPhysicsParticles", "RecoMCTruthLink", "SiTracks", "SiTracks_Refitted", "PandoraClusters", "PandoraPFOs", "SelectedPandoraPFOs", "LooseSelectedPandoraPFOs", "TightSelectedPandoraPFOs", "LE_SelectedPandoraPFOs", "LE_LooseSelectedPandoraPFOs", "LE_TightSelectedPandoraPFOs", "LumiCalClusters", "LumiCalRecoParticles", "BeamCalClusters", "BeamCalRecoParticles", "MergedRecoParticles", "MergedClusters", "RefinedVertexJets", "RefinedVertexJets_rel", "RefinedVertexJets_vtx", "RefinedVertexJets_vtx_RP", "BuildUpVertices", "BuildUpVertices_res", "BuildUpVertices_RP", "BuildUpVertices_res_RP", "BuildUpVertices_V0", "BuildUpVertices_V0_res", "BuildUpVertices_V0_RP", "BuildUpVertices_V0_res_RP", "PrimaryVertices", "PrimaryVertices_res", "PrimaryVertices_RP", "PrimaryVertices_res_RP", "RefinedVertices", "RefinedVertices_RP", "PFOsFromJets", END_TAG,
#                          "LCIOOutputFile", "Output_DST.slcio", END_TAG,
#                          "LCIOWriteMode", "WRITE_NEW", END_TAG
#                          ]


# from Configurables import PodioOutput
# out = PodioOutput("PodioOutput", filename = "output_k4SimDelphes.root")
# out.outputCommands = ["keep *"]


algList.append(TestConversion)
# algList.append(Output_DST)
# algList.append(out)

from Configurables import ApplicationMgr
ApplicationMgr( TopAlg = algList,
                EvtSel = 'NONE',
                EvtMax = 1,
                ExtSvc = [evtsvc],
                OutputLevel=DEBUG
)
