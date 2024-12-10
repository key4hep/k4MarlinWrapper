/*
 * Copyright (c) 2019-2024 Key4hep-Project.
 *
 * This file is part of Key4hep.
 * See https://key4hep.github.io/key4hep-doc/ for further info.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef K4MARLINWRAPPER_LCIO2EDM4HEP_H
#define K4MARLINWRAPPER_LCIO2EDM4HEP_H

#include <Gaudi/Property.h>
#include <GaudiKernel/AlgTool.h>

#include "k4MarlinWrapper/converters/IEDMConverter.h"

#include <lcio.h>

#include <map>
#include <string>
#include <tuple>

namespace podio {
  class CollectionBase;
}

namespace EVENT {
  class LCCollection;
}

class PodioDataSvc;

class Lcio2EDM4hepTool : public AlgTool, virtual public IEDMConverter {
public:
  Lcio2EDM4hepTool(const std::string& type, const std::string& name, const IInterface* parent);
  StatusCode initialize() final;
  StatusCode finalize() final;

  // **********************************
  // - Convert all collections indicated in Tool parameters
  // - Some collections implicitly convert associated collections
  // - Convert associated collections from LCRelation for existing EDM4hep relations
  // - Converted collections are put into TES
  // **********************************
  StatusCode convertCollections(lcio::LCEventImpl* lcio_event) final;

private:
  Gaudi::Property<std::map<std::string, std::string>> m_collNames{this, "collNameMapping", {}};
  Gaudi::Property<bool>                               m_convertAll{this, "convertAll", true};

  ServiceHandle<IDataProviderSvc> m_eds;
  PodioDataSvc*                   m_podioDataSvc;

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
