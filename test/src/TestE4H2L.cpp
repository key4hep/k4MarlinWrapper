#include "TestE4H2L.h"

DECLARE_COMPONENT(TestE4H2L)

TestE4H2L::TestE4H2L(const std::string& name, ISvcLocator* pSL) : GaudiAlgorithm(name, pSL) {
  declareProperty("EDM4hep2LcioTool", m_edm_conversionTool = nullptr);
  declareProperty("Lcio2EDM4hepTool", m_lcio_conversionTool = nullptr);
}

StatusCode TestE4H2L::initialize() {
  m_dataHandlesMap[m_e4h_calohit_name] =
      new DataHandle<edm4hep::CalorimeterHitCollection>(m_e4h_calohit_name, Gaudi::DataHandle::Writer, this);
  m_dataHandlesMap[m_e4h_rawcalohit_name] =
      new DataHandle<edm4hep::RawCalorimeterHitCollection>(m_e4h_rawcalohit_name, Gaudi::DataHandle::Writer, this);
  m_dataHandlesMap[m_e4h_simcalohit_name] =
      new DataHandle<edm4hep::SimCalorimeterHitCollection>(m_e4h_simcalohit_name, Gaudi::DataHandle::Writer, this);
  m_dataHandlesMap[m_e4h_tpchit_name] =
      new DataHandle<edm4hep::TPCHitCollection>(m_e4h_tpchit_name, Gaudi::DataHandle::Writer, this);
  m_dataHandlesMap[m_e4h_trackerhit_name] =
      new DataHandle<edm4hep::TrackerHitCollection>(m_e4h_trackerhit_name, Gaudi::DataHandle::Writer, this);
  m_dataHandlesMap[m_e4h_simtrackerhit_name] =
      new DataHandle<edm4hep::SimTrackerHitCollection>(m_e4h_simtrackerhit_name, Gaudi::DataHandle::Writer, this);
  m_dataHandlesMap[m_e4h_track_name] =
      new DataHandle<edm4hep::TrackCollection>(m_e4h_track_name, Gaudi::DataHandle::Writer, this);
  m_dataHandlesMap[m_e4h_mcparticle_name] =
      new DataHandle<edm4hep::MCParticleCollection>(m_e4h_mcparticle_name, Gaudi::DataHandle::Writer, this);

  return StatusCode::SUCCESS;
}

void TestE4H2L::createCalorimeterHits(const int num_elements, int& int_cnt, float& float_cnt) {
  // edm4hep::CalorimeterHits
  auto* calohit_coll = new edm4hep::CalorimeterHitCollection();

  for (int i = 0; i < num_elements; ++i) {
    auto elem = calohit_coll->create();

    elem.setCellID(int_cnt++);
    elem.setEnergy(float_cnt++);
    elem.setEnergyError(float_cnt++);
    elem.setTime(float_cnt++);
    edm4hep::Vector3f test_vec{float_cnt++, float_cnt++, float_cnt++};
    elem.setPosition(test_vec);
    elem.setType(int_cnt++);
  }

  auto* calohit_handle =
      dynamic_cast<DataHandle<edm4hep::CalorimeterHitCollection>*>(m_dataHandlesMap[m_e4h_calohit_name]);
  calohit_handle->put(calohit_coll);
}

void TestE4H2L::createRawCalorimeterHits(const int num_elements, int& int_cnt, float& float_cnt) {
  auto* rawcalohit_coll = new edm4hep::RawCalorimeterHitCollection();

  for (int i = 0; i < num_elements; ++i) {
    auto elem = rawcalohit_coll->create();

    elem.setCellID(int_cnt++);
    elem.setAmplitude(int_cnt++);
    elem.setTimeStamp(int_cnt++);
  }

  auto* rawcalohit_handle =
      dynamic_cast<DataHandle<edm4hep::RawCalorimeterHitCollection>*>(m_dataHandlesMap[m_e4h_rawcalohit_name]);
  rawcalohit_handle->put(rawcalohit_coll);
}

void TestE4H2L::createSimCalorimeterHits(const int num_elements, const int num_contributions,
                                         const std::vector<std::tuple<uint, uint, uint>>& link_mcparticles_idx,
                                         int& int_cnt, float& float_cnt) {
  auto* simcalohit_coll = new edm4hep::SimCalorimeterHitCollection();

  // Get MCParticles handle
  DataHandle<edm4hep::MCParticleCollection> mcparticles_handle{m_e4h_mcparticle_name, Gaudi::DataHandle::Reader, this};
  const auto                                mcparticles_coll = mcparticles_handle.get();

  for (int i = 0; i < num_elements; ++i) {
    // auto* elem = new edm4hep::SimCalorimeterHit();
    auto elem = simcalohit_coll->create();

    elem.setCellID(int_cnt++);
    elem.setEnergy(float_cnt++);
    edm4hep::Vector3f test_vec{float_cnt++, float_cnt++, float_cnt++};
    elem.setPosition(test_vec);

    for (int j = 0; j < num_contributions; j++) {
      auto* contrib = new edm4hep::MutableCaloHitContribution();
      contrib->setPDG(int_cnt++);
      contrib->setEnergy(float_cnt++);
      contrib->setTime(float_cnt++);
      edm4hep::Vector3f test_step_vec{float_cnt++, float_cnt++, float_cnt++};
      contrib->setStepPosition(test_step_vec);

      // add the corresponding mcparticle
      for (const auto& [simch_idx, contrib_idx, mcpart_idx] : link_mcparticles_idx) {
        if (i == simch_idx && j == contrib_idx) {
          contrib->setParticle(mcparticles_coll->at(mcpart_idx));
        }
      }

      elem.addToContributions(*contrib);
    }
  }

  auto* simcalohit_handle =
      dynamic_cast<DataHandle<edm4hep::SimCalorimeterHitCollection>*>(m_dataHandlesMap[m_e4h_simcalohit_name]);
  simcalohit_handle->put(simcalohit_coll);
}

void TestE4H2L::createTPCHits(const int num_elements, const int num_rawwords, int& int_cnt, float& float_cnt) {
  // edm4hep::CalorimeterHits
  auto* tpchit_coll = new edm4hep::TPCHitCollection();

  for (int i = 0; i < num_elements; ++i) {
    auto elem = tpchit_coll->create();

    elem.setCellID(int_cnt++);
    elem.setQuality(int_cnt++);
    elem.setTime(float_cnt++);
    elem.setCharge(float_cnt++);

    for (int j = 0; j < num_rawwords; ++j) {
      elem.addToRawDataWords(int_cnt++);
    }
  }

  auto* tpchit_handle = dynamic_cast<DataHandle<edm4hep::TPCHitCollection>*>(m_dataHandlesMap[m_e4h_tpchit_name]);
  tpchit_handle->put(tpchit_coll);
}

void TestE4H2L::createTrackerHits(const int num_elements, int& int_cnt, float& float_cnt) {
  auto trackerhit_coll = new edm4hep::TrackerHitCollection();

  for (int i = 0; i < num_elements; ++i) {
    auto elem = trackerhit_coll->create();

    elem.setCellID(int_cnt++);
    elem.setType(int_cnt++);
    elem.setQuality(int_cnt++);
    elem.setTime(float_cnt++);
    elem.setEDep(float_cnt++);
    elem.setEDepError(float_cnt++);
    elem.setEdx(float_cnt++);
    edm4hep::Vector3d test_vec{float_cnt++, float_cnt++, float_cnt++};
    elem.setPosition(test_vec);
    std::array<float, 6> test_array{float_cnt++, float_cnt++, float_cnt++, float_cnt++, float_cnt++, float_cnt++};
    elem.setCovMatrix(test_array);
  }

  auto* trackerhit_handle =
      dynamic_cast<DataHandle<edm4hep::TrackerHitCollection>*>(m_dataHandlesMap[m_e4h_trackerhit_name]);
  trackerhit_handle->put(trackerhit_coll);
}

void TestE4H2L::createSimTrackerHits(const int                                 num_elements,
                                     const std::vector<std::pair<uint, uint>>& link_mcparticles_idx, int& int_cnt,
                                     float& float_cnt) {
  // Get MCParticles handle
  DataHandle<edm4hep::MCParticleCollection> mcparticles_handle{m_e4h_mcparticle_name, Gaudi::DataHandle::Reader, this};
  const auto                                mcparticles_coll = mcparticles_handle.get();

  auto simtrackerhit_coll = new edm4hep::SimTrackerHitCollection();

  for (int i = 0; i < num_elements; ++i) {
    auto elem = simtrackerhit_coll->create();

    elem.setCellID(int_cnt++);
    elem.setEDep(float_cnt++);
    elem.setTime(float_cnt++);
    elem.setPathLength(float_cnt++);
    elem.setQuality(int_cnt++);
    edm4hep::Vector3d test_vec{float_cnt++, float_cnt++, float_cnt++};
    elem.setPosition(test_vec);
    edm4hep::Vector3f test_vec_mom{float_cnt++, float_cnt++, float_cnt++};
    elem.setMomentum(test_vec_mom);

    elem.setOverlay(true);
    elem.setProducedBySecondary(false);
  }

  // Connect single mcparticles to SimTrackerHits
  for (const auto& [orig_idx, link_idx] : link_mcparticles_idx) {
    simtrackerhit_coll->at(orig_idx).setMCParticle(mcparticles_coll->at(link_idx));
  }

  auto* simtrackerhit_handle =
      dynamic_cast<DataHandle<edm4hep::SimTrackerHitCollection>*>(m_dataHandlesMap[m_e4h_simtrackerhit_name]);
  simtrackerhit_handle->put(simtrackerhit_coll);
}

void TestE4H2L::createTracks(const int num_elements, const int subdetectorhitnumbers,
                             const std::vector<uint>& link_trackerhits_idx, const int num_track_states,
                             const std::vector<std::pair<uint, uint>>& track_link_tracks_idx, int& int_cnt,
                             float& float_cnt) {
  // edm4hep::Track
  auto track_coll = new edm4hep::TrackCollection();

  for (int i = 0; i < num_elements; ++i) {
    auto elem = track_coll->create();

    elem.setType(2);  // TODO specific type
    elem.setChi2(float_cnt++);
    elem.setNdf(int_cnt++);
    elem.setDEdx(float_cnt++);
    elem.setDEdxError(float_cnt++);
    elem.setRadiusOfInnermostHit(float_cnt++);

    for (int j = 0; j < subdetectorhitnumbers; ++j) {
      elem.addToSubDetectorHitNumbers(int_cnt++);
    }

    DataHandle<edm4hep::TrackerHitCollection> trackerhits_handle{m_e4h_trackerhit_name, Gaudi::DataHandle::Reader,
                                                                 this};
    const auto                                trackerhits_coll = trackerhits_handle.get();

    for (auto& idx : link_trackerhits_idx) {
      elem.addToTrackerHits((*trackerhits_coll)[idx]);
    }

    for (int j = 0; j < num_track_states; ++j) {
      edm4hep::TrackState trackstate;

      trackstate.location  = int_cnt++;
      trackstate.D0        = float_cnt++;
      trackstate.phi       = float_cnt++;
      trackstate.omega     = float_cnt++;
      trackstate.Z0        = float_cnt++;
      trackstate.tanLambda = float_cnt++;
      edm4hep::Vector3f test_vec{float_cnt++, float_cnt++, float_cnt++};
      trackstate.referencePoint        = test_vec;
      std::array<float, 15> test_array = {float_cnt++, float_cnt++, float_cnt++, float_cnt++, float_cnt++,
                                          float_cnt++, float_cnt++, float_cnt++, float_cnt++, float_cnt++,
                                          float_cnt++, float_cnt++, float_cnt++, float_cnt++, float_cnt++};
      trackstate.covMatrix             = test_array;

      elem.addToTrackStates(trackstate);
    }
  }

  // Connect tracks between them
  for (const auto& [orig_idx, link_idx] : track_link_tracks_idx) {
    track_coll->at(orig_idx).addToTracks(track_coll->at(link_idx));
  }

  // Save track collection with DataHandle
  auto* track_handle = dynamic_cast<DataHandle<edm4hep::TrackCollection>*>(m_dataHandlesMap[m_e4h_track_name]);
  track_handle->put(track_coll);
}

void TestE4H2L::createMCParticles(const int num_elements, const std::vector<std::pair<uint, uint>>& mcp_parents_idx,
                                  int& int_cnt, float& float_cnt) {
  // edm4hep::MCPartcile
  auto mcparticle_coll = new edm4hep::MCParticleCollection();

  for (int i = 0; i < num_elements; ++i) {
    auto elem = mcparticle_coll->create();

    elem.setPDG(int_cnt++);
    elem.setGeneratorStatus(int_cnt++);
    // elem->setSimulatorStatus(int_cnt++);
    edm4hep::Vector3d vertex_vec{float_cnt++, float_cnt++, float_cnt++};
    elem.setVertex(vertex_vec);
    elem.setTime(float_cnt++);
    edm4hep::Vector3d endpoint_vec{float_cnt++, float_cnt++, float_cnt++};
    elem.setEndpoint(endpoint_vec);
    edm4hep::Vector3f momentum_vec{float_cnt++, float_cnt++, float_cnt++};
    elem.setMomentum(momentum_vec);
    edm4hep::Vector3f momentumatendpoint_vec{float_cnt++, float_cnt++, float_cnt++};
    elem.setMomentumAtEndpoint(momentumatendpoint_vec);
    elem.setMass(float_cnt++);
    elem.setCharge(float_cnt++);
    edm4hep::Vector3f spin_vec{float_cnt++, float_cnt++, float_cnt++};
    elem.setSpin(spin_vec);
    edm4hep::Vector2i colorflow_vec{int_cnt++, int_cnt++};
    elem.setColorFlow(colorflow_vec);

    elem.setCreatedInSimulation(1);
    elem.setBackscatter(0);
    elem.setVertexIsNotEndpointOfParent(1);
    elem.setDecayedInTracker(0);
    elem.setDecayedInCalorimeter(1);
    elem.setHasLeftDetector(0);
    elem.setStopped(1);
    elem.setOverlay(0);
  }

  // Connect mcparticles between them with parent relationship
  for (const auto& [orig_idx, link_idx] : mcp_parents_idx) {
    mcparticle_coll->at(orig_idx).addToParents(mcparticle_coll->at(link_idx));
  }

  // Save mcparticle collection with DataHandle
  auto* mcparticle_handle =
      dynamic_cast<DataHandle<edm4hep::MCParticleCollection>*>(m_dataHandlesMap[m_e4h_mcparticle_name]);
  mcparticle_handle->put(mcparticle_coll);
}

bool TestE4H2L::checkEDMRawCaloHitLCIORawCaloHit(lcio::LCEventImpl* the_event) {
  DataHandle<edm4hep::RawCalorimeterHitCollection> rawcalohit_handle_orig{m_e4h_rawcalohit_name,
                                                                          Gaudi::DataHandle::Reader, this};
  const auto                                       rawcalohit_coll_orig = rawcalohit_handle_orig.get();

  auto lcio_rawcalohit_coll = the_event->getCollection(m_lcio_rawcalohit_name);
  auto lcio_coll_size       = lcio_rawcalohit_coll->getNumberOfElements();

  bool rawcalohit_same = (*rawcalohit_coll_orig).size() == lcio_coll_size;

  if (rawcalohit_same) {
    for (int i = 0; i < (*rawcalohit_coll_orig).size(); ++i) {
      auto  edm_rawcalohit_orig = (*rawcalohit_coll_orig)[i];
      auto* lcio_rawcalohit     = dynamic_cast<lcio::RawCalorimeterHitImpl*>(lcio_rawcalohit_coll->getElementAt(i));

      if (lcio_rawcalohit != nullptr) {
        uint64_t  combined_value     = 0;
        uint32_t* combined_value_ptr = reinterpret_cast<uint32_t*>(&combined_value);
        combined_value_ptr[0]        = lcio_rawcalohit->getCellID0();
        combined_value_ptr[1]        = lcio_rawcalohit->getCellID1();
        rawcalohit_same              = rawcalohit_same && (edm_rawcalohit_orig.getCellID() == combined_value);
        rawcalohit_same = rawcalohit_same && (edm_rawcalohit_orig.getAmplitude() == lcio_rawcalohit->getAmplitude());
        rawcalohit_same = rawcalohit_same && (edm_rawcalohit_orig.getTimeStamp() == lcio_rawcalohit->getTimeStamp());
      }
    }
  }

  if (!rawcalohit_same) {
    error() << "RawCalorimeterHit EDM4hep -> LCIO failed." << endmsg;
  }

  return rawcalohit_same;
}

bool TestE4H2L::checkEDMSimCaloHitLCIOSimCaloHit(
    lcio::LCEventImpl* the_event, const std::vector<std::tuple<uint, uint, uint>>& link_mcparticles_idx) {
  DataHandle<edm4hep::SimCalorimeterHitCollection> simcalohit_handle_orig{m_e4h_simcalohit_name,
                                                                          Gaudi::DataHandle::Reader, this};
  const auto                                       simcalohit_coll_orig = simcalohit_handle_orig.get();

  auto lcio_simcalohit_coll = the_event->getCollection(m_lcio_simcalohit_name);
  auto lcio_coll_size       = lcio_simcalohit_coll->getNumberOfElements();

  bool simcalohit_same = (*simcalohit_coll_orig).size() == lcio_coll_size;

  if (simcalohit_same) {
    for (int i = 0; i < (*simcalohit_coll_orig).size(); ++i) {
      auto  edm_simcalohit_orig = (*simcalohit_coll_orig)[i];
      auto* lcio_simcalohit     = dynamic_cast<lcio::SimCalorimeterHitImpl*>(lcio_simcalohit_coll->getElementAt(i));

      if (lcio_simcalohit != nullptr) {
        uint64_t  combined_value     = 0;
        uint32_t* combined_value_ptr = reinterpret_cast<uint32_t*>(&combined_value);
        combined_value_ptr[0]        = lcio_simcalohit->getCellID0();
        combined_value_ptr[1]        = lcio_simcalohit->getCellID1();
        simcalohit_same              = simcalohit_same && (edm_simcalohit_orig.getCellID() == combined_value);

        // simcalohit_same = simcalohit_same && (edm_simcalohit_orig.getEnergy()
        // == lcio_simcalohit->getEnergy());
        for (int k = 0; k < 3; ++k) {
          simcalohit_same =
              simcalohit_same && (edm_simcalohit_orig.getPosition()[k] == lcio_simcalohit->getPosition()[k]);
        }

        auto* lcio_mcparticles_coll = the_event->getCollection(m_lcio_mcparticle_name);

        simcalohit_same =
            simcalohit_same && (edm_simcalohit_orig.contributions_size() == lcio_simcalohit->getNMCContributions());
        if (simcalohit_same) {
          for (int j = 0; j < edm_simcalohit_orig.contributions_size(); ++j) {
            auto edm_calohitcontr = edm_simcalohit_orig.getContributions(j);

            simcalohit_same = simcalohit_same && (edm_calohitcontr.getPDG() == lcio_simcalohit->getPDGCont(j));
            simcalohit_same = simcalohit_same && (edm_calohitcontr.getEnergy() == lcio_simcalohit->getEnergyCont(j));
            simcalohit_same = simcalohit_same && (edm_calohitcontr.getTime() == lcio_simcalohit->getTimeCont(j));
            simcalohit_same = simcalohit_same && (edm_calohitcontr.getTime() == lcio_simcalohit->getTimeCont(j));
            for (int k = 0; k < 3; ++k) {
              simcalohit_same =
                  simcalohit_same && (edm_calohitcontr.getStepPosition()[k] == lcio_simcalohit->getStepPosition(j)[k]);
            }
          }
        }
      }
    }

    // Check MCParticles linked to Contributions in SimCaloHits
    // SimCaloHits have all the same number of contributions during creation for
    // this test
    // The contribution idx can be used directly then (instead of calculation
    // based on how many were added)
    auto lcio_mcparticles_coll = the_event->getCollection(m_lcio_mcparticle_name);
    for (const auto& [simch_idx, contrib_idx, mcpart_idx] : link_mcparticles_idx) {
      auto* lcio_simcalohit_orig =
          dynamic_cast<lcio::SimCalorimeterHitImpl*>(lcio_simcalohit_coll->getElementAt(simch_idx));
      auto* lcio_mcpart_orig = lcio_simcalohit_orig->getParticleCont(contrib_idx);
      auto* lcio_mcpart_link = dynamic_cast<lcio::MCParticleImpl*>(lcio_mcparticles_coll->getElementAt(mcpart_idx));

      simcalohit_same = simcalohit_same && (lcio_mcpart_orig == lcio_mcpart_link);
    }
  }

  if (!simcalohit_same) {
    error() << "SimCalorimeterHit EDM4hep -> LCIO failed." << endmsg;
  }

  return simcalohit_same;
}

bool TestE4H2L::checkEDMTPCHitLCIOTPCHit(lcio::LCEventImpl* the_event) {
  DataHandle<edm4hep::TPCHitCollection> tpchit_handle_orig{m_e4h_tpchit_name, Gaudi::DataHandle::Reader, this};
  const auto                            tpchit_coll_orig = tpchit_handle_orig.get();

  auto lcio_tpchit_coll = the_event->getCollection(m_lcio_tpchit_name);
  auto lcio_coll_size   = lcio_tpchit_coll->getNumberOfElements();

  bool tpchit_same = (*tpchit_coll_orig).size() == lcio_coll_size;

  if (tpchit_same) {
    for (int i = 0; i < (*tpchit_coll_orig).size(); ++i) {
      auto  edm_tpchit_orig = (*tpchit_coll_orig)[i];
      auto* lcio_tpchit     = dynamic_cast<lcio::TPCHitImpl*>(lcio_tpchit_coll->getElementAt(i));

      if (lcio_tpchit != nullptr) {
        tpchit_same = tpchit_same && (edm_tpchit_orig.getCellID() == lcio_tpchit->getCellID());
        tpchit_same = tpchit_same && (edm_tpchit_orig.getTime() == lcio_tpchit->getTime());
        tpchit_same = tpchit_same && (edm_tpchit_orig.getCharge() == lcio_tpchit->getCharge());
        tpchit_same = tpchit_same && (edm_tpchit_orig.getQuality() == lcio_tpchit->getQuality());

        tpchit_same = tpchit_same && (edm_tpchit_orig.rawDataWords_size() == lcio_tpchit->getNRawDataWords());
        if (tpchit_same) {
          for (int j = 0; j < lcio_tpchit->getNRawDataWords(); ++j) {
            tpchit_same = tpchit_same && (edm_tpchit_orig.getRawDataWords(j) == lcio_tpchit->getRawDataWord(j));
          }
        }
      }
    }
  }

  if (!tpchit_same) {
    error() << "TPCHit EDM4hep -> LCIO failed." << endmsg;
  }

  return tpchit_same;
}

bool TestE4H2L::checkEDMSimTrackerHitEDMSimTrackerHit(const std::vector<std::pair<uint, uint>>& link_mcparticles_idx) {
  DataHandle<edm4hep::SimTrackerHitCollection> simtrackerhit_handle_orig{m_e4h_simtrackerhit_name,
                                                                         Gaudi::DataHandle::Reader, this};
  const auto                                   simtrackerhit_coll_orig = simtrackerhit_handle_orig.get();

  DataHandle<edm4hep::SimTrackerHitCollection> simtrackerhit_handle{m_e4h_simtrackerhit_name + m_conv_tag,
                                                                    Gaudi::DataHandle::Reader, this};
  const auto                                   simtrackerhit_coll = simtrackerhit_handle.get();

  bool strh_same = (*simtrackerhit_coll_orig).size() == (*simtrackerhit_coll).size();

  if (strh_same) {
    for (int i = 0; i < (*simtrackerhit_coll).size(); ++i) {
      auto edm_strh_orig = (*simtrackerhit_coll_orig)[i];
      auto edm_strh      = (*simtrackerhit_coll)[i];

      strh_same = strh_same && (edm_strh_orig.getCellID() == edm_strh.getCellID());
      for (int k = 0; k < 3; ++k) {
        strh_same = strh_same && (edm_strh_orig.getPosition()[k] == edm_strh.getPosition()[k]);
      }
      strh_same = strh_same && (edm_strh_orig.getEDep() == edm_strh.getEDep());
      strh_same = strh_same && (edm_strh_orig.getTime() == edm_strh.getTime());
      for (int k = 0; k < 3; ++k) {
        strh_same = strh_same && (edm_strh_orig.getMomentum()[k] == edm_strh.getMomentum()[k]);
      }
      strh_same = strh_same && (edm_strh_orig.getPathLength() == edm_strh.getPathLength());
      strh_same = strh_same && (edm_strh_orig.getQuality() == edm_strh.getQuality());
      strh_same = strh_same && (edm_strh_orig.isOverlay() == edm_strh.isOverlay());
      strh_same = strh_same && (edm_strh_orig.isProducedBySecondary() == edm_strh.isProducedBySecondary());
    }

    if (strh_same) {
      // Check MCParticles linked to SimTrackerHits
      DataHandle<edm4hep::MCParticleCollection> mcparticle_handle{m_e4h_mcparticle_name + m_conv_tag,
                                                                  Gaudi::DataHandle::Reader, this};
      const auto                                mcparticle_coll = mcparticle_handle.get();

      for (const auto& [strh_idx, mcp_idx] : link_mcparticles_idx) {
        auto edm_strh        = (*simtrackerhit_coll)[strh_idx];
        auto edm_mcpart      = (*mcparticle_coll)[mcp_idx];
        auto edm_strh_mcpart = edm_strh.getMCParticle();

        strh_same = strh_same && (edm_strh_mcpart == edm_mcpart);
      }
    }
  }

  if (!strh_same) {
    error() << "SimTrackerHit EDM4hep -> LCIO -> EDM4hep failed." << endmsg;
  }

  return strh_same;
}

bool TestE4H2L::checkEDMSimTrackerHitLCIOSimTrackerHit(lcio::LCEventImpl*                        the_event,
                                                       const std::vector<std::pair<uint, uint>>& link_mcparticles_idx) {
  DataHandle<edm4hep::SimTrackerHitCollection> simtrackerhit_handle_orig{m_e4h_simtrackerhit_name,
                                                                         Gaudi::DataHandle::Reader, this};
  const auto                                   simtrackerhit_coll_orig = simtrackerhit_handle_orig.get();

  auto lcio_simtrackerhit_coll = the_event->getCollection(m_lcio_simtrackerhit_name);
  auto lcio_coll_size          = lcio_simtrackerhit_coll->getNumberOfElements();

  bool strh_same = (*simtrackerhit_coll_orig).size() == lcio_coll_size;

  if (strh_same) {
    for (int i = 0; i < (*simtrackerhit_coll_orig).size(); ++i) {
      auto  edm_strh_orig = (*simtrackerhit_coll_orig)[i];
      auto* lcio_strh     = dynamic_cast<lcio::SimTrackerHitImpl*>(lcio_simtrackerhit_coll->getElementAt(i));

      if (lcio_strh != nullptr) {
        // TODO handle split CellIDs
        uint64_t  combined_value     = 0;
        uint32_t* combined_value_ptr = reinterpret_cast<uint32_t*>(&combined_value);
        combined_value_ptr[0]        = lcio_strh->getCellID0();
        combined_value_ptr[1]        = lcio_strh->getCellID1();
        strh_same                    = strh_same && (edm_strh_orig.getCellID() == combined_value);

        strh_same = strh_same && (edm_strh_orig.getPosition()[0] == lcio_strh->getPosition()[0]);
        strh_same = strh_same && (edm_strh_orig.getPosition()[1] == lcio_strh->getPosition()[1]);
        strh_same = strh_same && (edm_strh_orig.getPosition()[2] == lcio_strh->getPosition()[2]);

        strh_same = strh_same && (edm_strh_orig.getEDep() == lcio_strh->getEDep());
        strh_same = strh_same && (edm_strh_orig.getTime() == lcio_strh->getTime());

        strh_same = strh_same && (edm_strh_orig.getMomentum()[0] == lcio_strh->getMomentum()[0]);
        strh_same = strh_same && (edm_strh_orig.getMomentum()[1] == lcio_strh->getMomentum()[1]);
        strh_same = strh_same && (edm_strh_orig.getMomentum()[2] == lcio_strh->getMomentum()[2]);

        strh_same = strh_same && (edm_strh_orig.getPathLength() == lcio_strh->getPathLength());
        strh_same = strh_same && (edm_strh_orig.getQuality() == lcio_strh->getQuality());
        strh_same = strh_same && (edm_strh_orig.isOverlay() == lcio_strh->isOverlay());
        strh_same = strh_same && (edm_strh_orig.isProducedBySecondary() == lcio_strh->isProducedBySecondary());
      }
    }

    if (strh_same) {
      auto* lcio_mcparticle_coll = the_event->getCollection(m_lcio_mcparticle_name);
      for (const auto& [strh_idx, mcp_idx] : link_mcparticles_idx) {
        auto* lcio_strh     = dynamic_cast<lcio::SimTrackerHitImpl*>(lcio_simtrackerhit_coll->getElementAt(strh_idx));
        auto  lcio_strh_mcp = lcio_strh->getMCParticle();

        auto* lcio_mcp_link = dynamic_cast<lcio::MCParticleImpl*>(lcio_mcparticle_coll->getElementAt(mcp_idx));

        strh_same = strh_same && (lcio_strh_mcp == lcio_mcp_link);
      }
    }
  }

  if (!strh_same) {
    error() << "SimTrackerHit EDM4hep -> LCIO failed." << endmsg;
  }

  return strh_same;
}

bool TestE4H2L::checkEDMTrackerHitLCIOTrackerHit(lcio::LCEventImpl* the_event) {
  DataHandle<edm4hep::TrackerHitCollection> trackerhit_handle_orig{m_e4h_trackerhit_name, Gaudi::DataHandle::Reader,
                                                                   this};
  const auto                                trackerhit_coll_orig = trackerhit_handle_orig.get();

  auto lcio_trackerhit_coll = the_event->getCollection(m_lcio_trackerhit_name);
  auto lcio_coll_size       = lcio_trackerhit_coll->getNumberOfElements();

  bool trackerhit_same = (*trackerhit_coll_orig).size() == lcio_coll_size;

  if (trackerhit_same) {
    for (int i = 0; i < (*trackerhit_coll_orig).size(); ++i) {
      auto  edm_trackerhit_orig = (*trackerhit_coll_orig)[i];
      auto* lcio_trackerhit     = dynamic_cast<lcio::TrackerHitImpl*>(lcio_trackerhit_coll->getElementAt(i));

      if (lcio_trackerhit != nullptr) {
        // TODO handle split CellIDs
        uint64_t  combined_value     = 0;
        uint32_t* combined_value_ptr = reinterpret_cast<uint32_t*>(&combined_value);
        combined_value_ptr[0]        = lcio_trackerhit->getCellID0();
        combined_value_ptr[1]        = lcio_trackerhit->getCellID1();
        trackerhit_same              = trackerhit_same && (edm_trackerhit_orig.getCellID() == combined_value);

        trackerhit_same = trackerhit_same && (edm_trackerhit_orig.getType() == lcio_trackerhit->getType());
        trackerhit_same = trackerhit_same && (edm_trackerhit_orig.getQuality() == lcio_trackerhit->getQuality());
        trackerhit_same = trackerhit_same && (edm_trackerhit_orig.getTime() == lcio_trackerhit->getTime());
        trackerhit_same = trackerhit_same && (edm_trackerhit_orig.getEDep() == lcio_trackerhit->getEDep());
        trackerhit_same = trackerhit_same && (edm_trackerhit_orig.getEDepError() == lcio_trackerhit->getEDepError());
        for (int k = 0; k < 3; ++k) {
          trackerhit_same =
              trackerhit_same && (edm_trackerhit_orig.getPosition()[k] == lcio_trackerhit->getPosition()[k]);
        }
        for (int k = 0; k < lcio_trackerhit->getCovMatrix().size(); ++k) {
          trackerhit_same =
              trackerhit_same && (edm_trackerhit_orig.getCovMatrix(k) == lcio_trackerhit->getCovMatrix()[k]);
        }

        // TODO Raw hits
      }
    }
  }

  if (!trackerhit_same) {
    error() << "Track EDM4hep -> LCIO failed." << endmsg;
  }

  return trackerhit_same;
}

bool TestE4H2L::checkEDMTrackLCIOTrack(lcio::LCEventImpl* the_event, const std::vector<uint>& link_trackerhits_idx,
                                       const std::vector<std::pair<uint, uint>>& track_link_tracks_idx) {
  DataHandle<edm4hep::TrackCollection> track_handle_orig{m_e4h_track_name, Gaudi::DataHandle::Reader, this};
  const auto                           track_coll_orig = track_handle_orig.get();

  auto lcio_track_coll = the_event->getCollection(m_lcio_track_name);
  auto lcio_coll_size  = lcio_track_coll->getNumberOfElements();

  bool track_same = (*track_coll_orig).size() == lcio_coll_size;

  if (track_same) {
    for (int i = 0; i < (*track_coll_orig).size(); ++i) {
      auto  edm_track_orig = (*track_coll_orig)[i];
      auto* lcio_track     = dynamic_cast<lcio::TrackImpl*>(lcio_track_coll->getElementAt(i));

      // TODO review EDM4hep to LCIO Track conversion for setTypeBit
      // track_same = track_same && (edm_track_orig.getType() ==
      // lcio_track->getType());

      track_same = track_same && (edm_track_orig.getChi2() == lcio_track->getChi2());
      track_same = track_same && (edm_track_orig.getNdf() == lcio_track->getNdf());
      track_same = track_same && (edm_track_orig.getDEdx() == lcio_track->getdEdx());
      track_same = track_same && (edm_track_orig.getDEdxError() == lcio_track->getdEdxError());
      track_same = track_same && (edm_track_orig.getRadiusOfInnermostHit() == lcio_track->getRadiusOfInnermostHit());

      std::vector<edm4hep::TrackerHit>   edm_trackerhits;
      std::vector<lcio::TrackerHitImpl*> lcio_trackerhits;

      track_same = track_same && (edm_track_orig.trackerHits_size() == lcio_track->getTrackerHits().size());
      if (track_same) {
        // Check linked trackerhits connections
        if (lcio_track->getTrackerHits().size() >= link_trackerhits_idx.size()) {
          auto* lcio_trackerhit_coll = the_event->getCollection(m_lcio_trackerhit_name);
          // Check the trackerhits in this track with the trackerhits from the
          // event to be the same
          for (int j = 0; j < link_trackerhits_idx.size(); ++j) {
            track_same =
                track_same &&
                (dynamic_cast<lcio::TrackerHitImpl*>(lcio_track->getTrackerHits()[j]) ==
                 dynamic_cast<lcio::TrackerHitImpl*>(lcio_trackerhit_coll->getElementAt(link_trackerhits_idx[j])));
          }
        }
      }

      // TODO Resizing in EDM4hep to LCIO conversion causes to "have" 50 hits
      if (lcio_track->getSubdetectorHitNumbers().size() == 50) {
        for (int j = 0; j < edm_track_orig.subDetectorHitNumbers_size(); ++j) {
          track_same =
              track_same && (edm_track_orig.getSubDetectorHitNumbers(j) == lcio_track->getSubdetectorHitNumbers()[j]);
        }
        for (int j = edm_track_orig.subDetectorHitNumbers_size(); j < 50; ++j) {
          track_same = track_same && (0 == lcio_track->getSubdetectorHitNumbers()[j]);
        }
      } else {
        track_same = track_same &&
                     (edm_track_orig.subDetectorHitNumbers_size() == lcio_track->getSubdetectorHitNumbers().size());
        if (track_same) {
          for (int j = 0; j < edm_track_orig.subDetectorHitNumbers_size(); ++j) {
            track_same =
                track_same && (edm_track_orig.getSubDetectorHitNumbers(j) == lcio_track->getSubdetectorHitNumbers()[j]);
          }
        }
      }

      track_same = track_same && (edm_track_orig.trackStates_size() == lcio_track->getTrackStates().size());
      if ((edm_track_orig.trackStates_size() == lcio_track->getTrackStates().size())) {
        for (int j = 0; j < edm_track_orig.trackStates_size(); ++j) {
          auto edm_trackestate_orig = edm_track_orig.getTrackStates(j);
          auto lcio_trackestate     = lcio_track->getTrackStates()[j];

          track_same = track_same && (edm_trackestate_orig.location == lcio_trackestate->getLocation());
          track_same = track_same && (edm_trackestate_orig.D0 == lcio_trackestate->getD0());
          track_same = track_same && (edm_trackestate_orig.phi == lcio_trackestate->getPhi());
          track_same = track_same && (edm_trackestate_orig.omega == lcio_trackestate->getOmega());
          track_same = track_same && (edm_trackestate_orig.Z0 == lcio_trackestate->getZ0());
          track_same = track_same && (edm_trackestate_orig.tanLambda == lcio_trackestate->getTanLambda());
          for (int k = 0; k < 3; ++k) {
            track_same =
                track_same && (edm_trackestate_orig.referencePoint[k] == lcio_trackestate->getReferencePoint()[k]);
          }
          // Check same size cov matrix
          track_same = track_same && (edm_trackestate_orig.covMatrix.size() == lcio_trackestate->getCovMatrix().size());
          if (edm_trackestate_orig.covMatrix.size() == lcio_trackestate->getCovMatrix().size()) {
            for (int k = 0; k < edm_trackestate_orig.covMatrix.size(); ++k) {
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
      track_same            = track_same && (lcio_track_orig->getTracks()[appeared[orig_idx]] == lcio_track_link);
      appeared[orig_idx]++;
    }
  }

  if (!track_same) {
    error() << "Track EDM4hep -> LCIO failed." << endmsg;
  }

  return track_same;
}

bool TestE4H2L::checkEDMMCParticleLCIOMCParticle(lcio::LCEventImpl*                        the_event,
                                                 const std::vector<std::pair<uint, uint>>& mcp_parents_idx) {
  DataHandle<edm4hep::MCParticleCollection> mcparticle_handle_orig{m_e4h_mcparticle_name, Gaudi::DataHandle::Reader,
                                                                   this};
  const auto                                mcparticle_coll_orig = mcparticle_handle_orig.get();

  auto lcio_mcparticle_coll = the_event->getCollection(m_lcio_mcparticle_name);
  auto lcio_coll_size       = lcio_mcparticle_coll->getNumberOfElements();

  bool mcp_same = (*mcparticle_coll_orig).size() == lcio_coll_size;

  if (mcp_same) {
    for (int i = 0; i < (*mcparticle_coll_orig).size(); ++i) {
      auto  edm_mcp_orig = (*mcparticle_coll_orig)[i];
      auto* lcio_mcp     = dynamic_cast<lcio::MCParticleImpl*>(lcio_mcparticle_coll->getElementAt(i));

      mcp_same = mcp_same && (edm_mcp_orig.getPDG() == lcio_mcp->getPDG());
      mcp_same = mcp_same && (edm_mcp_orig.getGeneratorStatus() == lcio_mcp->getGeneratorStatus());
      // mcp_same = mcp_same && (edm_mcp_orig.getSimulatorStatus() ==
      // lcio_mcp->getSimulatorStatus());

      mcp_same = mcp_same && (edm_mcp_orig.getVertex()[0] == lcio_mcp->getVertex()[0]);
      mcp_same = mcp_same && (edm_mcp_orig.getVertex()[1] == lcio_mcp->getVertex()[1]);
      mcp_same = mcp_same && (edm_mcp_orig.getVertex()[2] == lcio_mcp->getVertex()[2]);

      mcp_same = mcp_same && (edm_mcp_orig.getTime() == lcio_mcp->getTime());

      mcp_same = mcp_same && (edm_mcp_orig.getEndpoint()[0] == lcio_mcp->getEndpoint()[0]);
      mcp_same = mcp_same && (edm_mcp_orig.getEndpoint()[1] == lcio_mcp->getEndpoint()[1]);
      mcp_same = mcp_same && (edm_mcp_orig.getEndpoint()[2] == lcio_mcp->getEndpoint()[2]);

      mcp_same = mcp_same && (edm_mcp_orig.getMomentum()[0] == lcio_mcp->getMomentum()[0]);
      mcp_same = mcp_same && (edm_mcp_orig.getMomentum()[1] == lcio_mcp->getMomentum()[1]);
      mcp_same = mcp_same && (edm_mcp_orig.getMomentum()[2] == lcio_mcp->getMomentum()[2]);

      mcp_same = mcp_same && (edm_mcp_orig.getMomentumAtEndpoint()[0] == lcio_mcp->getMomentumAtEndpoint()[0]);
      mcp_same = mcp_same && (edm_mcp_orig.getMomentumAtEndpoint()[1] == lcio_mcp->getMomentumAtEndpoint()[1]);
      mcp_same = mcp_same && (edm_mcp_orig.getMomentumAtEndpoint()[2] == lcio_mcp->getMomentumAtEndpoint()[2]);

      mcp_same = mcp_same && (edm_mcp_orig.getMass() == lcio_mcp->getMass());
      mcp_same = mcp_same && (edm_mcp_orig.getCharge() == lcio_mcp->getCharge());

      mcp_same = mcp_same && (edm_mcp_orig.getSpin()[0] == lcio_mcp->getSpin()[0]);
      mcp_same = mcp_same && (edm_mcp_orig.getSpin()[1] == lcio_mcp->getSpin()[1]);
      mcp_same = mcp_same && (edm_mcp_orig.getSpin()[2] == lcio_mcp->getSpin()[2]);

      mcp_same = mcp_same && (edm_mcp_orig.getColorFlow()[0] == lcio_mcp->getColorFlow()[0]);
      mcp_same = mcp_same && (edm_mcp_orig.getColorFlow()[1] == lcio_mcp->getColorFlow()[1]);

      mcp_same = mcp_same && (edm_mcp_orig.isCreatedInSimulation() == lcio_mcp->isCreatedInSimulation());
      mcp_same = mcp_same && (edm_mcp_orig.isBackscatter() == lcio_mcp->isBackscatter());
      mcp_same = mcp_same && (edm_mcp_orig.vertexIsNotEndpointOfParent() == lcio_mcp->vertexIsNotEndpointOfParent());
      mcp_same = mcp_same && (edm_mcp_orig.isDecayedInTracker() == lcio_mcp->isDecayedInTracker());
      mcp_same = mcp_same && (edm_mcp_orig.isDecayedInCalorimeter() == lcio_mcp->isDecayedInCalorimeter());
      mcp_same = mcp_same && (edm_mcp_orig.hasLeftDetector() == lcio_mcp->hasLeftDetector());
      mcp_same = mcp_same && (edm_mcp_orig.isStopped() == lcio_mcp->isStopped());
      mcp_same = mcp_same && (edm_mcp_orig.isOverlay() == lcio_mcp->isOverlay());
    }

    std::vector<uint> appeared(lcio_coll_size, 0);
    for (const auto& [orig_idx, link_idx] : mcp_parents_idx) {
      auto* lcio_mcp_orig = dynamic_cast<lcio::MCParticleImpl*>(lcio_mcparticle_coll->getElementAt(orig_idx));
      auto* lcio_mcp_link = dynamic_cast<lcio::MCParticleImpl*>(lcio_mcparticle_coll->getElementAt(link_idx));
      mcp_same            = mcp_same && (lcio_mcp_orig->getParents()[appeared[orig_idx]] == lcio_mcp_link);
      appeared[orig_idx]++;
    }
  }

  if (!mcp_same) {
    error() << "MCParticles EDM4hep -> LCIO failed." << endmsg;
  }

  return mcp_same;
}

bool TestE4H2L::checkEDMCaloHitEDMCaloHit() {
  // CalorimeterHit
  DataHandle<edm4hep::CalorimeterHitCollection> calohit_handle_orig{m_e4h_calohit_name, Gaudi::DataHandle::Reader,
                                                                    this};
  const auto                                    calohit_coll_orig = calohit_handle_orig.get();
  DataHandle<edm4hep::CalorimeterHitCollection> calohit_handle{m_e4h_calohit_name + "_conv", Gaudi::DataHandle::Reader,
                                                               this};
  const auto                                    calohit_coll = calohit_handle.get();

  bool calohit_same = true;

  if ((*calohit_coll).size() > 0) {  // avoid always true problems
    for (int i = 0; i < (*calohit_coll).size(); ++i) {
      auto edm_calohit_orig = (*calohit_coll_orig)[i];
      auto edm_calohit      = (*calohit_coll)[i];

      calohit_same = calohit_same && (edm_calohit_orig.getCellID() == edm_calohit.getCellID());
      calohit_same = calohit_same && (edm_calohit_orig.getEnergy() == edm_calohit.getEnergy());
      calohit_same = calohit_same && (edm_calohit_orig.getEnergyError() == edm_calohit.getEnergyError());
      calohit_same = calohit_same && (edm_calohit_orig.getTime() == edm_calohit.getTime());
      calohit_same = calohit_same && (edm_calohit_orig.getPosition() == edm_calohit.getPosition());
      calohit_same = calohit_same && (edm_calohit_orig.getType() == edm_calohit.getType());
    }
  }

  if (!calohit_same) {
    error() << "CalorimeterHit EDM4hep -> LCIO -> EDM4hep failed." << endmsg;
  }

  return calohit_same;
}

bool TestE4H2L::checkEDMMCParticleEDMMCParticle(const std::vector<std::pair<uint, uint>>& mcp_parents_idx) {
  DataHandle<edm4hep::MCParticleCollection> mcparticle_handle_orig{m_e4h_mcparticle_name, Gaudi::DataHandle::Reader,
                                                                   this};
  const auto                                mcparticle_coll_orig = mcparticle_handle_orig.get();
  DataHandle<edm4hep::MCParticleCollection> mcparticle_handle{m_e4h_mcparticle_name + m_conv_tag,
                                                              Gaudi::DataHandle::Reader, this};
  const auto                                mcparticle_coll = mcparticle_handle.get();

  bool mcp_same = (*mcparticle_coll_orig).size() == (*mcparticle_coll).size();
  if (mcp_same) {
    for (int i = 0; i < (*mcparticle_coll_orig).size(); ++i) {
      auto edm_mcp_orig = (*mcparticle_coll_orig)[i];
      auto edm_mcp      = (*mcparticle_coll)[i];

      mcp_same = mcp_same && (edm_mcp_orig.getPDG() == edm_mcp.getPDG());
      mcp_same = mcp_same && (edm_mcp_orig.getGeneratorStatus() == edm_mcp.getGeneratorStatus());
      // mcp_same = mcp_same && (edm_mcp_orig.getSimulatorStatus() ==
      // edm_mcp.getSimulatorStatus());

      mcp_same = mcp_same && (edm_mcp_orig.getVertex()[0] == edm_mcp.getVertex()[0]);
      mcp_same = mcp_same && (edm_mcp_orig.getVertex()[1] == edm_mcp.getVertex()[1]);
      mcp_same = mcp_same && (edm_mcp_orig.getVertex()[2] == edm_mcp.getVertex()[2]);

      mcp_same = mcp_same && (edm_mcp_orig.getTime() == edm_mcp.getTime());

      mcp_same = mcp_same && (edm_mcp_orig.getEndpoint()[0] == edm_mcp.getEndpoint()[0]);
      mcp_same = mcp_same && (edm_mcp_orig.getEndpoint()[1] == edm_mcp.getEndpoint()[1]);
      mcp_same = mcp_same && (edm_mcp_orig.getEndpoint()[2] == edm_mcp.getEndpoint()[2]);

      mcp_same = mcp_same && (edm_mcp_orig.getMomentum()[0] == edm_mcp.getMomentum()[0]);
      mcp_same = mcp_same && (edm_mcp_orig.getMomentum()[1] == edm_mcp.getMomentum()[1]);
      mcp_same = mcp_same && (edm_mcp_orig.getMomentum()[2] == edm_mcp.getMomentum()[2]);

      mcp_same = mcp_same && (edm_mcp_orig.getMomentumAtEndpoint()[0] == edm_mcp.getMomentumAtEndpoint()[0]);
      mcp_same = mcp_same && (edm_mcp_orig.getMomentumAtEndpoint()[1] == edm_mcp.getMomentumAtEndpoint()[1]);
      mcp_same = mcp_same && (edm_mcp_orig.getMomentumAtEndpoint()[2] == edm_mcp.getMomentumAtEndpoint()[2]);

      mcp_same = mcp_same && (edm_mcp_orig.getMass() == edm_mcp.getMass());
      mcp_same = mcp_same && (edm_mcp_orig.getCharge() == edm_mcp.getCharge());

      mcp_same = mcp_same && (edm_mcp_orig.getSpin()[0] == edm_mcp.getSpin()[0]);
      mcp_same = mcp_same && (edm_mcp_orig.getSpin()[1] == edm_mcp.getSpin()[1]);
      mcp_same = mcp_same && (edm_mcp_orig.getSpin()[2] == edm_mcp.getSpin()[2]);

      mcp_same = mcp_same && (edm_mcp_orig.getColorFlow()[0] == edm_mcp.getColorFlow()[0]);
      mcp_same = mcp_same && (edm_mcp_orig.getColorFlow()[1] == edm_mcp.getColorFlow()[1]);

      mcp_same = mcp_same && (edm_mcp_orig.isCreatedInSimulation() == edm_mcp.isCreatedInSimulation());
      mcp_same = mcp_same && (edm_mcp_orig.isBackscatter() == edm_mcp.isBackscatter());
      mcp_same = mcp_same && (edm_mcp_orig.vertexIsNotEndpointOfParent() == edm_mcp.vertexIsNotEndpointOfParent());
      mcp_same = mcp_same && (edm_mcp_orig.isDecayedInTracker() == edm_mcp.isDecayedInTracker());
      mcp_same = mcp_same && (edm_mcp_orig.isDecayedInCalorimeter() == edm_mcp.isDecayedInCalorimeter());
      mcp_same = mcp_same && (edm_mcp_orig.hasLeftDetector() == edm_mcp.hasLeftDetector());
      mcp_same = mcp_same && (edm_mcp_orig.isStopped() == edm_mcp.isStopped());
      mcp_same = mcp_same && (edm_mcp_orig.isOverlay() == edm_mcp.isOverlay());
    }

    // Check links between converted MCParticles
    std::vector<uint> appeared((*mcparticle_coll).size(), 0);
    for (const auto& [orig_idx, link_idx] : mcp_parents_idx) {
      auto edm_mcp_orig = (*mcparticle_coll)[orig_idx];
      auto edm_mcp_link = (*mcparticle_coll)[link_idx];
      mcp_same          = mcp_same && (edm_mcp_orig.getParents(appeared[orig_idx]) == edm_mcp_link);
      appeared[orig_idx]++;
    }
  }

  if (!mcp_same) {
    error() << "MCParticle EDM4hep -> LCIO -> EDM4hep failed." << endmsg;
  }

  return mcp_same;
}

bool TestE4H2L::checkEDMSimCaloHitEDMSimCaloHit(const std::vector<std::tuple<uint, uint, uint>>& link_mcparticles_idx) {
  DataHandle<edm4hep::SimCalorimeterHitCollection> simcalohit_handle_orig{m_e4h_simcalohit_name,
                                                                          Gaudi::DataHandle::Reader, this};
  const auto                                       simcalohit_coll_orig = simcalohit_handle_orig.get();
  DataHandle<edm4hep::SimCalorimeterHitCollection> simcalohit_handle{m_e4h_simcalohit_name + m_conv_tag,
                                                                     Gaudi::DataHandle::Reader, this};
  const auto                                       simcalohit_coll = simcalohit_handle.get();

  bool simcalohit_same = (*simcalohit_coll_orig).size() == (*simcalohit_coll).size();
  if (simcalohit_same) {
    for (int i = 0; i < (*simcalohit_coll_orig).size(); ++i) {
      auto edm_simcalohit_orig = (*simcalohit_coll_orig)[i];
      auto edm_simcalohit      = (*simcalohit_coll)[i];

      simcalohit_same = simcalohit_same && (edm_simcalohit_orig.getCellID() == edm_simcalohit.getCellID());
      for (int k = 0; k < 3; ++k) {
        simcalohit_same = simcalohit_same && (edm_simcalohit_orig.getPosition()[k] == edm_simcalohit.getPosition()[k]);
      }

      simcalohit_same =
          simcalohit_same && (edm_simcalohit_orig.contributions_size() == edm_simcalohit.contributions_size());
      if (simcalohit_same) {
        for (int j = 0; j < edm_simcalohit_orig.contributions_size(); ++j) {
          auto edm_calohit_orig_contr = edm_simcalohit_orig.getContributions(j);
          auto edm_calohit_contr      = edm_simcalohit.getContributions(j);

          simcalohit_same = simcalohit_same && (edm_calohit_orig_contr.getPDG() == edm_calohit_contr.getPDG());
          simcalohit_same = simcalohit_same && (edm_calohit_orig_contr.getEnergy() == edm_calohit_contr.getEnergy());
          simcalohit_same = simcalohit_same && (edm_calohit_orig_contr.getTime() == edm_calohit_contr.getTime());
          simcalohit_same = simcalohit_same && (edm_calohit_orig_contr.getTime() == edm_calohit_contr.getTime());
          for (int k = 0; k < 3; ++k) {
            simcalohit_same = simcalohit_same &&
                              (edm_calohit_orig_contr.getStepPosition()[k] == edm_calohit_contr.getStepPosition()[k]);
          }
        }
      }
    }

    // Check MCParticles linked to Contributions in SimCaloHits
    // SimCaloHits have all the same number of contributions during creation for
    // this test
    // The contribution idx can be used directly then (instead of calculation
    // based on how many were added)
    DataHandle<edm4hep::MCParticleCollection> mcparticle_handle{m_e4h_mcparticle_name + m_conv_tag,
                                                                Gaudi::DataHandle::Reader, this};
    const auto                                mcparticle_coll = mcparticle_handle.get();

    for (const auto& [simch_idx, contrib_idx, mcpart_idx] : link_mcparticles_idx) {
      auto edm_simcalohit = (*simcalohit_coll)[simch_idx];

      auto edm_simch_mcpart = edm_simcalohit.getContributions(contrib_idx).getParticle();
      auto edm_mcpart       = (*mcparticle_coll)[mcpart_idx];

      simcalohit_same = simcalohit_same && (edm_simch_mcpart == edm_mcpart);
    }
  }

  if (!simcalohit_same) {
    error() << "SimCalorimeterHit EDM4hep -> LCIO -> EDM4hep failed." << endmsg;
  }

  return simcalohit_same;
}

bool TestE4H2L::checkEDMTrackEDMTrack(const std::vector<std::pair<uint, uint>>& track_link_tracks_idx) {
  // Track
  DataHandle<edm4hep::TrackCollection> track_handle_orig{m_e4h_track_name, Gaudi::DataHandle::Reader, this};
  const auto                           track_coll_orig = track_handle_orig.get();
  DataHandle<edm4hep::TrackCollection> track_handle{m_e4h_track_name + "_conv", Gaudi::DataHandle::Reader, this};
  const auto                           track_coll = track_handle.get();

  bool track_same = (*track_coll_orig).size() == (*track_coll).size();

  if (track_same) {
    for (int i = 0; i < (*track_coll).size(); ++i) {
      auto edm_track_orig = (*track_coll_orig)[i];
      auto edm_track      = (*track_coll)[i];

      // TODO Review EDM4hep to LCIO Track conversion for TypeBit
      // track_same = track_same && (edm_track_orig.getType() ==
      // edm_track.getType());
      track_same = track_same && (edm_track_orig.getChi2() == edm_track.getChi2());
      track_same = track_same && (edm_track_orig.getNdf() == edm_track.getNdf());
      track_same = track_same && (edm_track_orig.getDEdx() == edm_track.getDEdx());
      track_same = track_same && (edm_track_orig.getDEdxError() == edm_track.getDEdxError());
      track_same = track_same && (edm_track_orig.getRadiusOfInnermostHit() == edm_track.getRadiusOfInnermostHit());

      track_same = track_same && (edm_track_orig.trackerHits_size() == edm_track.trackerHits_size());
      if ((edm_track_orig.trackerHits_size() == edm_track.trackerHits_size())) {
        for (int j = 0; j < edm_track_orig.trackerHits_size(); ++j) {
          auto edm_trackerhit_orig = edm_track_orig.getTrackerHits(j);
          auto edm_trackerhit      = edm_track.getTrackerHits(j);

          track_same = track_same && (edm_trackerhit_orig.getCellID() == edm_trackerhit.getCellID());
          track_same = track_same && (edm_trackerhit_orig.getType() == edm_trackerhit.getType());
          track_same = track_same && (edm_trackerhit_orig.getQuality() == edm_trackerhit.getQuality());
          track_same = track_same && (edm_trackerhit_orig.getTime() == edm_trackerhit.getTime());
          track_same = track_same && (edm_trackerhit_orig.getEDep() == edm_trackerhit.getEDep());

          track_same = track_same && (edm_trackerhit_orig.getEDepError() == edm_trackerhit.getEDepError());
          track_same = track_same && (edm_trackerhit_orig.getPosition() == edm_trackerhit.getPosition());
          track_same = track_same && (edm_trackerhit_orig.getCovMatrix() == edm_trackerhit.getCovMatrix());

          // TODO Raw hits
        }
      }

      // TODO Resizing in EDM4hep to LCIO conversion causes to "have" 50 hits
      if (edm_track.subDetectorHitNumbers_size() == 50) {
        for (int j = 0; j < edm_track_orig.subDetectorHitNumbers_size(); ++j) {
          track_same =
              track_same && (edm_track_orig.getSubDetectorHitNumbers(j) == edm_track.getSubDetectorHitNumbers(j));
        }
        for (int j = edm_track_orig.subDetectorHitNumbers_size(); j < 50; ++j) {
          track_same = track_same && (0 == edm_track.getSubDetectorHitNumbers(j));
        }
      } else {
        track_same =
            track_same && (edm_track_orig.subDetectorHitNumbers_size() == edm_track.subDetectorHitNumbers_size());
        if ((edm_track_orig.subDetectorHitNumbers_size() == edm_track.subDetectorHitNumbers_size())) {
          for (int j = 0; j < edm_track_orig.subDetectorHitNumbers_size(); ++j) {
            track_same =
                track_same && (edm_track_orig.getSubDetectorHitNumbers(j) == edm_track.getSubDetectorHitNumbers(j));
          }
        }
      }

      track_same = track_same && (edm_track_orig.trackStates_size() == edm_track.trackStates_size());
      if ((edm_track_orig.trackStates_size() == edm_track.trackStates_size())) {
        for (int j = 0; j < edm_track_orig.trackStates_size(); ++j) {
          auto edm_trackestate_orig = edm_track_orig.getTrackStates(j);
          auto edm_trackestate      = edm_track.getTrackStates(j);

          track_same = track_same && (edm_trackestate_orig.location == edm_trackestate.location);
          track_same = track_same && (edm_trackestate_orig.D0 == edm_trackestate.D0);
          track_same = track_same && (edm_trackestate_orig.phi == edm_trackestate.phi);
          track_same = track_same && (edm_trackestate_orig.omega == edm_trackestate.omega);

          track_same = track_same && (edm_trackestate_orig.Z0 == edm_trackestate.Z0);
          track_same = track_same && (edm_trackestate_orig.tanLambda == edm_trackestate.tanLambda);
          track_same = track_same && (edm_trackestate_orig.referencePoint == edm_trackestate.referencePoint);
          track_same = track_same && (edm_trackestate_orig.covMatrix == edm_trackestate.covMatrix);
        }
      }
    }

    // Check linked tracks
    std::vector<uint> appeared((*track_coll).size(), 0);
    for (const auto& [orig_idx, link_idx] : track_link_tracks_idx) {
      auto edm_track_orig = (*track_coll)[orig_idx];
      auto edm_track_link = (*track_coll)[link_idx];
      track_same          = track_same && (edm_track_orig.getTracks(appeared[orig_idx]) == edm_track_link);
      appeared[orig_idx]++;
    }
  }

  if (!track_same) {
    error() << "Track EDM4hep -> LCIO -> EDM4hep failed." << endmsg;
  }

  return track_same;
}

StatusCode TestE4H2L::execute() {
  lcio::LCEventImpl* the_event = new lcio::LCEventImpl();

  // Configuration for the test
  int   int_cnt   = 10;
  float float_cnt = 10.1;

  // CaloHit
  const int calohit_elems = 2;

  // RawCaloHit
  const int rawcalohit_elems = 2;

  // TPCHit
  const int tpchit_elems    = 4;
  const int tpchit_rawwords = 6;

  // TrackerHits
  const int trackerhits_elems = 5;

  // Tracks
  const int               track_elems                 = 4;
  const int               track_subdetectorhitnumbers = 4;
  const std::vector<uint> track_link_trackerhits_idx  = {0, 2, 4};
  // Check bounds
  for (const auto& idx : track_link_trackerhits_idx) {
    assert(idx < trackerhits_elems);
  }
  const int                                track_states          = 5;
  const std::vector<std::pair<uint, uint>> track_link_tracks_idx = {{0, 2}, {1, 3}, {2, 3}, {3, 2}, {3, 0}};
  // Check bounds
  for (const auto& [orig, dest] : track_link_tracks_idx) {
    assert(orig < track_elems);
    assert(dest < track_elems);
  }

  // MCParticles
  const int                                mcparticle_elems = 5;
  const std::vector<std::pair<uint, uint>> mcp_parents_idx  = {{4, 0}, {4, 1}, {3, 2}, {3, 0}, {3, 1}, {2, 1}};
  // Check bounds
  for (const auto& [daughter, parent] : mcp_parents_idx) {
    assert(daughter < mcparticle_elems);
    assert(parent < mcparticle_elems);
    // TODO daughter id must be higher than parent
    assert(daughter > parent);
  }

  // SimCaloHit
  const int simcalohit_elems = 3;
  // Fixed amount of contributions for every simcalohit
  const int simcalohit_contributions = 4;
  // SimCaloHit_idx, Contrib_idx, MCParticle_idx
  // There must be one MCPartcle per contribution (fill every 1st and 2nd idx)
  const std::vector<std::tuple<uint, uint, uint>> simcalohit_mcparticles_idx = {
      {0, 0, 0}, {0, 1, 2}, {0, 2, 1}, {0, 3, 4}, {1, 0, 1}, {1, 1, 3},
      {1, 2, 4}, {1, 3, 4}, {2, 0, 0}, {2, 1, 3}, {2, 2, 2}, {2, 3, 0},
  };
  // Check bounds
  assert(simcalohit_mcparticles_idx.size() == (simcalohit_elems * simcalohit_contributions));
  // Check correct indices
  for (int i = 0; i < simcalohit_elems; ++i) {
    for (int j = 0; j < simcalohit_contributions; ++j) {
      uint c_idx = j + i * simcalohit_contributions;
      assert(std::get<0>(simcalohit_mcparticles_idx[c_idx]) == i);
      assert(std::get<1>(simcalohit_mcparticles_idx[c_idx]) == j);
    }
  }
  for (const auto& [simch_idx, contrib_idx, mcpart_idx] : simcalohit_mcparticles_idx) {
    assert(mcpart_idx < mcparticle_elems);
    assert(contrib_idx < simcalohit_contributions);
    assert(simch_idx < simcalohit_elems);
  }

  // SimTrackerHits
  const int                                simtrackerhit_elems  = 5;
  const std::vector<std::pair<uint, uint>> strh_mcparticles_idx = {{0, 0}, {1, 2}, {2, 1}, {3, 4}, {4, 1}};
  for (const auto& [strh, mcp] : strh_mcparticles_idx) {
    assert(strh < simtrackerhit_elems);
    assert(mcp < mcparticle_elems);
  }

  /////////////////////////////////////////////////////////////////////
  // Create fake collections based on configuration
  /////////////////////////////////////////////////////////////////////
  createCalorimeterHits(calohit_elems, int_cnt, float_cnt);
  createRawCalorimeterHits(rawcalohit_elems, int_cnt, float_cnt);
  createTPCHits(tpchit_elems, tpchit_rawwords, int_cnt, float_cnt);
  createTrackerHits(trackerhits_elems, int_cnt, float_cnt);
  createTracks(  // Depends on TrackerHits
      track_elems, track_subdetectorhitnumbers, track_link_trackerhits_idx, track_states, track_link_tracks_idx,
      int_cnt, float_cnt);
  createMCParticles(mcparticle_elems, mcp_parents_idx, int_cnt, float_cnt);
  createSimCalorimeterHits(  // Depends on createMCParticles()
      simcalohit_elems, simcalohit_contributions, simcalohit_mcparticles_idx, int_cnt, float_cnt);
  createSimTrackerHits(  // Depends on createMCParticles()
      simtrackerhit_elems, strh_mcparticles_idx, int_cnt, float_cnt);

  // Convert from EDM4hep to LCIO
  StatusCode edm_sc = m_edm_conversionTool->convertCollections(the_event);

  /////////////////////////////////////////////////////////////////////
  // Check EDM4hep -> LCIO conversion
  /////////////////////////////////////////////////////////////////////
  bool lcio_same = checkEDMTrackLCIOTrack(the_event, track_link_trackerhits_idx, track_link_tracks_idx) &&
                   checkEDMTrackerHitLCIOTrackerHit(the_event) && checkEDMTPCHitLCIOTPCHit(the_event) &&
                   checkEDMRawCaloHitLCIORawCaloHit(the_event) &&
                   checkEDMMCParticleLCIOMCParticle(the_event, mcp_parents_idx) &&
                   checkEDMSimCaloHitLCIOSimCaloHit(the_event, simcalohit_mcparticles_idx) &&
                   checkEDMSimTrackerHitLCIOSimTrackerHit(the_event, strh_mcparticles_idx);

  // Convert from LCIO to EDM4hep
  StatusCode lcio_sc = m_lcio_conversionTool->convertCollections(the_event);

  /////////////////////////////////////////////////////////////////////
  // Check EDM4hep -> LCIO -> EDM4hep conversion
  /////////////////////////////////////////////////////////////////////
  bool edm_same = checkEDMCaloHitEDMCaloHit() && checkEDMTrackEDMTrack(track_link_tracks_idx) &&
                  checkEDMMCParticleEDMMCParticle(mcp_parents_idx) &&
                  checkEDMSimCaloHitEDMSimCaloHit(simcalohit_mcparticles_idx) &&
                  checkEDMSimTrackerHitEDMSimTrackerHit(strh_mcparticles_idx);

  return (edm_same && lcio_same) ? StatusCode::SUCCESS : StatusCode::FAILURE;
}

StatusCode TestE4H2L::finalize() { return Algorithm::finalize(); }
