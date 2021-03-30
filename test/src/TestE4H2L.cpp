#include <TestE4H2L.h>

DECLARE_COMPONENT(TestE4H2L)

TestE4H2L::TestE4H2L(const std::string& name, ISvcLocator* pSL) : GaudiAlgorithm(name, pSL) {
  declareProperty("EDM4hep2LcioTool", m_edm_conversionTool = nullptr);
  declareProperty("Lcio2EDM4hepTool", m_lcio_conversionTool = nullptr);
}

StatusCode TestE4H2L::initialize() {

  m_dataHandlesMap[m_edm_callohit_name] = new DataHandle<edm4hep::CalorimeterHitCollection>(
    m_edm_callohit_name, Gaudi::DataHandle::Writer, this);
  return StatusCode::SUCCESS;
}


StatusCode TestE4H2L::execute() {

  lcio::LCEventImpl* the_event = nullptr;
  the_event = new lcio::LCEventImpl();

  const int num_elements = 2;
  // Create dummy
  auto dest = new edm4hep::CalorimeterHitCollection();

  int int_cnt = 10;
  float float_cnt = 10.1;

  for (int i=0; i < num_elements; ++i) {

    auto elem = new edm4hep::CalorimeterHit();

    elem->setCellID(int_cnt++);
    elem->setEnergy(float_cnt++);
    elem->setEnergyError(float_cnt++);
    elem->setTime(float_cnt++);
    edm4hep::Vector3f test_vec {float_cnt++, float_cnt++, float_cnt++};
    elem->setPosition(test_vec);
    elem->setType(int_cnt++);

    dest->push_back(*elem);
  }

  auto* handle = dynamic_cast<DataHandle<edm4hep::CalorimeterHitCollection>*>( m_dataHandlesMap[m_edm_callohit_name] );
  handle->put(dest);

  StatusCode edm_sc = m_edm_conversionTool->convertCollections(the_event);

  // Convert collection back
  StatusCode lcio_sc =  m_lcio_conversionTool->convertCollections(the_event);

  DataHandle<edm4hep::CalorimeterHitCollection> calo_handle_orig {
    "E4H_CaloHitCollection", Gaudi::DataHandle::Reader, this};
  const auto calo_coll_orig = calo_handle_orig.get();
  DataHandle<edm4hep::CalorimeterHitCollection> calo_handle {
    "E4H_CaloHitCollection_conv", Gaudi::DataHandle::Reader, this};
  const auto calo_coll = calo_handle.get();


  bool same = true;


  if ((*calo_coll).size() > 0) { // avoid always true problems
    for (int i=0; i < (*calo_coll).size(); ++i) {
      auto edm_calo_orig = (*calo_coll_orig)[i];
      auto edm_calo = (*calo_coll)[i];

      same = same && (edm_calo_orig.getCellID() == edm_calo.getCellID());
      same = same && (edm_calo_orig.getEnergy() == edm_calo.getEnergy());
      same = same && (edm_calo_orig.getEnergyError() == edm_calo.getEnergyError());
      same = same && (edm_calo_orig.getTime() == edm_calo.getTime());
      same = same && (edm_calo_orig.getPosition() == edm_calo.getPosition());
      same = same && (edm_calo_orig.getType() == edm_calo.getType());
    }
  }

  return (same) ? StatusCode::SUCCESS : StatusCode::FAILURE;
}

StatusCode TestE4H2L::finalize() {
  return Algorithm::finalize();
}
