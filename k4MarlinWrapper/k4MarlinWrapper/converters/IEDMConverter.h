#ifndef K4MARLINWRAPPER_IEDMCONVERTER_H
#define K4MARLINWRAPPER_IEDMCONVERTER_H

#include "GaudiKernel/IAlgTool.h"

#include "lcio.h"
#include "IMPL/LCEventImpl.h"

class IEDMConverter : virtual public IAlgTool {
public:

  DeclareInterfaceID( IEDMConverter, 1, 0 );

  virtual StatusCode convertCollections(
    lcio::LCEventImpl* lcio_event) = 0;
};

#endif