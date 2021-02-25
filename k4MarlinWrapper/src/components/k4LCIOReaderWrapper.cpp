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

  // Create eventHeader to fill
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


    // Is the type needed?
    if (m_lcio2edm_params[i] == "EVENT::ReconstructedParticle") {
      // Pass name of collection to get
      auto mycoll = dynamic_cast<edm4hep::ReconstructedParticleCollection*>(m_converter->getCollection(m_lcio2edm_params[i+1]));

      if (mycoll == nullptr) {
        debug() << m_lcio2edm_params[i] << " collection was not found." << endmsg;
        mycoll = new edm4hep::ReconstructedParticleCollection();
        mycoll->setID(m_table->add(m_lcio2edm_params[i+1]));
      }

      std::cout << "edm4hep::ReconstructedParticleCollection\n";
      std::cout << *mycoll << std::endl;

      // Register collection
      EDMCollectionWrapper<edm4hep::ReconstructedParticleCollection> mycoll_wrapper {mycoll};
      auto pO = std::make_unique<EDMCollectionWrapper<edm4hep::ReconstructedParticleCollection>>(mycoll_wrapper);
      StatusCode reg_sc = evtSvc()->registerObject("/edm4hep/ReconstructedParticleCollection", pO.release());

    } else if (m_lcio2edm_params[i] == "EVENT::ParticleID") {

      auto* mycoll = dynamic_cast<edm4hep::ParticleIDCollection*>(m_converter->getCollection(m_lcio2edm_params[i+1]));

      if (mycoll == nullptr) {
        debug() << m_lcio2edm_params[i] << " collection was not found." << endmsg;
        mycoll = new edm4hep::ParticleIDCollection();
        mycoll->setID(m_table->add(m_lcio2edm_params[i+1]));
      }

      std::cout << "edm4hep::ParticleIDCollection\n";
      std::cout << *mycoll << std::endl;

      EDMCollectionWrapper<edm4hep::ParticleIDCollection> mycoll_wrapper {mycoll};
      auto pO = std::make_unique<EDMCollectionWrapper<edm4hep::ParticleIDCollection>>(mycoll_wrapper);
      StatusCode reg_sc = evtSvc()->registerObject("/edm4hep/ParticleIDCollection", pO.release());

    } else if (m_lcio2edm_params[i] == "EVENT::Track") {

      auto* mycoll = dynamic_cast<edm4hep::TrackCollection*>(m_converter->getCollection(m_lcio2edm_params[i+1]));

      if (mycoll == nullptr) {
        debug() << m_lcio2edm_params[i] << " collection was not found." << endmsg;
        mycoll = new edm4hep::TrackCollection();
        mycoll->setID(m_table->add(m_lcio2edm_params[i+1]));
      }

      std::cout << "edm4hep::TrackCollection\n";
      std::cout << *mycoll << std::endl;

      EDMCollectionWrapper<edm4hep::TrackCollection> mycoll_wrapper {mycoll};
      auto pO = std::make_unique<EDMCollectionWrapper<edm4hep::TrackCollection>>(mycoll_wrapper);
      StatusCode reg_sc = evtSvc()->registerObject("/edm4hep/TrackCollection", pO.release());

    } else {
      error() << m_lcio2edm_params[i] << " conversion type not supported." << endmsg;
    }

  }

  return StatusCode::SUCCESS;
}
