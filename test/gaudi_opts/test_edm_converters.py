from Gaudi.Configuration import *

from Configurables import k4DataSvc, TestE4H2L, EDM4hep2LcioTool, Lcio2EDM4hepTool

algList = []

END_TAG = "END_TAG"


# EDM4hep2lcio Tool
edmConvTool = EDM4hep2LcioTool("EDM4hep2lcio")
edmConvTool.Parameters = [
    "CalorimeterHit", "E4H_CaloHitCollection", "LCIO_CaloHitCollection",
    "ParticleID", "E4H_ParticleIDCollection", "LCIO_ParticleIDCollection"
]

# LCIO2EDM4hep Tool
lcioConvTool = Lcio2EDM4hepTool("Lcio2EDM4hep")
lcioConvTool.Parameters = [
    "CalorimeterHit", "LCIO_CaloHitCollection", "E4H_CaloHitCollection_conv",
    "ParticleID", "LCIO_ParticleIDCollection", "E4H_ParticleIDCollection_conv"
]

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
                OutputLevel=DEBUG
)