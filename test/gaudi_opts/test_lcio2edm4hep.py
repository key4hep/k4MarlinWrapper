from Gaudi.Configuration import *

from Configurables import k4DataSvc, LCIO2EDM4hep

algList = []

# theFile= 'edminput.root'
theFile = '/eos/experiment/fcc/ee/generation/DelphesEvents/fcc_tmp/p8_ee_Ztautau_ecm91_EvtGen_Tau2MuGamma/events_001720714.root'
evtsvc = k4DataSvc('EventDataSvc')
# evtsvc.input = '$k4MarlinWrapper_tests_DIR/inputFiles/' + theFile
evtsvc.input = theFile

from Configurables import PodioInput
inp = PodioInput('InputReader')
inp.collections = [
    'Particle',
    'ReconstructedParticles',
    'EFlowTrack'
]
inp.OutputLevel = DEBUG
algList.append(inp)

tester = LCIO2EDM4hep()
algList.append(tester)

from Configurables import ApplicationMgr
ApplicationMgr( TopAlg = algList,
                EvtSel = 'NONE',
                EvtMax   = 2,
                ExtSvc = [evtsvc],
                OutputLevel=DEBUG
)
