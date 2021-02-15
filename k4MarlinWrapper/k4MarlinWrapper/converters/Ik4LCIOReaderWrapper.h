#ifndef K4MARLINWRAPPER_IK4LCIOREADERWRAPPER_H
#define K4MARLINWRAPPER_IK4LCIOREADERWRAPPER_H

#include "GaudiKernel/IAlgTool.h"

#include "lcio.h"
#include "IMPL/LCEventImpl.h"

class Ik4LCIOReaderWrapper : virtual public IAlgTool {
public:

  DeclareInterfaceID( Ik4LCIOReaderWrapper, 1, 0 );

  virtual StatusCode convertCollections(
    lcio::LCEventImpl* lcio_event) = 0;
};

#endif