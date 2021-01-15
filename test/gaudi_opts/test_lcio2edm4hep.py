from Gaudi.Configuration import *

algList = []

from Configurables import k4DataSvc
podioevent = k4DataSvc("EventDataSvc")
# podioevent.input = "output_k4test_exampledata.root"
podioevent.input = "/eos/experiment/fcc/ee/generation/DelphesEvents/fcc_tmp/p8_ee_Ztautau_ecm91_EvtGen_Tau2MuGamma/events_001720714.root"

from Configurables import PodioInput
inp = PodioInput("InputReader")
inp.collections = ["ReconstructedParticles"]
algList.append(inp)

from Configurables import LCIO2EDM4hep
test = LCIO2EDM4hep("My_Alg")
algList.append(test)

from Configurables import ApplicationMgr
ApplicationMgr( TopAlg=algList,
                EvtSel="NONE",
                EvtMax=100,
                ExtSvc=[podioevent],
                OutputLevel=DEBUG,
                )
