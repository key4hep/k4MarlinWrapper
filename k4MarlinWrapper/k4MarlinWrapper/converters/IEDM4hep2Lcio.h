#pragma once

#include "GaudiKernel/IAlgTool.h"

class IEDM4hep2LcioTool : virtual public IAlgTool {
public:

  DeclareInterfaceID( IEDM4hep2LcioTool, 1, 0 );

  virtual StatusCode convertCollections(
    const Gaudi::Property<std::vector<std::string>>& parameters,
    lcio::LCEventImpl* lcio_event) = 0;
};