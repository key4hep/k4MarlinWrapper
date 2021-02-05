from Gaudi.Configuration import *

from Configurables import k4DataSvc, ToolSvc, MarlinProcessorWrapper, EDM4hep2LcioTool

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
algList.append(inp)

ToolSvc.LogLevel = DEBUG

procA = MarlinProcessorWrapper("TestProcessor")
procA.OutputLevel = DEBUG
procA.ProcessorType = "AIDAProcessor"
procA.Parameters = ["FileName", "histograms", END_TAG,
                    "FileType", "root", END_TAG,
                    "Compress", "1", END_TAG,
                    "Verbosity", "DEBUG", END_TAG
                    ]
procA.Conversion = ["edm4hep::ReconstructedParticleCollection", "ReconstructedParticles", "VertexBarrelCollection",
                    "edm4hep::TrackCollection", "EFlowTrack", "VertexBarrelCollection",
                    "edm4hep::ParticleIDCollection", "ParticleIDs", "VertexBarrelCollection"
                    ]
procA.addTool(EDM4hep2LcioTool())
algList.append(procA)


from Configurables import ApplicationMgr
ApplicationMgr( TopAlg = algList,
                EvtSel = 'NONE',
                EvtMax   = 5,
                ExtSvc = [evtsvc],
                OutputLevel=DEBUG
)
