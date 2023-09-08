/*
 * Copyright (c) 2019-2023 Key4hep-Project.
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
#include "k4MarlinWrapper/TrackingCellIDEncodingSvc.h"

// k4fwcore
#include <k4Interface/IGeoSvc.h>

#include <GaudiKernel/MsgStream.h>

DECLARE_COMPONENT(TrackingCellIDEncodingSvc);

TrackingCellIDEncodingSvc::TrackingCellIDEncodingSvc(const std::string& name, ISvcLocator* svc)
    : base_class(name, svc) {}

TrackingCellIDEncodingSvc::~TrackingCellIDEncodingSvc() {}

StatusCode TrackingCellIDEncodingSvc::initialize() {
  try {
    info() << "Looking for GeoSvc with the name" << m_geoSvcName.value() << endmsg;

    m_geoSvc = serviceLocator()->service(m_geoSvcName);

    info() << "Taking the encoding string as specified in dd4hep constant: " << m_encodingStringVariable.value()
           << endmsg;

    auto encodingString = m_geoSvc->constantAsString(m_encodingStringVariable.value());

    info() << "LCTrackerCellID::_encoding will be set to " << encodingString << endmsg;

    UTIL::LCTrackerCellID::instance().set_encoding_string(encodingString);

    return StatusCode::SUCCESS;

  } catch (std::exception& e) {
    error() << "Failed to set the encoding string!\n";
    error() << e.what() << endmsg;
    return StatusCode::FAILURE;
  }
}

StatusCode TrackingCellIDEncodingSvc::finalize() { return StatusCode::SUCCESS; }
