#ifndef K4MARLINWRAPPER_K4LCIOREADERWRAPPER_H
#define K4MARLINWRAPPER_K4LCIOREADERWRAPPER_H

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
   * Convert the collection data and return the mapping of LCIO to EDM4hep
   * objects, the LCRelation collections (and their names) as well as the subset
   * collections (with their names and types). These data are necessary for
   * resolving relations and creating association collections.
   *
   * The converted collections are put into the TES (but the contained objects
   * have no relations set yet).
   */
  std::tuple<LCIO2EDM4hepConv::LcioEdmTypeMapping, std::vector<std::tuple<std::string, EVENT::LCCollection*>>,
             std::vector<std::tuple<std::string, EVENT::LCCollection*, std::string>>>
  convertCollectionData(const std::map<std::string, std::string>& collsToConvert, lcio::LCEventImpl* the_event);

  /**
   * Create the subset collections and put them into the TES.
   */
  void createSubsetColls(const std::vector<std::tuple<std::string, EVENT::LCCollection*, std::string>>& subsetColls,
                         const LCIO2EDM4hepConv::LcioEdmTypeMapping& lcio2edm4hepMaps);

  /**
   * Create the association collections from the LCRelation collections and put
   * them into the TES.
   */
  void createAssociations(const std::vector<std::tuple<std::string, EVENT::LCCollection*>>& lcRelationColls,
                          const LCIO2EDM4hepConv::LcioEdmTypeMapping&                       lcio2edm4hepMaps);

  /**
   * Register a collection into the TES. If the lcioColl is not a nullptr also
   * convert the metadata from the input lcio collection.
   */
  template <typename T>
  void registerCollection(std::tuple<const std::string&, std::unique_ptr<podio::CollectionBase>> namedColl,
                          EVENT::LCCollection*                                                   lcioColl = nullptr);

  /**
    * Register a collection into the TES. If the lcioColl is not a nullptr also
    * convert the metadata from the input lcio collection.
    */
  template <typename T>
  void registerCollection(const std::string& name, std::unique_ptr<T>&& coll, EVENT::LCCollection* lcioColl = nullptr) {
    registerCollection<T>(std::make_tuple(name, std::move(coll)), lcioColl);
  }
};

#endif
