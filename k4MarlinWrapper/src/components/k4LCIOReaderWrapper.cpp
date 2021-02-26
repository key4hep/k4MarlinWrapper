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

template <typename T>
void k4LCIOReaderWrapper::convertAndRegister(
  const std::string& register_name,
  const std::string& collection_name,
  k4LCIOConverter* lcio_converter,
  podio::CollectionIDTable* id_table) const
{
  // Pass name of collection to get
  T* mycoll = dynamic_cast<T*>(lcio_converter->getCollection(collection_name));

  if (mycoll == nullptr) {
    mycoll = new T();
    mycoll->setID(id_table->add(collection_name));
  }

  // Register collection
  EDMCollectionWrapper<T> mycoll_wrapper {mycoll};
  auto pO = std::make_unique<EDMCollectionWrapper<T>>(mycoll_wrapper);
  StatusCode reg_sc = evtSvc()->registerObject(register_name, pO.release());
}


StatusCode k4LCIOReaderWrapper::convertCollections(
  lcio::LCEventImpl* the_event)
{

  if (m_lcio2edm_params.size() % 3 != 0) {
    error() << " Error processing conversion parameters. 3 arguments per collection expected. " << endmsg;
    return StatusCode::FAILURE;
  }

  // Set the event to the converter
  podio::CollectionIDTable* id_table = new podio::CollectionIDTable();
  k4LCIOConverter* lcio_converter = new k4LCIOConverter(id_table);
  lcio_converter->set(the_event);

  // Create eventHeader to fill
  auto* dest = new edm4hep::EventHeaderCollection();
  edm4hep::EventHeader lval = dest->create();
  lval.setEventNumber(the_event->getEventNumber());
  lval.setRunNumber(the_event->getRunNumber());
  lval.setTimeStamp(the_event->getTimeStamp());
  lval.setWeight(the_event->getWeight());

  // Convert based on parameters
  for (int i = 0; i < m_lcio2edm_params.size(); i=i+3) {

    if (m_lcio2edm_params[i] == "ReconstructedParticle") {
      convertAndRegister<edm4hep::ReconstructedParticleCollection>(
        m_lcio2edm_params[i+2], m_lcio2edm_params[i+1], lcio_converter, id_table);
    } else if (m_lcio2edm_params[i] == "ParticleID") {
      convertAndRegister<edm4hep::ParticleIDCollection>(
        m_lcio2edm_params[i+2], m_lcio2edm_params[i+1], lcio_converter, id_table);
    } else if (m_lcio2edm_params[i] == "Track") {
      convertAndRegister<edm4hep::TrackCollection>(
        m_lcio2edm_params[i+2], m_lcio2edm_params[i+1], lcio_converter, id_table);
    } else {
      error() << m_lcio2edm_params[i] << ": conversion type not supported." << endmsg;
      error() << "List of supported types: Track, ParticleID, ReconstructedParticle." << endmsg;
    }

  }

  return StatusCode::SUCCESS;
}
