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

from Configurables import EventDataSvc, GeoSvc, MarlinProcessorWrapper
from Gaudi.Configuration import INFO
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

ColDrawConfig = namedtuple("ColDrawConfig", ["col_name", "marker_type", "size", "toggle_group"])
colConfigs = [
    ColDrawConfig("VXDCollection", "0", "5", "1"),
    ColDrawConfig("SITCollection", "0", "5", "1"),
    ColDrawConfig("FTD_PIXELCollection", "0", "5", "1"),
    ColDrawConfig("FTD_STRIPCollection", "0", "5", "1"),
    ColDrawConfig("FTDCollection", "0", "5", "1"),
    ColDrawConfig("TPCCollection", "0", "3", "1"),
    ColDrawConfig("SETCollection", "0", "3", "1"),
    ColDrawConfig("ETDCollection", "0", "3", "1"),
    ColDrawConfig("VertexBarrelCollection", "0", "5", "1"),
    ColDrawConfig("VertexEndcapCollection", "0", "5", "1"),
    ColDrawConfig("InnerTrackerBarrelCollection", "0", "5", "1"),
    ColDrawConfig("InnerTrackerEndcapCollection", "0", "5", "1"),
    ColDrawConfig("OuterTrackerBarrelCollection", "0", "5", "1"),
    ColDrawConfig("OuterTrackerEndcapCollection", "0", "5", "1"),
    ColDrawConfig("VXDTrackerHits", "0", "5", "11"),
    ColDrawConfig("SITTrackerHits", "0", "5", "11"),
    ColDrawConfig("TPCTrackerHits", "0", "5", "11"),
    ColDrawConfig("FTDTrackerHits", "0", "5", "11"),
    ColDrawConfig("FTDStripTrackerHits", "0", "5", "11"),
    ColDrawConfig("FTDPixelTrackerHits", "0", "5", "11"),
    ColDrawConfig("FTDSpacePoints", "0", "5", "11"),
    ColDrawConfig("SETTrackerHits", "0", "5", "11"),
    ColDrawConfig("ITrackerEndcapHits", "0", "5", "11"),
    ColDrawConfig("ITrackerHits", "0", "5", "11"),
    ColDrawConfig("OTrackerEndcapHits", "0", "5", "11"),
    ColDrawConfig("OTrackerHits", "0", "5", "11"),
    ColDrawConfig("VXDEndcapTrackerHits", "0", "5", "11"),
    ColDrawConfig("LHcalCollection", "0", "3", "2"),
    ColDrawConfig("LumiCalCollection", "0", "3", "2"),
    ColDrawConfig("MuonBarrelCollection", "0", "3", "2"),
    ColDrawConfig("MuonEndCapCollection", "0", "3", "2"),
    ColDrawConfig("EcalBarrelSiliconCollection", "0", "3", "2"),
    ColDrawConfig("EcalBarrelSiliconPreShower", "0", "3", "2"),
    ColDrawConfig("EcalEndcapRingCollection", "0", "3", "2"),
    ColDrawConfig("EcalEndcapRingPreShower", "0", "3", "2"),
    ColDrawConfig("EcalEndcapSiliconCollection", "0", "3", "2"),
    ColDrawConfig("EcalEndcapSiliconPreShower", "0", "3", "2"),
    ColDrawConfig("HcalBarrelRegCollection", "0", "3", "2"),
    ColDrawConfig("HcalEndCapRingCollection", "0", "3", "2"),
    ColDrawConfig("HcalEndCapsCollection", "0", "3", "2"),
    ColDrawConfig("HcalEndcapRingCollection", "0", "3", "2"),
    ColDrawConfig("HcalEndcapsCollection", "0", "3", "2"),
    ColDrawConfig("ECalBarrelSiHitsEven", "0", "3", "2"),
    ColDrawConfig("ECalBarrelSiHitsOdd", "0", "3", "2"),
    ColDrawConfig("ECalEndcapSiHitsEven", "0", "3", "2"),
    ColDrawConfig("ECalEndcapSiHitsOdd", "0", "3", "2"),
    ColDrawConfig("EcalBarrelCollection", "0", "3", "2"),
    ColDrawConfig("EcalEndcapsCollection", "0", "3", "2"),
    ColDrawConfig("YokeEndcapsCollection", "0", "3", "2"),
    ColDrawConfig("ECalBarrelCollection", "0", "3", "2"),
    ColDrawConfig("ECalEndcapCollection", "0", "3", "2"),
    ColDrawConfig("ECalPlugCollection", "0", "3", "2"),
    ColDrawConfig("EcalBarrelCollection", "0", "3", "2"),
    ColDrawConfig("EcalEndcapCollection", "0", "3", "2"),
    ColDrawConfig("EcalPlugCollection", "0", "3", "2"),
    ColDrawConfig("HCalBarrelCollection", "0", "3", "2"),
    ColDrawConfig("HCalEndcapCollection", "0", "3", "2"),
    ColDrawConfig("HCalRingCollection", "0", "3", "2"),
    ColDrawConfig("YokeBarrelCollection", "0", "3", "2"),
    ColDrawConfig("YokeEndcapCollection", "0", "3", "2"),
    ColDrawConfig("LumiCalCollection", "0", "3", "2"),
    ColDrawConfig("BeamCalCollection", "0", "3", "2"),
    ColDrawConfig("HCALEndcap", "0", "5", "12"),
    ColDrawConfig("HCALOther", "0", "5", "12"),
    ColDrawConfig("MUON", "0", "2", "12"),
    ColDrawConfig("LHCAL", "0", "3", "12"),
    ColDrawConfig("LCAL", "0", "3", "12"),
    ColDrawConfig("BCAL", "0", "3", "12"),
    ColDrawConfig("ECALBarrel", "0", "2", "12"),
    ColDrawConfig("ECALEndcap", "0", "2", "12"),
    ColDrawConfig("ECALOther", "0", "2", "12"),
    ColDrawConfig("HCALBarrel", "0", "5", "12"),
    ColDrawConfig("EcalBarrelCollectionRec", "0", "5", "12"),
    ColDrawConfig("EcalEndcapRingCollectionRec", "0", "5", "12"),
    ColDrawConfig("EcalEndcapsCollectionRec", "0", "5", "12"),
    ColDrawConfig("HcalBarrelCollectionRec", "0", "5", "12"),
    ColDrawConfig("HcalEndcapRingCollectionRec", "0", "5", "12"),
    ColDrawConfig("HcalEndcapsCollectionRec", "0", "5", "12"),
    ColDrawConfig("TruthTracks", "0", "6", "3"),
    ColDrawConfig("ForwardTracks", "0", "6", "4"),
    ColDrawConfig("SiTracks", "0", "6", "5"),
    ColDrawConfig("ClupatraTracks", "0", "6", "6"),
    ColDrawConfig("MarlinTrkTracks", "0", "6", "7"),
    ColDrawConfig("PandoraClusters", "0", "3", "8"),
    ColDrawConfig("PandoraPFOs", "0", "3", "9"),
    ColDrawConfig("MCParticle", "0", "3", "0"),
    ColDrawConfig("VertexBarrelHits", "0", "5", "11"),
    ColDrawConfig("VertexEndcapHits", "0", "5", "11"),
    ColDrawConfig("InnerTrackerBarrelHits", "0", "5", "11"),
    ColDrawConfig("InnerTrackerEndcapHits", "0", "5", "11"),
    ColDrawConfig("OuterTrackerBarrelHits", "0", "5", "11"),
    ColDrawConfig("OuterTrackerEndcapHits", "0", "5", "11"),
    ColDrawConfig("ECalBarrelHits", "0", "3", "12"),
    ColDrawConfig("ECalEndcapHits", "0", "3", "12"),
    ColDrawConfig("ECalPlugHits", "0", "3", "12"),
    ColDrawConfig("HCalBarrelHits", "0", "3", "12"),
    ColDrawConfig("HCalEndcapHits", "0", "3", "12"),
    ColDrawConfig("HCalRingHits", "0", "3", "12"),
    ColDrawConfig("YokeBarrelHits", "0", "3", "12"),
    ColDrawConfig("YokeEndcapHits", "0", "3", "12"),
    ColDrawConfig("LumiCalHits", "0", "3", "12"),
    ColDrawConfig("BeamCalHits", "0", "3", "12"),
    ColDrawConfig("Tracks", "0", "3", "3"),
    ColDrawConfig("SelectedPandoraPFOCollection", "0", "3", "4"),
    ColDrawConfig("LooseSelectedPandoraPFOCollection", "0", "3", "5"),
    ColDrawConfig("TightSelectedPandoraPFOCollection", "0", "3", "6"),
    ColDrawConfig("PandoraPFOCollection", "0", "3", "7"),
    ColDrawConfig("JetOut", "0", "0", "3"),
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
            str(config.marker_type),
            str(config.size),
            str(config.toggle_group),
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
