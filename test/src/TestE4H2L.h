#ifndef TEST_E4H2L_H
#define TEST_E4H2L_H

#include <string>

#include <GaudiAlg/GaudiAlgorithm.h>

#include <k4FWCore/DataHandle.h>

// Converters interface
#include "k4MarlinWrapper/converters/IEDMConverter.h"

class TestE4H2L : public GaudiAlgorithm {
public:
  explicit TestE4H2L(const std::string& name, ISvcLocator* pSL);
  virtual ~TestE4H2L() = default;
  virtual StatusCode execute() override final;
  virtual StatusCode finalize() override final;
  virtual StatusCode initialize() override final;

private:
  ToolHandle<IEDMConverter> m_edm_conversionTool{"IEDMConverter/EDM4hep2Lcio", this};
  ToolHandle<IEDMConverter> m_lcio_conversionTool{"IEDMConverter/Lcio2EDM4hep", this};

  std::map<std::string, DataObjectHandleBase*> m_dataHandlesMap;

  const std::string m_e4h_calohit_name       = "E4H_CaloHitCollection";
  const std::string m_e4h_rawcalohit_name    = "E4H_RawCaloHitCollection";
  const std::string m_e4h_simcalohit_name    = "E4H_SimCaloHitCollection";
  const std::string m_e4h_tpchit_name        = "E4H_TPCHitCollection";
  const std::string m_e4h_trackerhit_name    = "E4H_TrackerHitCollection";
  const std::string m_e4h_simtrackerhit_name = "E4H_SimTrackerHitCollection";
  const std::string m_e4h_track_name         = "E4H_TrackCollection";
  const std::string m_e4h_mcparticle_name    = "E4H_MCParticleCollection";

  const std::string m_lcio_calohit_name       = "LCIO_CaloHitCollection";
  const std::string m_lcio_rawcalohit_name    = "LCIO_RawCaloHitCollection";
  const std::string m_lcio_simcalohit_name    = "LCIO_SimCaloHitCollection";
  const std::string m_lcio_tpchit_name        = "LCIO_TPCHitCollection";
  const std::string m_lcio_trackerhit_name    = "LCIO_TrackerHitCollection";
  const std::string m_lcio_simtrackerhit_name = "LCIO_SimTrackerHitCollection";
  const std::string m_lcio_track_name         = "LCIO_TrackCollection";
  const std::string m_lcio_mcparticle_name    = "LCIO_MCParticleCollection";

  const std::string m_conv_tag = "_conv";

  // Fake data creation
  void createCalorimeterHits(const int num_elements, int& int_cnt, float& float_cnt);
  void createRawCalorimeterHits(const int num_elements, int& int_cnt, float& float_cnt);
  void createSimCalorimeterHits(const int num_elements, const int num_contributions,
                                const std::vector<std::tuple<uint, uint, uint>>& link_mcparticles_idx, int& int_cnt,
                                float& float_cnt);
  void createTPCHits(const int num_elements, const int num_rawwords, int& int_cnt, float& float_cnt);
  void createTrackerHits(const int num_elements, int& int_cnt, float& float_cnt);
  void createSimTrackerHits(const int num_elements, const std::vector<std::pair<uint, uint>>& link_mcparticles_idx,
                            int& int_cnt, float& float_cnt);
  void createTracks(const int num_elements, const int subdetectorhitnumbers,
                    const std::vector<uint>& link_trackerhits_idx, const int num_track_states,
                    const std::vector<std::pair<uint, uint>>& track_link_tracks_idx, int& int_cnt, float& float_cnt);
  void createMCParticles(const int num_elements, const std::vector<std::pair<uint, uint>>& track_parents_idx,
                         int& int_cnt, float& float_cnt);

  // EDM4hep -> LCIO checks
  bool checkEDMRawCaloHitLCIORawCaloHit(lcio::LCEventImpl* the_event);
  bool checkEDMTPCHitLCIOTPCHit(lcio::LCEventImpl* the_event);
  bool checkEDMTrackerHitLCIOTrackerHit(lcio::LCEventImpl* the_event);
  bool checkEDMTrackLCIOTrack(lcio::LCEventImpl* the_event, const std::vector<uint>& link_trackerhits_idx,
                              const std::vector<std::pair<uint, uint>>& track_link_tracks_idx);
  bool checkEDMMCParticleLCIOMCParticle(lcio::LCEventImpl*                        the_event,
                                        const std::vector<std::pair<uint, uint>>& mcp_parents_idx);
  bool checkEDMSimCaloHitLCIOSimCaloHit(lcio::LCEventImpl*                               the_event,
                                        const std::vector<std::tuple<uint, uint, uint>>& link_mcparticles_idx);
  bool checkEDMSimTrackerHitLCIOSimTrackerHit(lcio::LCEventImpl*                        the_event,
                                              const std::vector<std::pair<uint, uint>>& link_mcparticles_idx);

  // EDM4hep -> LCIO -> EDM4hep checks
  bool checkEDMCaloHitEDMCaloHit();
  bool checkEDMTrackEDMTrack(const std::vector<std::pair<uint, uint>>& track_link_tracks_idx);
  bool checkEDMMCParticleEDMMCParticle(const std::vector<std::pair<uint, uint>>& mcp_parents_idx);
  bool checkEDMSimCaloHitEDMSimCaloHit(const std::vector<std::tuple<uint, uint, uint>>& link_mcparticles_idx);
  bool checkEDMSimTrackerHitEDMSimTrackerHit(const std::vector<std::pair<uint, uint>>& link_mcparticles_idx);
};

#endif