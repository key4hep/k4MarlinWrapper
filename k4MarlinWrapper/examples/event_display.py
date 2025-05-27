#!/usr/bin/env python3
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

from collections import namedtuple

from Gaudi.Configuration import INFO
from Configurables import MarlinProcessorWrapper, EventDataSvc, GeoSvc
from k4FWCore import ApplicationMgr, IOSvc
from k4FWCore.parseArgs import parser
from k4MarlinWrapper.io_helpers import IOHandlerHelper


parser.add_argument(
    "--inputFiles",
    action="extend",
    nargs="+",
    metavar=["file1", "file2"],
    help="One or multiple input files",
)

parser.add_argument(
    "--compactFile",
    help="Compact detector file to use",
    type=str,
    default="CLICPerformance/Visualisation/CLIC_o3_v06_CED/CLIC_o3_v06_CED.xml",
)

reco_args = parser.parse_known_args()[0]

algList = []
svcList = [EventDataSvc("EventDataSvc")]

iosvc = IOSvc()

geoSvc = GeoSvc("GeoSvc")
geoSvc.detectors = [reco_args.compactFile]
geoSvc.OutputLevel = INFO
geoSvc.EnableGeant4Geo = False
svcList.append(geoSvc)

io_handler = IOHandlerHelper(algList, iosvc)
io_handler.add_reader(reco_args.inputFiles)

ColConfig = namedtuple("ColConfig", ["col_name", "val1", "val2", "col_toggle_group"])
colConfigs = [
    ColConfig("VXDCollection", "0", "5", "1"),
    ColConfig("SITCollection", "0", "5", "1"),
    ColConfig("FTD_PIXELCollection", "0", "5", "1"),
    ColConfig("FTD_STRIPCollection", "0", "5", "1"),
    ColConfig("FTDCollection", "0", "5", "1"),
    ColConfig("TPCCollection", "0", "3", "1"),
    ColConfig("SETCollection", "0", "3", "1"),
    ColConfig("ETDCollection", "0", "3", "1"),
    ColConfig("VertexBarrelCollection", "0", "5", "1"),
    ColConfig("VertexEndcapCollection", "0", "5", "1"),
    ColConfig("InnerTrackerBarrelCollection", "0", "5", "1"),
    ColConfig("InnerTrackerEndcapCollection", "0", "5", "1"),
    ColConfig("OuterTrackerBarrelCollection", "0", "5", "1"),
    ColConfig("OuterTrackerEndcapCollection", "0", "5", "1"),
    ColConfig("VXDTrackerHits", "0", "5", "11"),
    ColConfig("SITTrackerHits", "0", "5", "11"),
    ColConfig("TPCTrackerHits", "0", "5", "11"),
    ColConfig("FTDTrackerHits", "0", "5", "11"),
    ColConfig("FTDStripTrackerHits", "0", "5", "11"),
    ColConfig("FTDPixelTrackerHits", "0", "5", "11"),
    ColConfig("FTDSpacePoints", "0", "5", "11"),
    ColConfig("SETTrackerHits", "0", "5", "11"),
    ColConfig("ITrackerEndcapHits", "0", "5", "11"),
    ColConfig("ITrackerHits", "0", "5", "11"),
    ColConfig("OTrackerEndcapHits", "0", "5", "11"),
    ColConfig("OTrackerHits", "0", "5", "11"),
    ColConfig("VXDEndcapTrackerHits", "0", "5", "11"),
    ColConfig("LHcalCollection", "0", "3", "2"),
    ColConfig("LumiCalCollection", "0", "3", "2"),
    ColConfig("MuonBarrelCollection", "0", "3", "2"),
    ColConfig("MuonEndCapCollection", "0", "3", "2"),
    ColConfig("EcalBarrelSiliconCollection", "0", "3", "2"),
    ColConfig("EcalBarrelSiliconPreShower", "0", "3", "2"),
    ColConfig("EcalEndcapRingCollection", "0", "3", "2"),
    ColConfig("EcalEndcapRingPreShower", "0", "3", "2"),
    ColConfig("EcalEndcapSiliconCollection", "0", "3", "2"),
    ColConfig("EcalEndcapSiliconPreShower", "0", "3", "2"),
    ColConfig("HcalBarrelRegCollection", "0", "3", "2"),
    ColConfig("HcalEndCapRingCollection", "0", "3", "2"),
    ColConfig("HcalEndCapsCollection", "0", "3", "2"),
    ColConfig("HcalEndcapRingCollection", "0", "3", "2"),
    ColConfig("HcalEndcapsCollection", "0", "3", "2"),
    ColConfig("ECalBarrelSiHitsEven", "0", "3", "2"),
    ColConfig("ECalBarrelSiHitsOdd", "0", "3", "2"),
    ColConfig("ECalEndcapSiHitsEven", "0", "3", "2"),
    ColConfig("ECalEndcapSiHitsOdd", "0", "3", "2"),
    ColConfig("EcalBarrelCollection", "0", "3", "2"),
    ColConfig("EcalEndcapsCollection", "0", "3", "2"),
    ColConfig("YokeEndcapsCollection", "0", "3", "2"),
    ColConfig("ECalBarrelCollection", "0", "3", "2"),
    ColConfig("ECalEndcapCollection", "0", "3", "2"),
    ColConfig("ECalPlugCollection", "0", "3", "2"),
    ColConfig("EcalBarrelCollection", "0", "3", "2"),
    ColConfig("EcalEndcapCollection", "0", "3", "2"),
    ColConfig("EcalPlugCollection", "0", "3", "2"),
    ColConfig("HCalBarrelCollection", "0", "3", "2"),
    ColConfig("HCalEndcapCollection", "0", "3", "2"),
    ColConfig("HCalRingCollection", "0", "3", "2"),
    ColConfig("YokeBarrelCollection", "0", "3", "2"),
    ColConfig("YokeEndcapCollection", "0", "3", "2"),
    ColConfig("LumiCalCollection", "0", "3", "2"),
    ColConfig("BeamCalCollection", "0", "3", "2"),
    ColConfig("HCALEndcap", "0", "5", "12"),
    ColConfig("HCALOther", "0", "5", "12"),
    ColConfig("MUON", "0", "2", "12"),
    ColConfig("LHCAL", "0", "3", "12"),
    ColConfig("LCAL", "0", "3", "12"),
    ColConfig("BCAL", "0", "3", "12"),
    ColConfig("ECALBarrel", "0", "2", "12"),
    ColConfig("ECALEndcap", "0", "2", "12"),
    ColConfig("ECALOther", "0", "2", "12"),
    ColConfig("HCALBarrel", "0", "5", "12"),
    ColConfig("EcalBarrelCollectionRec", "0", "5", "12"),
    ColConfig("EcalEndcapRingCollectionRec", "0", "5", "12"),
    ColConfig("EcalEndcapsCollectionRec", "0", "5", "12"),
    ColConfig("HcalBarrelCollectionRec", "0", "5", "12"),
    ColConfig("HcalEndcapRingCollectionRec", "0", "5", "12"),
    ColConfig("HcalEndcapsCollectionRec", "0", "5", "12"),
    ColConfig("TruthTracks", "0", "6", "3"),
    ColConfig("ForwardTracks", "0", "6", "4"),
    ColConfig("SiTracks", "0", "6", "5"),
    ColConfig("ClupatraTracks", "0", "6", "6"),
    ColConfig("MarlinTrkTracks", "0", "6", "7"),
    ColConfig("PandoraClusters", "0", "3", "8"),
    ColConfig("PandoraPFOs", "0", "3", "9"),
    ColConfig("MCParticle", "0", "3", "0"),
    ColConfig("VertexBarrelHits", "0", "5", "11"),
    ColConfig("VertexEndcapHits", "0", "5", "11"),
    ColConfig("InnerTrackerBarrelHits", "0", "5", "11"),
    ColConfig("InnerTrackerEndcapHits", "0", "5", "11"),
    ColConfig("OuterTrackerBarrelHits", "0", "5", "11"),
    ColConfig("OuterTrackerEndcapHits", "0", "5", "11"),
    ColConfig("ECalBarrelHits", "0", "3", "12"),
    ColConfig("ECalEndcapHits", "0", "3", "12"),
    ColConfig("ECalPlugHits", "0", "3", "12"),
    ColConfig("HCalBarrelHits", "0", "3", "12"),
    ColConfig("HCalEndcapHits", "0", "3", "12"),
    ColConfig("HCalRingHits", "0", "3", "12"),
    ColConfig("YokeBarrelHits", "0", "3", "12"),
    ColConfig("YokeEndcapHits", "0", "3", "12"),
    ColConfig("LumiCalHits", "0", "3", "12"),
    ColConfig("BeamCalHits", "0", "3", "12"),
    ColConfig("Tracks", "0", "3", "3"),
    ColConfig("SelectedPandoraPFOCollection", "0", "3", "4"),
    ColConfig("LooseSelectedPandoraPFOCollection", "0", "3", "5"),
    ColConfig("TightSelectedPandoraPFOCollection", "0", "3", "6"),
    ColConfig("PandoraPFOCollection", "0", "3", "7"),
    ColConfig("JetOut", "0", "0", "3"),
]

MyCEDViewer = MarlinProcessorWrapper("MyCEDViewer")
MyCEDViewer.OutputLevel = INFO
MyCEDViewer.ProcessorType = "DDCEDViewer"
MyCEDViewer.Parameters = {
    "ColorByEnergy": ["false"],
    "ColorByEnergyAutoColor": ["false"],
    "ColorByEnergyBrightness": ["1.0"],
    "ColorByEnergyMax": ["35.0"],
    "ColorByEnergyMin": ["0.0"],
    "ColorByEnergySaturation": ["1.0"],
    "ColorScheme": ["10"],
    "DetailledDrawing": ["VXD", "VertexBarrel"],
    "DrawDetector": ["true"],
    "DrawDetectorID": ["0"],
    "DrawHelixForPFOs": ["0"],
    "DrawHelixForTrack": ["0"],
    "DrawInLayer": [
        item
        for config in colConfigs
        for item in [
            config.col_name,
            str(config.val1),
            str(config.val2),
            str(config.col_toggle_group),
        ]
    ],
    "DrawSurfaces": ["true"],
    "HelixMaxR": ["2000"],
    "HelixMaxZ": ["2500"],
    "MCParticleEnergyCut": ["0.001"],
    "ScaleLineThickness": ["1"],
    "ScaleMarkerSize": ["1"],
    "UseColorForHelixTracks": ["0"],
    "UseTPCForLimitsOfHelix": ["true"],
    "UsingParticleGun": ["false"],
    "WaitForKeyboard": ["1"],
}

algList.append(MyCEDViewer)

# We need to convert the inputs in case we have EDM4hep input
io_handler.finalize_converters()

ApplicationMgr(TopAlg=algList, EvtSel="NONE", EvtMax=10, ExtSvc=svcList, OutputLevel=INFO)
