#include <TestE4H2L.h>

DECLARE_COMPONENT(TestE4H2L)

TestE4H2L::TestE4H2L(const std::string& name, ISvcLocator* pSL) : GaudiAlgorithm(name, pSL) {
  declareProperty("EDM4hep2LcioTool", m_edm_conversionTool = nullptr);
  declareProperty("Lcio2EDM4hepTool", m_lcio_conversionTool = nullptr);
}

StatusCode TestE4H2L::initialize() {

  m_dataHandlesMap[m_edm_callohit_name] = new DataHandle<edm4hep::CalorimeterHitCollection>(
    m_edm_callohit_name, Gaudi::DataHandle::Writer, this);
  m_dataHandlesMap[m_edm_trackerhit_name] = new DataHandle<edm4hep::TrackerHitCollection>(
    m_edm_trackerhit_name, Gaudi::DataHandle::Writer, this);
  m_dataHandlesMap[m_edm_track_name] = new DataHandle<edm4hep::TrackCollection>(
    m_edm_track_name, Gaudi::DataHandle::Writer, this);

  return StatusCode::SUCCESS;
}

void TestE4H2L::createCalorimeterHits(
  int& int_cnt,
  float& float_cnt)
{
  // edm4hep::CalorimeterHits
  const int calohit_elems = 2;
  auto calohit_coll = new edm4hep::CalorimeterHitCollection();

  for (int i=0; i < calohit_elems; ++i) {

    auto elem = new edm4hep::CalorimeterHit();

    elem->setCellID(int_cnt++);
    elem->setEnergy(float_cnt++);
    elem->setEnergyError(float_cnt++);
    elem->setTime(float_cnt++);
    edm4hep::Vector3f test_vec {float_cnt++, float_cnt++, float_cnt++};
    elem->setPosition(test_vec);
    elem->setType(int_cnt++);

    calohit_coll->push_back(*elem);
  }

  auto* calohit_handle = dynamic_cast<DataHandle<edm4hep::CalorimeterHitCollection>*>(
    m_dataHandlesMap[m_edm_callohit_name]);
  calohit_handle->put(calohit_coll);
}


void TestE4H2L::createTrackerHits(
  int& int_cnt,
  float& float_cnt)
{
  const int trackerhits_elems = 5;
  auto trackerhit_coll = new edm4hep::TrackerHitCollection();

  for (int i=0; i < trackerhits_elems; ++i) {

    auto elem = new edm4hep::TrackerHit();

    elem->setCellID(int_cnt++);
    elem->setType(int_cnt++);
    elem->setQuality(int_cnt++);
    elem->setTime(float_cnt++);
    elem->setEDep(float_cnt++);
    elem->setEDepError(float_cnt++);
    elem->setEdx(float_cnt++);
    edm4hep::Vector3d test_vec {float_cnt++, float_cnt++, float_cnt++};
    elem->setPosition(test_vec);
    std::array<float, 6> test_array{
      float_cnt++, float_cnt++, float_cnt++,
      float_cnt++, float_cnt++, float_cnt++};
    elem->setCovMatrix(test_array);

    trackerhit_coll->push_back(*elem);
  }

  auto* trackerhit_handle = dynamic_cast<DataHandle<edm4hep::TrackerHitCollection>*>(
    m_dataHandlesMap[m_edm_trackerhit_name]);
  trackerhit_handle->put(trackerhit_coll);
}


void TestE4H2L::createTracks(
  int& int_cnt,
  float& float_cnt)
{
  // edm4hep::Track
  const int track_elems = 4;
  auto track_coll = new edm4hep::TrackCollection();

  for (int i=0; i < track_elems; ++i) {

    auto elem = new edm4hep::Track();

    elem->setType(2); // TODO specific type
    elem->setChi2(float_cnt++);
    elem->setNdf(int_cnt++);
    elem->setDEdx(float_cnt++);
    elem->setDEdxError(float_cnt++);
    elem->setRadiusOfInnermostHit(float_cnt++);

    const int track_subdetectorhitnumbers = 4;
    for (int j=0; j < track_subdetectorhitnumbers; ++j){
      elem->addToSubDetectorHitNumbers(int_cnt++);
    }

    DataHandle<edm4hep::TrackerHitCollection> trackerhits_handle {
      m_edm_trackerhit_name, Gaudi::DataHandle::Reader, this};
    const auto trackerhits_coll = trackerhits_handle.get();

    elem->addToTrackerHits((*trackerhits_coll)[0]);
    elem->addToTrackerHits((*trackerhits_coll)[2]);
    elem->addToTrackerHits((*trackerhits_coll)[4]);

    const int track_states = 5;
    for (int j=0; j < track_states; ++j){

      edm4hep::TrackState trackstate;

      trackstate.location = int_cnt++;
      trackstate.D0 = float_cnt++;
      trackstate.phi = float_cnt++;
      trackstate.omega = float_cnt++;
      trackstate.Z0 = float_cnt++;
      trackstate.tanLambda = float_cnt++;
      edm4hep::Vector3f test_vec {float_cnt++, float_cnt++, float_cnt++};
      trackstate.referencePoint = test_vec;
      std::array<float, 15> test_array = {
        float_cnt++, float_cnt++, float_cnt++,
        float_cnt++, float_cnt++, float_cnt++,
        float_cnt++, float_cnt++, float_cnt++,
        float_cnt++, float_cnt++, float_cnt++,
        float_cnt++, float_cnt++, float_cnt++};
      trackstate.covMatrix = test_array;

      elem->addToTrackStates(trackstate);
    }

    track_coll->push_back(*elem);
  }

  // Connect tracks between them
  if (track_coll->size() >= 4) {
    // Add 0 with 2
    track_coll->at(0).addToTracks(track_coll->at(2));
    // Add 1 with 3
    track_coll->at(1).addToTracks(track_coll->at(3));
    // Add 2 with 3 + circular dependency
    track_coll->at(2).addToTracks(track_coll->at(3));
    track_coll->at(3).addToTracks(track_coll->at(2));
    // Add 3 with 0
    track_coll->at(3).addToTracks(track_coll->at(0));
  }

  auto* track_handle = dynamic_cast<DataHandle<edm4hep::TrackCollection>*>(
    m_dataHandlesMap[m_edm_track_name]);
  track_handle->put(track_coll);
}

void TestE4H2L::createFakeCollections()
{
  int int_cnt = 10;
  float float_cnt = 10.1;

  createCalorimeterHits(int_cnt, float_cnt);
  createTrackerHits(int_cnt, float_cnt);
  createTracks(int_cnt, float_cnt);
}



bool TestE4H2L::checkEDMTrackerHitLCIOTrackerHit(
  lcio::LCEventImpl* the_event)
{
  DataHandle<edm4hep::TrackerHitCollection> trackerhit_handle_orig {
    "E4H_TrackerHitCollection", Gaudi::DataHandle::Reader, this};
  const auto trackerhit_coll_orig = trackerhit_handle_orig.get();

  auto lcio_trackerhit_coll = the_event->getCollection("LCIO_TrackerHitCollection");
  auto lcio_coll_size = lcio_trackerhit_coll->getNumberOfElements();

  bool trackerhit_same = (*trackerhit_coll_orig).size() == lcio_coll_size;

  if (trackerhit_same) {
    for (int i=0; i < (*trackerhit_coll_orig).size(); ++i) {

      auto edm_trackerhit_orig = (*trackerhit_coll_orig)[i];
      auto* lcio_trackerhit = dynamic_cast<lcio::TrackerHitImpl*>(lcio_trackerhit_coll->getElementAt(i));

      if (lcio_trackerhit != nullptr) {
        // TODO handle split CellIDs
        uint64_t combined_value = 0;
        uint32_t* combined_value_ptr = reinterpret_cast<uint32_t*>(&combined_value);
        combined_value_ptr[0] = lcio_trackerhit->getCellID0();
        combined_value_ptr[1] = lcio_trackerhit->getCellID1();
        trackerhit_same = trackerhit_same && (edm_trackerhit_orig.getCellID() == combined_value);

        trackerhit_same = trackerhit_same && (edm_trackerhit_orig.getType() == lcio_trackerhit->getType());
        trackerhit_same = trackerhit_same && (edm_trackerhit_orig.getQuality() == lcio_trackerhit->getQuality());
        trackerhit_same = trackerhit_same && (edm_trackerhit_orig.getTime() == lcio_trackerhit->getTime());
        trackerhit_same = trackerhit_same && (edm_trackerhit_orig.getEDep() == lcio_trackerhit->getEDep());
        trackerhit_same = trackerhit_same && (edm_trackerhit_orig.getEDepError() == lcio_trackerhit->getEDepError());
        for (int k=0; k<3; ++k) {
          trackerhit_same = trackerhit_same && (edm_trackerhit_orig.getPosition()[k] == lcio_trackerhit->getPosition()[k]);
        }
        for (int k=0; k<lcio_trackerhit->getCovMatrix().size(); ++k) {
          trackerhit_same = trackerhit_same && (edm_trackerhit_orig.getCovMatrix(k) == lcio_trackerhit->getCovMatrix()[k]);
        }

        // TODO Raw hits
      }

    }
  }

  if (!trackerhit_same) {
    debug() << "Track EDM4hep -> LCIO failed." << endmsg;
  }

  return trackerhit_same;
}


bool TestE4H2L::checkEDMTrackLCIOTrack(
  lcio::LCEventImpl* the_event)
{
  DataHandle<edm4hep::TrackCollection> track_handle_orig {
    "E4H_TrackCollection", Gaudi::DataHandle::Reader, this};
  const auto track_coll_orig = track_handle_orig.get();

  auto lcio_track_coll = the_event->getCollection("LCIO_TrackCollection");
  auto lcio_coll_size = lcio_track_coll->getNumberOfElements();

  bool track_same = (*track_coll_orig).size() == lcio_coll_size;

  if (track_same) {
    for (int i=0; i < (*track_coll_orig).size(); ++i) {
      auto edm_track_orig = (*track_coll_orig)[i];
      auto* lcio_track = dynamic_cast<lcio::TrackImpl*>(lcio_track_coll->getElementAt(i));

      // TODO review EDM4hep to LCIO Track conversion for setTypeBit
      // track_same = track_same && (edm_track_orig.getType() == lcio_track->getType());

      track_same = track_same && (edm_track_orig.getChi2() == lcio_track->getChi2());
      track_same = track_same && (edm_track_orig.getNdf() == lcio_track->getNdf());
      track_same = track_same && (edm_track_orig.getDEdx() == lcio_track->getdEdx());
      track_same = track_same && (edm_track_orig.getDEdxError() == lcio_track->getdEdxError());
      track_same = track_same && (edm_track_orig.getRadiusOfInnermostHit() == lcio_track->getRadiusOfInnermostHit());

      std::vector<edm4hep::TrackerHit> edm_trackerhits;
      std::vector<lcio::TrackerHitImpl*> lcio_trackerhits;

      track_same = track_same && (edm_track_orig.trackerHits_size() == lcio_track->getTrackerHits().size());
      if (track_same) {
        // Check linked trackerhits connections
        if (lcio_track->getTrackerHits().size() >= 3) {
          // get linked trackerhits to track
          auto* lcio_tr_trackerhit0 = dynamic_cast<lcio::TrackerHitImpl*>(lcio_track->getTrackerHits()[0]);
          auto* lcio_tr_trackerhit1 = dynamic_cast<lcio::TrackerHitImpl*>(lcio_track->getTrackerHits()[1]);
          auto* lcio_tr_trackerhit2 = dynamic_cast<lcio::TrackerHitImpl*>(lcio_track->getTrackerHits()[2]);

          auto* lcio_trackerhit_coll = the_event->getCollection("LCIO_TrackerHitCollection");

          // get trackerhits directly from the collection
          auto* lcio_trackerhit0 = dynamic_cast<lcio::TrackerHitImpl*>(lcio_trackerhit_coll->getElementAt(0));
          auto* lcio_trackerhit2 = dynamic_cast<lcio::TrackerHitImpl*>(lcio_trackerhit_coll->getElementAt(2));
          auto* lcio_trackerhit4 = dynamic_cast<lcio::TrackerHitImpl*>(lcio_trackerhit_coll->getElementAt(4));

          track_same = track_same && (lcio_tr_trackerhit0 == lcio_trackerhit0);
          track_same = track_same && (lcio_tr_trackerhit1 == lcio_trackerhit2);
          track_same = track_same && (lcio_tr_trackerhit2 == lcio_trackerhit4);
        }
      }

      // TODO Resizing in EDM4hep to LCIO conversion causes to "have" 50 hits
      if (lcio_track->getSubdetectorHitNumbers().size() == 50) {
        for (int j=0; j < edm_track_orig.subDetectorHitNumbers_size(); ++j) {
          track_same = track_same && (edm_track_orig.getSubDetectorHitNumbers(j) == lcio_track->getSubdetectorHitNumbers()[j]);
        }
        for (int j=edm_track_orig.subDetectorHitNumbers_size(); j<50; ++j) {
          track_same = track_same && (0 == lcio_track->getSubdetectorHitNumbers()[j]);
        }
      } else {
        track_same = track_same && (edm_track_orig.subDetectorHitNumbers_size() == lcio_track->getSubdetectorHitNumbers().size());
      }


      track_same = track_same && (edm_track_orig.trackStates_size() == lcio_track->getTrackStates().size());
      if ((edm_track_orig.trackStates_size() == lcio_track->getTrackStates().size())) {
        for (int j=0; j < edm_track_orig.trackStates_size(); ++j) {

          auto edm_trackestate_orig = edm_track_orig.getTrackStates(j);
          auto lcio_trackestate = lcio_track->getTrackStates()[j];

          track_same = track_same && (edm_trackestate_orig.location == lcio_trackestate->getLocation());
          track_same = track_same && (edm_trackestate_orig.D0 == lcio_trackestate->getD0());
          track_same = track_same && (edm_trackestate_orig.phi == lcio_trackestate->getPhi());
          track_same = track_same && (edm_trackestate_orig.omega == lcio_trackestate->getOmega());
          track_same = track_same && (edm_trackestate_orig.Z0 == lcio_trackestate->getZ0());
          track_same = track_same && (edm_trackestate_orig.tanLambda == lcio_trackestate->getTanLambda());
          for (int k=0; k<3; ++k) {
            track_same = track_same && (edm_trackestate_orig.referencePoint[k] == lcio_trackestate->getReferencePoint()[k]);
          }
          // Check same size cov matrix
          track_same = track_same && (edm_trackestate_orig.covMatrix.size() == lcio_trackestate->getCovMatrix().size());
          if (edm_trackestate_orig.covMatrix.size() == lcio_trackestate->getCovMatrix().size()) {
            for (int k=0; k < edm_trackestate_orig.covMatrix.size(); ++k) {
              track_same = track_same && (edm_trackestate_orig.covMatrix[k] == lcio_trackestate->getCovMatrix()[k]);
            }
          }

        }
      }

    }

    // // Check track connections
    if (lcio_coll_size >= 4) {
      auto* lcio_track0 = dynamic_cast<lcio::TrackImpl*>(lcio_track_coll->getElementAt(0));
      auto* lcio_track1 = dynamic_cast<lcio::TrackImpl*>(lcio_track_coll->getElementAt(1));
      auto* lcio_track2 = dynamic_cast<lcio::TrackImpl*>(lcio_track_coll->getElementAt(2));
      auto* lcio_track3 = dynamic_cast<lcio::TrackImpl*>(lcio_track_coll->getElementAt(3));

      track_same = track_same && (lcio_track0->getTracks()[0] == lcio_track2);
      track_same = track_same && (lcio_track1->getTracks()[0] == lcio_track3);
      track_same = track_same && (lcio_track2->getTracks()[0] == lcio_track3);
      track_same = track_same && (lcio_track3->getTracks()[0] == lcio_track2);
      track_same = track_same && (lcio_track3->getTracks()[1] == lcio_track0);
    }
  }

  if (!track_same) {
    debug() << "Track EDM4hep -> LCIO failed." << endmsg;
  }

  return track_same;

}


bool TestE4H2L::isSameEDM4hepLCIO(
  lcio::LCEventImpl* the_event)
{
  bool generalCheck = false;

  generalCheck =
    checkEDMTrackLCIOTrack(the_event) &&
    checkEDMTrackerHitLCIOTrackerHit(the_event);

  return generalCheck;
}



bool TestE4H2L::checkEDMCaloHitEDMCaloHit()
{
  // CalorimeterHit
  DataHandle<edm4hep::CalorimeterHitCollection> calohit_handle_orig {
    "E4H_CaloHitCollection", Gaudi::DataHandle::Reader, this};
  const auto calohit_coll_orig = calohit_handle_orig.get();
  DataHandle<edm4hep::CalorimeterHitCollection> calohit_handle {
    "E4H_CaloHitCollection_conv", Gaudi::DataHandle::Reader, this};
  const auto calohit_coll = calohit_handle.get();

  bool calohit_same = true;

  if ((*calohit_coll).size() > 0) { // avoid always true problems
    for (int i=0; i < (*calohit_coll).size(); ++i) {
      auto edm_calohit_orig = (*calohit_coll_orig)[i];
      auto edm_calohit = (*calohit_coll)[i];

      calohit_same = calohit_same && (edm_calohit_orig.getCellID() == edm_calohit.getCellID());
      calohit_same = calohit_same && (edm_calohit_orig.getEnergy() == edm_calohit.getEnergy());
      calohit_same = calohit_same && (edm_calohit_orig.getEnergyError() == edm_calohit.getEnergyError());
      calohit_same = calohit_same && (edm_calohit_orig.getTime() == edm_calohit.getTime());
      calohit_same = calohit_same && (edm_calohit_orig.getPosition() == edm_calohit.getPosition());
      calohit_same = calohit_same && (edm_calohit_orig.getType() == edm_calohit.getType());
    }
  }

  if (!calohit_same) {
    debug() << "CalorimeterHit EDM4hep -> LCIO -> EDM4hep failed." << endmsg;
  }

  return calohit_same;
}


bool TestE4H2L::checkEDMTrackEDMTrack()
{
  // Track
  DataHandle<edm4hep::TrackCollection> track_handle_orig {
    "E4H_TrackCollection", Gaudi::DataHandle::Reader, this};
  const auto track_coll_orig = track_handle_orig.get();
  DataHandle<edm4hep::TrackCollection> track_handle {
    "E4H_TrackCollection_conv", Gaudi::DataHandle::Reader, this};
  const auto track_coll = track_handle.get();

  bool track_same = (*track_coll_orig).size() == (*track_coll).size();

  if (track_same) {
    for (int i=0; i < (*track_coll).size(); ++i) {
      auto edm_track_orig = (*track_coll_orig)[i];
      auto edm_track = (*track_coll)[i];

      // TODO Review EDM4hep to LCIO Track conversion for TypeBit
      // track_same = track_same && (edm_track_orig.getType() == edm_track.getType());
      track_same = track_same && (edm_track_orig.getChi2() == edm_track.getChi2());
      track_same = track_same && (edm_track_orig.getNdf() == edm_track.getNdf());
      track_same = track_same && (edm_track_orig.getDEdx() == edm_track.getDEdx());
      track_same = track_same && (edm_track_orig.getDEdxError() == edm_track.getDEdxError());
      track_same = track_same && (edm_track_orig.getRadiusOfInnermostHit() == edm_track.getRadiusOfInnermostHit());

      // track_same = track_same && (edm_track_orig.trackerHits_size() == edm_track.trackerHits_size());
      // if ((edm_track_orig.trackerHits_size() == edm_track.trackerHits_size())) {
      //   for (int j=0; j< edm_track_orig.trackerHits_size(); ++j) {

      //     // auto edm_trackerhit_orig = edm_track_orig.getTrackerHits(j);
      //     // auto edm_trackerhit = edm_track.getTrackerHits(j);

      //     // track_same = track_same && (edm_trackerhit_orig.getCellID() == edm_trackerhit.getCellID());
      //     // track_same = track_same && (edm_trackerhit_orig.getType() == edm_trackerhit.getType());
      //     // track_same = track_same && (edm_trackerhit_orig.getQuality() == edm_trackerhit.getQuality());
      //     // track_same = track_same && (edm_trackerhit_orig.getTime() == edm_trackerhit.getTime());
      //     // track_same = track_same && (edm_trackerhit_orig.getEDep() == edm_trackerhit.getEDep());

      //     // track_same = track_same && (edm_trackerhit_orig.getEDepError() == edm_trackerhit.getEDepError());
      //     // track_same = track_same && (edm_trackerhit_orig.getPosition() == edm_trackerhit.getPosition());
      //     // track_same = track_same && (edm_trackerhit_orig.getCovMatrix() == edm_trackerhit.getCovMatrix());

      //     // TODO Raw hits
      //   }
      // }

      // track_same = track_same && (edm_track_orig.subDetectorHitNumbers_size() == edm_track.subDetectorHitNumbers_size());
      // if ((edm_track_orig.subDetectorHitNumbers_size() == edm_track.subDetectorHitNumbers_size())) {
      //   for (int j=0; j< edm_track_orig.subDetectorHitNumbers_size(); ++j) {
      //     track_same = track_same && (edm_track_orig.getSubDetectorHitNumbers(j) == edm_track.getSubDetectorHitNumbers(j));
      //   }
      // }

      // track_same = track_same && (edm_track_orig.trackStates_size() == edm_track.trackStates_size());
      // if ((edm_track_orig.trackStates_size() == edm_track.trackStates_size())) {
      //   for (int j=0; j< edm_track_orig.trackStates_size(); ++j) {

      //     auto edm_trackestate_orig = edm_track_orig.getTrackStates(j);
      //     auto edm_trackestate = edm_track.getTrackStates(j);

      //     track_same = track_same && (edm_trackestate_orig.location == edm_trackestate.location);
      //     track_same = track_same && (edm_trackestate_orig.D0 == edm_trackestate.D0);
      //     track_same = track_same && (edm_trackestate_orig.phi == edm_trackestate.phi);
      //     track_same = track_same && (edm_trackestate_orig.omega == edm_trackestate.omega);

      //     track_same = track_same && (edm_trackestate_orig.Z0 == edm_trackestate.Z0);
      //     track_same = track_same && (edm_trackestate_orig.tanLambda == edm_trackestate.tanLambda);
      //     track_same = track_same && (edm_trackestate_orig.referencePoint == edm_trackestate.referencePoint);
      //     track_same = track_same && (edm_trackestate_orig.covMatrix == edm_trackestate.covMatrix);
      //   }
      // }

    }
  }

  if (!track_same) {
    debug() << "Track EDM4hep -> LCIO -> EDM4hep failed." << endmsg;
  }

  return track_same;
}

bool TestE4H2L::isSameEDM4hepEDM4hep()
{

  bool generalCheck = false;

  // Do general check
  generalCheck =
    checkEDMCaloHitEDMCaloHit() &&
    checkEDMTrackEDMTrack();

  return generalCheck;
}


StatusCode TestE4H2L::execute() {

  lcio::LCEventImpl* the_event = new lcio::LCEventImpl();

  createFakeCollections();

  // Convert from EDM4hep to LCIO
  StatusCode edm_sc = m_edm_conversionTool->convertCollections(the_event);

  bool lcio_same = isSameEDM4hepLCIO(the_event);

  // Convert from LCIO to EDM4hep
  StatusCode lcio_sc =  m_lcio_conversionTool->convertCollections(the_event);

  bool edm_same = isSameEDM4hepEDM4hep();

  return (edm_same && lcio_same) ? StatusCode::SUCCESS : StatusCode::FAILURE;
}

StatusCode TestE4H2L::finalize() {
  return Algorithm::finalize();
}
