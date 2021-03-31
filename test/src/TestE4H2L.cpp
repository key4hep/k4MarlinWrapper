#include <TestE4H2L.h>

DECLARE_COMPONENT(TestE4H2L)

TestE4H2L::TestE4H2L(const std::string& name, ISvcLocator* pSL) : GaudiAlgorithm(name, pSL) {
  declareProperty("EDM4hep2LcioTool", m_edm_conversionTool = nullptr);
  declareProperty("Lcio2EDM4hepTool", m_lcio_conversionTool = nullptr);
}

StatusCode TestE4H2L::initialize() {

  m_dataHandlesMap[m_edm_callohit_name] = new DataHandle<edm4hep::CalorimeterHitCollection>(
    m_edm_callohit_name, Gaudi::DataHandle::Writer, this);
  m_dataHandlesMap[m_edm_particleid_name] = new DataHandle<edm4hep::ParticleIDCollection>(
    m_edm_particleid_name, Gaudi::DataHandle::Writer, this);

  return StatusCode::SUCCESS;
}

void TestE4H2L::createFakeCollections()
{

  // edm4hep::CalorimeterHits
  const int calohit_elems = 2;
  auto calohit_coll = new edm4hep::CalorimeterHitCollection();

  int int_cnt = 10;
  float float_cnt = 10.1;

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

  auto* calohit_handle = dynamic_cast<DataHandle<edm4hep::CalorimeterHitCollection>*>( m_dataHandlesMap[m_edm_callohit_name] );
  calohit_handle->put(calohit_coll);


  // edm4hep::ParticleID
  const int particleID_elems = 2;
  auto particleid_coll = new edm4hep::ParticleIDCollection();

  for (int i=0; i < particleID_elems; ++i) {

    auto elem = new edm4hep::ParticleID();

    elem->setType(int_cnt++);
    elem->setPDG(int_cnt++);
    elem->setAlgorithmType(int_cnt++);
    elem->setLikelihood(float_cnt++);

    const int particleID_params = 3;
    for (int j=0; j < particleID_params; ++j){
      elem->addToParameters(float_cnt++);
    }

    particleid_coll->push_back(*elem);
  }

  auto* particleid_handle = dynamic_cast<DataHandle<edm4hep::ParticleIDCollection>*>( m_dataHandlesMap[m_edm_particleid_name] );
  particleid_handle->put(particleid_coll);
}


bool TestE4H2L::isSameEDM4hepEDM4hep()
{

  bool generalCheck = false;

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

  // ParticleID
  DataHandle<edm4hep::ParticleIDCollection> particleid_handle_orig {
    "E4H_ParticleIDCollection", Gaudi::DataHandle::Reader, this};
  const auto particleid_coll_orig = particleid_handle_orig.get();
  DataHandle<edm4hep::ParticleIDCollection> particleid_handle {
    "E4H_ParticleIDCollection_conv", Gaudi::DataHandle::Reader, this};
  const auto particleid_coll = particleid_handle.get();

  bool pid_same = true;

  if ((*particleid_coll).size() > 0) { // avoid always true problems
    for (int i=0; i < (*particleid_coll).size(); ++i) {
      auto edm_pid_orig = (*particleid_coll_orig)[i];
      auto edm_pid = (*particleid_coll)[i];

      pid_same = pid_same && (edm_pid_orig.getType() == edm_pid.getType());
      pid_same = pid_same && (edm_pid_orig.getPDG() == edm_pid.getPDG());
      pid_same = pid_same && (edm_pid_orig.getAlgorithmType() == edm_pid.getAlgorithmType());
      pid_same = pid_same && (edm_pid_orig.getLikelihood() == edm_pid.getLikelihood());

      pid_same = pid_same && (edm_pid_orig.parameters_size() == edm_pid.parameters_size());
      for (int j=0; j< edm_pid_orig.parameters_size(); ++j){
        pid_same = pid_same && (edm_pid_orig.getParameters(j) == edm_pid.getParameters(j));
      }
    }
  }

  if (!pid_same) {
    debug() << "ParameterID EDM4hep -> LCIO -> EDM4hep failed." << endmsg;
  }

  // Do general check
  generalCheck = calohit_same && pid_same;

  return generalCheck;
}


StatusCode TestE4H2L::execute() {

  lcio::LCEventImpl* the_event = new lcio::LCEventImpl();

  createFakeCollections();

  // Convert from EDM4hep to LCIO
  StatusCode edm_sc = m_edm_conversionTool->convertCollections(the_event);

  // Convert from LCIO to EDM4hep
  StatusCode lcio_sc =  m_lcio_conversionTool->convertCollections(the_event);

  bool same = isSameEDM4hepEDM4hep();

  return (same) ? StatusCode::SUCCESS : StatusCode::FAILURE;
}

StatusCode TestE4H2L::finalize() {
  return Algorithm::finalize();
}
