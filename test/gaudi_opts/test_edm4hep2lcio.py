from Gaudi.Configuration import *

from Configurables import k4DataSvc, MarlinProcessorWrapper, EDM4hep2LcioTool, k4LCIOReaderWrapper

algList = []

END_TAG = "END_TAG"

# theFile= 'edminput.root'
theFile = '/eos/experiment/fcc/ee/generation/DelphesEvents/fcc_tmp/p8_ee_Ztautau_ecm91_EvtGen_Tau2MuGamma/events_001720714.root'
evtsvc = k4DataSvc('EventDataSvc')
evtsvc.input = theFile

from Configurables import PodioInput
inp = PodioInput('InputReader')
inp.collections = [
    'ParticleIDs',
    'ReconstructedParticles',
    'EFlowTrack'
]
inp.OutputLevel = DEBUG


# EDM4hep2lcio Tool
edmConvTool = EDM4hep2LcioTool("EDM4hep2lcio")
edmConvTool.EDM2LCIOConversion = [
    "Track", "EFlowTrack", "LCIOCollectionName2",
    "ReconstructedParticle", "ReconstructedParticles", "TightSelectedPandoraPFOs"
]

# LCIO2EDM4hep Tool
lcioConvTool = k4LCIOReaderWrapper("LCIO2EDM4hep")
lcioConvTool.LCIO2EMD4hepConversion = [
    "Track", "LCIOCollectionName2", "MYEFlowTrack1",
    "ReconstructedParticle", "TightSelectedPandoraPFOs", "MYReconstructedParticles1"
]
lcioConvTool.OutputLevel = DEBUG

MyFastJetProcessor = MarlinProcessorWrapper("MyFastJetProcessor")
MyFastJetProcessor.OutputLevel = WARNING
MyFastJetProcessor.ProcessorType = "FastJetProcessor"
MyFastJetProcessor.Parameters = [
                                 "algorithm", "ValenciaPlugin", "1.2", "1.0", "0.7", END_TAG,
                                 "clusteringMode", "ExclusiveNJets", "2", END_TAG,
                                 "jetOut", "JetsAfterGamGamRemoval", END_TAG,
                                 "recParticleIn", "TightSelectedPandoraPFOs", END_TAG,
                                 "recParticleOut", "PFOsFromJets", END_TAG,
                                 "recombinationScheme", "E_scheme", END_TAG,
                                 "storeParticlesInJets", "true", END_TAG
                                 ]
MyFastJetProcessor.EDMConversionTool=edmConvTool
MyFastJetProcessor.LCIOConversionTool=lcioConvTool


Output_DST = MarlinProcessorWrapper("Output_DST")
Output_DST.OutputLevel = WARNING
Output_DST.ProcessorType = "LCIOOutputProcessor"
Output_DST.Parameters = [
                         "DropCollectionNames", END_TAG,
                         "DropCollectionTypes", "MCParticle", "LCRelation", "SimCalorimeterHit", "CalorimeterHit", "SimTrackerHit", "TrackerHit", "TrackerHitPlane", "Track", "ReconstructedParticle", "LCFloatVec", "Clusters", END_TAG,
                         "FullSubsetCollections", "EfficientMCParticles", "InefficientMCParticles", "MCPhysicsParticles", END_TAG,
                         "KeepCollectionNames", "MCParticlesSkimmed", "MCPhysicsParticles", "RecoMCTruthLink", "SiTracks", "SiTracks_Refitted", "PandoraClusters", "PandoraPFOs", "SelectedPandoraPFOs", "LooseSelectedPandoraPFOs", "TightSelectedPandoraPFOs", "LE_SelectedPandoraPFOs", "LE_LooseSelectedPandoraPFOs", "LE_TightSelectedPandoraPFOs", "LumiCalClusters", "LumiCalRecoParticles", "BeamCalClusters", "BeamCalRecoParticles", "MergedRecoParticles", "MergedClusters", "RefinedVertexJets", "RefinedVertexJets_rel", "RefinedVertexJets_vtx", "RefinedVertexJets_vtx_RP", "BuildUpVertices", "BuildUpVertices_res", "BuildUpVertices_RP", "BuildUpVertices_res_RP", "BuildUpVertices_V0", "BuildUpVertices_V0_res", "BuildUpVertices_V0_RP", "BuildUpVertices_V0_res_RP", "PrimaryVertices", "PrimaryVertices_res", "PrimaryVertices_RP", "PrimaryVertices_res_RP", "RefinedVertices", "RefinedVertices_RP", "PFOsFromJets", END_TAG,
                         "LCIOOutputFile", "Output_DST.slcio", END_TAG,
                         "LCIOWriteMode", "WRITE_NEW", END_TAG
                         ]


from Configurables import PodioOutput
out = PodioOutput("PodioOutput", filename = "output_k4SimDelphes.root")
out.outputCommands = ["keep *"]

# ApplicationMgr().TopAlg += [out]

algList.append(inp)
algList.append(MyFastJetProcessor)
# algList.append(Output_DST)
algList.append(out)

from Configurables import ApplicationMgr
ApplicationMgr( TopAlg = algList,
                EvtSel = 'NONE',
                EvtMax   = 5,
                ExtSvc = [evtsvc],
                OutputLevel=DEBUG
)
