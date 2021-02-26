#ifndef K4MARLINWRAPPER_K4LCIOREADERWRAPPER_H
#define K4MARLINWRAPPER_K4LCIOREADERWRAPPER_H


// GAUDI
#include "GaudiAlg/GaudiTool.h"

// k4LCIOReader
#include "k4LCIOReader/k4LCIOReader.h"
#include "k4LCIOReader/k4LCIOConverter.h"

// EDM4hep
#include "edm4hep/ParticleIDCollection.h"
#include "edm4hep/ReconstructedParticle.h"
#include "edm4hep/ReconstructedParticleCollection.h"
#include "edm4hep/ReconstructedParticleData.h"
#include "edm4hep/Track.h"
#include "edm4hep/TrackCollection.h"
#include "edm4hep/VertexConst.h"
#include "edm4hep/EventHeaderCollection.h"

#include "podio/CollectionBase.h"

#include "EDMCollectionWrapper.h"

// Interface
#include "Ik4LCIOReaderWrapper.h"


class k4LCIOReaderWrapper : public GaudiTool, virtual public Ik4LCIOReaderWrapper {

public:
  k4LCIOReaderWrapper(const std::string& type, const std::string& name, const IInterface* parent);
  virtual ~k4LCIOReaderWrapper();
  virtual StatusCode initialize();
  virtual StatusCode finalize();

  StatusCode convertCollections(
    lcio::LCEventImpl* lcio_event);

private:

  Gaudi::Property<std::vector<std::string>> m_lcio2edm_params{this, "LCIO2EMD4hepConversion", {}};

  template <typename T>
  void convertAndRegister(
    const std::string& register_name,
    const std::string& collection_name,
    k4LCIOConverter* lcio_converter,
    podio::CollectionIDTable* id_table) const;

};

#endif