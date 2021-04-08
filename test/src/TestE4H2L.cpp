#include <TestE4H2L.h>

DECLARE_COMPONENT(TestE4H2L)

TestE4H2L::TestE4H2L(const std::string& name, ISvcLocator* pSL) : GaudiAlgorithm(name, pSL) {
  declareProperty("EDM4hep2LcioTool", m_edm_conversionTool = nullptr);
  declareProperty("Lcio2EDM4hepTool", m_lcio_conversionTool = nullptr);
}

StatusCode TestE4H2L::initialize() {

  m_dataHandlesMap[m_e4h_calohit_name] = new DataHandle<edm4hep::CalorimeterHitCollection>(
    m_e4h_calohit_name, Gaudi::DataHandle::Writer, this);
  m_dataHandlesMap[m_e4h_rawcalohit_name] = new DataHandle<edm4hep::RawCalorimeterHitCollection>(
    m_e4h_rawcalohit_name, Gaudi::DataHandle::Writer, this);
  m_dataHandlesMap[m_e4h_tpchit_name] = new DataHandle<edm4hep::TPCHitCollection>(
    m_e4h_tpchit_name, Gaudi::DataHandle::Writer, this);
  m_dataHandlesMap[m_e4h_trackerhit_name] = new DataHandle<edm4hep::TrackerHitCollection>(
    m_e4h_trackerhit_name, Gaudi::DataHandle::Writer, this);
  m_dataHandlesMap[m_e4h_track_name] = new DataHandle<edm4hep::TrackCollection>(
    m_e4h_track_name, Gaudi::DataHandle::Writer, this);

  return StatusCode::SUCCESS;
}

void TestE4H2L::createCalorimeterHits(
  const int num_elements,
  int& int_cnt,
  float& float_cnt)
{
  // edm4hep::CalorimeterHits
  auto* calohit_coll = new edm4hep::CalorimeterHitCollection();

  for (int i=0; i < num_elements; ++i) {

    auto* elem = new edm4hep::CalorimeterHit();

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
    m_dataHandlesMap[m_e4h_calohit_name]);
  calohit_handle->put(calohit_coll);
}


void TestE4H2L::createRawCalorimeterHits(
  const int num_elements,
  int& int_cnt,
  float& float_cnt)
{
  // edm4hep::CalorimeterHits
  auto* rawcalohit_coll = new edm4hep::RawCalorimeterHitCollection();

  for (int i=0; i < num_elements; ++i) {

    auto* elem = new edm4hep::RawCalorimeterHit();

    elem->setCellID(int_cnt++);
    elem->setAmplitude(int_cnt++);
    elem->setTimeStamp(int_cnt++);

    rawcalohit_coll->push_back(*elem);
  }

  auto* rawcalohit_handle = dynamic_cast<DataHandle<edm4hep::RawCalorimeterHitCollection>*>(
    m_dataHandlesMap[m_e4h_rawcalohit_name]);
  rawcalohit_handle->put(rawcalohit_coll);
}



void TestE4H2L::createTPCHits(
  const int num_elements,
  const int num_rawwords,
  int& int_cnt,
  float& float_cnt)
{
  // edm4hep::CalorimeterHits
  auto* tpchit_coll = new edm4hep::TPCHitCollection();

  for (int i=0; i < num_elements; ++i) {

    auto* elem = new edm4hep::TPCHit();

    elem->setCellID(int_cnt++);
    elem->setQuality(int_cnt++);
    elem->setTime(float_cnt++);
    elem->setCharge(float_cnt++);

    for (int j=0; j<num_rawwords; ++j) {
      elem->addToRawDataWords(int_cnt++);
    }

    tpchit_coll->push_back(*elem);
  }

  auto* tpchit_handle = dynamic_cast<DataHandle<edm4hep::TPCHitCollection>*>(
    m_dataHandlesMap[m_e4h_tpchit_name]);
  tpchit_handle->put(tpchit_coll);
}


void TestE4H2L::createTrackerHits(
  const int num_elements,
  int& int_cnt,
  float& float_cnt)
{

  auto trackerhit_coll = new edm4hep::TrackerHitCollection();

  for (int i=0; i < num_elements; ++i) {

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
    m_dataHandlesMap[m_e4h_trackerhit_name]);
  trackerhit_handle->put(trackerhit_coll);
}


void TestE4H2L::createTracks(
  const int num_elements,
  const int subdetectorhitnumbers,
  const std::vector<uint>& link_trackerhits_idx,
  const int num_track_states,
  const std::vector<std::pair<uint, uint>>& track_link_tracks_idx,
  int& int_cnt,
  float& float_cnt)
{
  // edm4hep::Track
  auto track_coll = new edm4hep::TrackCollection();

  for (int i=0; i < num_elements; ++i) {

    auto elem = new edm4hep::Track();

    elem->setType(2); // TODO specific type
    elem->setChi2(float_cnt++);
    elem->setNdf(int_cnt++);
    elem->setDEdx(float_cnt++);
    elem->setDEdxError(float_cnt++);
    elem->setRadiusOfInnermostHit(float_cnt++);

    for (int j=0; j < subdetectorhitnumbers; ++j){
      elem->addToSubDetectorHitNumbers(int_cnt++);
    }

    DataHandle<edm4hep::TrackerHitCollection> trackerhits_handle {
      m_e4h_trackerhit_name, Gaudi::DataHandle::Reader, this};
    const auto trackerhits_coll = trackerhits_handle.get();

    for (auto& idx : link_trackerhits_idx) {
      elem->addToTrackerHits((*trackerhits_coll)[idx]);
    }

    for (int j=0; j < num_track_states; ++j){

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
  for (const auto& [orig_idx, link_idx] : track_link_tracks_idx) {
    track_coll->at(orig_idx).addToTracks(track_coll->at(link_idx));
  }

  // Save track collection with DataHandle
  auto* track_handle = dynamic_cast<DataHandle<edm4hep::TrackCollection>*>(
    m_dataHandlesMap[m_e4h_track_name]);
  track_handle->put(track_coll);
}




bool TestE4H2L::checkEDMRawCaloHitLCIORawCaloHit(
  lcio::LCEventImpl* the_event)
{
  DataHandle<edm4hep::RawCalorimeterHitCollection> rawcalohit_handle_orig {
    m_e4h_rawcalohit_name, Gaudi::DataHandle::Reader, this};
  const auto rawcalohit_coll_orig = rawcalohit_handle_orig.get();

  auto lcio_rawcalohit_coll = the_event->getCollection(m_lcio_rawcalohit_name);
  auto lcio_coll_size = lcio_rawcalohit_coll->getNumberOfElements();

  bool rawcalohit_same = (*rawcalohit_coll_orig).size() == lcio_coll_size;

  if (rawcalohit_same) {
    for (int i=0; i < (*rawcalohit_coll_orig).size(); ++i) {

      auto edm_tpchit_orig = (*rawcalohit_coll_orig)[i];
      auto* lcio_tpchit = dynamic_cast<lcio::RawCalorimeterHitImpl*>(lcio_rawcalohit_coll->getElementAt(i));

      if (lcio_tpchit != nullptr) {

        uint64_t combined_value = 0;
        uint32_t* combined_value_ptr = reinterpret_cast<uint32_t*>(&combined_value);
        combined_value_ptr[0] = lcio_tpchit->getCellID0();
        combined_value_ptr[1] = lcio_tpchit->getCellID1();
        rawcalohit_same = rawcalohit_same && (edm_tpchit_orig.getCellID() == combined_value);
        rawcalohit_same = rawcalohit_same && (edm_tpchit_orig.getAmplitude() == lcio_tpchit->getAmplitude());
        rawcalohit_same = rawcalohit_same && (edm_tpchit_orig.getTimeStamp() == lcio_tpchit->getTimeStamp());
      }

    }
  }

  if (!rawcalohit_same) {
    debug() << "RawCalorimeterHit EDM4hep -> LCIO failed." << endmsg;
  }

  return rawcalohit_same;
}





bool TestE4H2L::checkEDMTPCHitLCIOTPCHit(
  lcio::LCEventImpl* the_event)
{
  DataHandle<edm4hep::TPCHitCollection> tpchit_handle_orig {
    m_e4h_tpchit_name, Gaudi::DataHandle::Reader, this};
  const auto tpchit_coll_orig = tpchit_handle_orig.get();

  auto lcio_tpchit_coll = the_event->getCollection(m_lcio_tpchit_name);
  auto lcio_coll_size = lcio_tpchit_coll->getNumberOfElements();

  bool tpchit_same = (*tpchit_coll_orig).size() == lcio_coll_size;

  if (tpchit_same) {
    for (int i=0; i < (*tpchit_coll_orig).size(); ++i) {

      auto edm_tpchit_orig = (*tpchit_coll_orig)[i];
      auto* lcio_tpchit = dynamic_cast<lcio::TPCHitImpl*>(lcio_tpchit_coll->getElementAt(i));

      if (lcio_tpchit != nullptr) {

        tpchit_same = tpchit_same && (edm_tpchit_orig.getCellID() == lcio_tpchit->getCellID());
        tpchit_same = tpchit_same && (edm_tpchit_orig.getTime() == lcio_tpchit->getTime());
        tpchit_same = tpchit_same && (edm_tpchit_orig.getCharge() == lcio_tpchit->getCharge());
        tpchit_same = tpchit_same && (edm_tpchit_orig.getQuality() == lcio_tpchit->getQuality());

        tpchit_same = tpchit_same && (edm_tpchit_orig.rawDataWords_size() == lcio_tpchit->getNRawDataWords());
        if (tpchit_same) {
          for (int j=0; j<lcio_tpchit->getNRawDataWords(); ++j) {
            tpchit_same = tpchit_same && (edm_tpchit_orig.getRawDataWords(j) == lcio_tpchit->getRawDataWord(j));
          }
        }
      }

    }
  }

  if (!tpchit_same) {
    debug() << "TPCHit EDM4hep -> LCIO failed." << endmsg;
  }

  return tpchit_same;
}




bool TestE4H2L::checkEDMTrackerHitLCIOTrackerHit(
  lcio::LCEventImpl* the_event)
{
  DataHandle<edm4hep::TrackerHitCollection> trackerhit_handle_orig {
    m_e4h_trackerhit_name, Gaudi::DataHandle::Reader, this};
  const auto trackerhit_coll_orig = trackerhit_handle_orig.get();

  auto lcio_trackerhit_coll = the_event->getCollection(m_lcio_trackerhit_name);
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
  lcio::LCEventImpl* the_event,
  const std::vector<uint>& link_trackerhits_idx,
  const std::vector<std::pair<uint, uint>>& track_link_tracks_idx)
{
  DataHandle<edm4hep::TrackCollection> track_handle_orig {
    m_e4h_track_name, Gaudi::DataHandle::Reader, this};
  const auto track_coll_orig = track_handle_orig.get();

  auto lcio_track_coll = the_event->getCollection(m_lcio_track_name);
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
        if (lcio_track->getTrackerHits().size() >= link_trackerhits_idx.size()) {

          auto* lcio_trackerhit_coll = the_event->getCollection(m_lcio_trackerhit_name);
          // Check the trackerhits in this track with the trackerhits from the event to be the same
          for (int j=0; j<link_trackerhits_idx.size(); ++j) {
            track_same = track_same &&
              (dynamic_cast<lcio::TrackerHitImpl*>(lcio_track->getTrackerHits()[j]) ==
               dynamic_cast<lcio::TrackerHitImpl*>(lcio_trackerhit_coll->getElementAt(link_trackerhits_idx[j])));
          }
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

    std::vector<uint> appeared(lcio_coll_size, 0);
    for (const auto& [orig_idx, link_idx] : track_link_tracks_idx) {
      auto* lcio_track_orig = dynamic_cast<lcio::TrackImpl*>(lcio_track_coll->getElementAt(orig_idx));
      auto* lcio_track_link = dynamic_cast<lcio::TrackImpl*>(lcio_track_coll->getElementAt(link_idx));
      track_same = track_same && (lcio_track_orig->getTracks()[appeared[orig_idx]] == lcio_track_link);
      appeared[orig_idx]++;
    }
  }

  if (!track_same) {
    debug() << "Track EDM4hep -> LCIO failed." << endmsg;
  }

  return track_same;
}



bool TestE4H2L::checkEDMCaloHitEDMCaloHit()
{
  // CalorimeterHit
  DataHandle<edm4hep::CalorimeterHitCollection> calohit_handle_orig {
    m_e4h_calohit_name, Gaudi::DataHandle::Reader, this};
  const auto calohit_coll_orig = calohit_handle_orig.get();
  DataHandle<edm4hep::CalorimeterHitCollection> calohit_handle {
    m_e4h_calohit_name + "_conv", Gaudi::DataHandle::Reader, this};
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
    m_e4h_track_name, Gaudi::DataHandle::Reader, this};
  const auto track_coll_orig = track_handle_orig.get();
  DataHandle<edm4hep::TrackCollection> track_handle {
    m_e4h_track_name + "_conv", Gaudi::DataHandle::Reader, this};
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


StatusCode TestE4H2L::execute() {

  lcio::LCEventImpl* the_event = new lcio::LCEventImpl();

  // Configuration for the test
  int int_cnt = 10;
  float float_cnt = 10.1;

  // CaloHit
  const int calohit_elems = 2;

  // RawCaloHit
  const int rawcalohit_elems = 2;

  // TPCHit
  const int tpchit_elems = 4;
  const int tpchit_rawwords = 6;

  // TrackerHits
  const int trackerhits_elems = 5;

  // Tracks
  const int track_elems = 4;
  const int track_subdetectorhitnumbers = 4;
  const std::vector<uint> track_link_trackerhits_idx =
    {0, 2, 4};
  const int track_states = 5;
  const std::vector<std::pair<uint, uint>> track_link_tracks_idx =
    {{0,2}, {1,3}, {2,3}, {3,2}, {3,0}};



  // Create fake collections based on configuration
  createCalorimeterHits(calohit_elems, int_cnt, float_cnt);
  createRawCalorimeterHits(rawcalohit_elems, int_cnt, float_cnt);
  createTPCHits(
    tpchit_elems,
    tpchit_rawwords,
    int_cnt,
    float_cnt);
  createTrackerHits(trackerhits_elems, int_cnt, float_cnt);
  createTracks(
    track_elems,
    track_subdetectorhitnumbers,
    track_link_trackerhits_idx,
    track_states,
    track_link_tracks_idx,
    int_cnt,
    float_cnt);

  // Convert from EDM4hep to LCIO
  StatusCode edm_sc = m_edm_conversionTool->convertCollections(the_event);

  // Check EDM4hep -> LCIO conversion
  bool lcio_same =
    checkEDMTrackLCIOTrack(
      the_event,
      track_link_trackerhits_idx,
      track_link_tracks_idx) &&
    checkEDMTrackerHitLCIOTrackerHit(the_event) &&
    checkEDMTPCHitLCIOTPCHit(the_event) &&
    checkEDMRawCaloHitLCIORawCaloHit(the_event);

  // Convert from LCIO to EDM4hep
  StatusCode lcio_sc =  m_lcio_conversionTool->convertCollections(the_event);

  // Check EDM4hep -> LCIO -> EDM4hep conversion
  bool edm_same =
    checkEDMCaloHitEDMCaloHit() &&
    checkEDMTrackEDMTrack();

  return (edm_same && lcio_same) ? StatusCode::SUCCESS : StatusCode::FAILURE;
}

StatusCode TestE4H2L::finalize() {
  return Algorithm::finalize();
}
