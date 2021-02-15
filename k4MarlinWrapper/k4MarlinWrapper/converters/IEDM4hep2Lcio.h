#ifndef K4MARLINWRAPPER_IEDM4HEP2LCIO_H
#define K4MARLINWRAPPER_IEDM4HEP2LCIO_H

#include "GaudiKernel/IAlgTool.h"

#include "lcio.h"
#include "IMPL/LCEventImpl.h"

class IEDM4hep2LcioTool : virtual public IAlgTool {
public:

  DeclareInterfaceID( IEDM4hep2LcioTool, 1, 0 );

  virtual StatusCode convertCollections(
    lcio::LCEventImpl* lcio_event) = 0;
};

#endif