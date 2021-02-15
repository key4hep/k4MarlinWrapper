#include "converters/k4LCIOReaderWrapper.h"


DECLARE_COMPONENT(k4LCIOReaderWrapper);

k4LCIOReaderWrapper::k4LCIOReaderWrapper(const std::string& type, const std::string& name, const IInterface* parent)
    : GaudiTool(type, name, parent) {
  declareInterface<Ik4LCIOReaderWrapper>(this);
}

k4LCIOReaderWrapper::~k4LCIOReaderWrapper() { ; }


StatusCode k4LCIOReaderWrapper::initialize() {
  return GaudiTool::initialize();
}

StatusCode k4LCIOReaderWrapper::finalize() {
  return GaudiTool::finalize();
}


StatusCode k4LCIOReaderWrapper::convertCollections(
  lcio::LCEventImpl* the_event)
{

  if (m_lcio2edm_params.size() % 3 != 0) {
    error() << " Error processing conversion parameters. 3 arguments per collection expected. " << endmsg;
    return StatusCode::FAILURE;
  }

  // Set the event to the converter
  podio::CollectionIDTable* m_table = new podio::CollectionIDTable();
  k4LCIOConverter* m_converter = new k4LCIOConverter(m_table);
  m_converter->set(the_event);

  // Create event to fill
  // podio::CollectionBase* dest = new edm4hep::EventHeaderCollection();
  auto* dest = new edm4hep::EventHeaderCollection();
  edm4hep::EventHeader lval = dest->create();
  lval.setEventNumber(the_event->getEventNumber());
  lval.setRunNumber(the_event->getRunNumber());
  lval.setTimeStamp(the_event->getTimeStamp());
  lval.setWeight(the_event->getWeight());

  // Convert based on parameters
  for (int i = 0; i < m_lcio2edm_params.size(); i=i+3) {

      // convertAdd(
      //   m_lcio2edm_params[i],
      //   m_lcio2edm_params[i+1],
      //   m_lcio2edm_params[i+2],
      //   the_event);

    auto* mycoll = dynamic_cast<edm4hep::ReconstructedParticleCollection*>(m_converter->getCollection(m_lcio2edm_params[i+1]));

    if (mycoll == nullptr) {
      mycoll = new edm4hep::ReconstructedParticleCollection();
      mycoll->setID(m_table->add(m_lcio2edm_params[i+1]));
    }

    std::cout << *mycoll << std::endl;

  }

  return StatusCode::SUCCESS;
}
