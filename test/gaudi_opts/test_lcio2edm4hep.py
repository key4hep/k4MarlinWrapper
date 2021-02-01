from Gaudi.Configuration import *

from Configurables import k4DataSvc, LCIO2EDM4hep, MarlinProcessorWrapper

algList = []

END_TAG = "END_TAG"

# theFile= 'edminput.root'
theFile = '/eos/experiment/fcc/ee/generation/DelphesEvents/fcc_tmp/p8_ee_Ztautau_ecm91_EvtGen_Tau2MuGamma/events_001720714.root'
evtsvc = k4DataSvc('EventDataSvc')
# evtsvc.input = '$k4MarlinWrapper_tests_DIR/inputFiles/' + theFile
evtsvc.input = theFile

from Configurables import PodioInput
inp = PodioInput('InputReader')
inp.collections = [
    'Particle',
    'ParticleIDs',
    'ReconstructedParticles',
    'EFlowTrack'
]
inp.OutputLevel = DEBUG
algList.append(inp)

lcioConverter = LCIO2EDM4hep()
algList.append(lcioConverter)

procA = MarlinProcessorWrapper("TestProcessor")
procA.OutputLevel = DEBUG
procA.ProcessorType = "AIDAProcessor"
procA.Parameters = ["FileName", "histograms", END_TAG,
                    "FileType", "root", END_TAG,
                    "Compress", "1", END_TAG,
                    "Verbosity", "DEBUG", END_TAG
                    ]
procA.Conversion = ["EDM4hep2Lcio", "true", END_TAG,
                    "edm4hep::Track", "EFlowTrack", "VertexBarrelCollection", END_TAG
                    ]
algList.append(procA)

from Configurables import ApplicationMgr
ApplicationMgr( TopAlg = algList,
                EvtSel = 'NONE',
                EvtMax   = 2,
                ExtSvc = [evtsvc],
                OutputLevel=DEBUG
)
