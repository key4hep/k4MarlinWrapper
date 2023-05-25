#ifndef K4MARLINWRAPPER_K4LCIOREADERWRAPPER_H
#define K4MARLINWRAPPER_K4LCIOREADERWRAPPER_H

// GAUDI
#include <GaudiAlg/GaudiTool.h>

// k4FWCore
#include <k4FWCore/DataHandle.h>
#include <k4FWCore/PodioLegacyDataSvc.h>

// k4LCIOReader
#include <k4LCIOReader/k4LCIOConverter.h>
#include <k4LCIOReader/k4LCIOReader.h>

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

  StatusCode convertCollections(lcio::LCEventImpl* lcio_event) override;

private:
  Gaudi::Property<std::map<std::string, std::string>> m_collNames{this, "collNameMapping", {}};
  Gaudi::Property<bool>                               m_convertAll{this, "convertAll", true};

  ServiceHandle<IDataProviderSvc> m_eds;
  PodioLegacyDataSvc*             m_podioDataSvc;

  bool collectionExist(const std::string& collection_name);

  std::tuple<LCIO2EDM4hepConv::LcioEdmTypeMapping, std::vector<std::tuple<std::string, EVENT::LCCollection*>>,
             std::vector<std::tuple<std::string, EVENT::LCCollection*, std::string>>>
  convertCollectionData(const std::map<std::string, std::string>& collsToConvert, lcio::LCEventImpl* the_event);

  void createSubsetColls(const std::vector<std::tuple<std::string, EVENT::LCCollection*, std::string>>& subsetColls,
                         const LCIO2EDM4hepConv::LcioEdmTypeMapping& lcio2edm4hepMaps);

  void createAssociations(const std::vector<std::tuple<std::string, EVENT::LCCollection*>>& lcRelationColls,
                          const LCIO2EDM4hepConv::LcioEdmTypeMapping&                       lcio2edm4hepMaps);

  template <typename T>
  void convertRegister(const std::string& edm_name, const std::string& lcio_name,
                       std::unique_ptr<k4LCIOConverter>& lcio_conver, const lcio::LCCollection* const lcio_coll,
                       const bool cnv_metadata = false);

  // Non nullptr lcio collection -> convert metadata
  template <typename T>
  void registerCollection(std::tuple<const std::string&, std::unique_ptr<podio::CollectionBase>> namedColl,
                          EVENT::LCCollection*                                                   lcioColl = nullptr);

  template <typename T>
  void registerCollection(const std::string& name, std::unique_ptr<T>&& coll, EVENT::LCCollection* lcioColl = nullptr) {
    registerCollection<T>(std::make_tuple(name, std::move(coll)), lcioColl);
  }
};

#endif
