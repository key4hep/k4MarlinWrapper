#ifndef K4MARLINWRAPPER_EDMCOLLECTIONWRAPPER_H
#define K4MARLINWRAPPER_EDMCOLLECTIONWRAPPER_H

#include <GaudiKernel/DataObject.h>

#include <type_traits>

#include "podio/CollectionBase.h"

template <class T>
class EDMCollectionWrapper : public DataObject {
public:
  EDMCollectionWrapper(T* podio_collection) {
    static_assert(std::is_base_of<podio::CollectionBase, T>::value);
    m_podio_collection = podio_collection;
  }

  ~EDMCollectionWrapper(){};

  T* m_podio_collection = nullptr;
};

#endif