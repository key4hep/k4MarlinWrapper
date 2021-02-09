#ifndef K4MARLINWRAPPER_IEDM4HEP2LCIO_H
#define K4MARLINWRAPPER_IEDM4HEP2LCIO_H

#include "GaudiKernel/IAlgTool.h"

class IEDM4hep2LcioTool : virtual public IAlgTool {
public:

  DeclareInterfaceID( IEDM4hep2LcioTool, 1, 0 );

  virtual StatusCode convertCollections(
    const Gaudi::Property<std::vector<std::string>>& parameters) = 0;
};

#endif