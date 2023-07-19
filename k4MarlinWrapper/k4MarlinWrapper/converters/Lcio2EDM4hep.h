#ifndef K4MARLINWRAPPER_LCIO2EDM4HEP_H
#define K4MARLINWRAPPER_LCIO2EDM4HEP_H

// GAUDI
#include <GaudiAlg/GaudiTool.h>

// k4FWCore
#include <k4FWCore/DataHandle.h>
#include <k4FWCore/PodioLegacyDataSvc.h>

// Converter Interface
#include "k4MarlinWrapper/converters/IEDMConverter.h"

#include <map>
#include <string>
#include <tuple>
#include <vector>

namespace podio {
  class CollectionBase;
}

namespace LCIO2EDM4hepConv {
  struct LcioEdmTypeMapping;
}

namespace EVENT {
  class LCCollection;
}

class Lcio2EDM4hepTool : public GaudiTool, virtual public IEDMConverter {
public:
  Lcio2EDM4hepTool(const std::string& type, const std::string& name, const IInterface* parent);
  virtual ~Lcio2EDM4hepTool();
  virtual StatusCode initialize();
  virtual StatusCode finalize();

  // **********************************
  // - Convert all collections indicated in Tool parameters
  // - Some collections implicitly convert associated collections
  // - Convert associated collections from LCRelation for existing EDM4hep relations
  // - Converted collections are put into TES
  // **********************************
  StatusCode convertCollections(lcio::LCEventImpl* lcio_event) override;

private:
  Gaudi::Property<std::map<std::string, std::string>> m_collNames{this, "collNameMapping", {}};
  Gaudi::Property<bool>                               m_convertAll{this, "convertAll", true};

  ServiceHandle<IDataProviderSvc> m_eds;
  PodioLegacyDataSvc*             m_podioDataSvc;

  // **********************************
  // Check if a collection was already registered to skip it
  // **********************************
  bool collectionExist(const std::string& collection_name);

  /**
   * Register a collection into the TES. If the lcioColl is not a nullptr also
   * convert the metadata from the input lcio collection.
   */
  void registerCollection(std::tuple<const std::string&, std::unique_ptr<podio::CollectionBase>> namedColl,
                          EVENT::LCCollection*                                                   lcioColl = nullptr);

  /**
    * Register a collection into the TES. If the lcioColl is not a nullptr also
    * convert the metadata from the input lcio collection.
    */
  void registerCollection(const std::string& name, std::unique_ptr<podio::CollectionBase>&& coll,
                          EVENT::LCCollection* lcioColl = nullptr) {
    registerCollection(std::make_tuple(name, std::move(coll)), lcioColl);
  }
};

#endif
