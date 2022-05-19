#ifndef K4MARLINWRAPPER_K4LCIOREADERWRAPPER_H
#define K4MARLINWRAPPER_K4LCIOREADERWRAPPER_H

// GAUDI
#include <GaudiAlg/GaudiTool.h>

// k4FWCore
#include <k4FWCore/DataHandle.h>
#include <k4FWCore/PodioDataSvc.h>

// k4LCIOReader
#include <k4LCIOReader/k4LCIOConverter.h>
#include <k4LCIOReader/k4LCIOReader.h>

// Converter Interface
#include "k4MarlinWrapper/converters/IEDMConverter.h"

class Lcio2EDM4hepTool : public GaudiTool, virtual public IEDMConverter {
public:
  Lcio2EDM4hepTool(const std::string& type, const std::string& name, const IInterface* parent);
  virtual ~Lcio2EDM4hepTool();
  virtual StatusCode initialize();
  virtual StatusCode finalize();

  StatusCode convertCollections(lcio::LCEventImpl* lcio_event);

private:
  Gaudi::Property<std::vector<std::string>> m_params{this, "Parameters", {}};

  std::map<std::string, DataObjectHandleBase*> m_dataHandlesMap;

  ServiceHandle<IDataProviderSvc> m_eds;
  PodioDataSvc*                   m_podioDataSvc;

  bool collectionExist(const std::string& collection_name);

  template <typename T>
  void convertRegister(const std::string& edm_name, const std::string& lcio_name,
                       std::unique_ptr<k4LCIOConverter>& lcio_conver, const lcio::LCCollection* const lcio_coll,
                       const bool cnv_metadata = false);
};

#endif
