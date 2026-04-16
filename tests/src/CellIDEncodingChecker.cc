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

#include <k4FWCore/MetadataUtils.h>

#include <Gaudi/Algorithm.h>
#include <GaudiKernel/StatusCode.h>

#include <map>
#include <string>

class CellIDEncodingChecker final : public Gaudi::Algorithm {
public:
  CellIDEncodingChecker(const std::string& name, ISvcLocator* pSL) : Gaudi::Algorithm(name, pSL) {}

  StatusCode initialize() override {
    if (Gaudi::Algorithm::initialize().isFailure()) {
      return StatusCode::FAILURE;
    }
    for (const auto& [collName, expectedEncoding] : m_expectedEncodings) {
      const auto encoding = k4FWCore::getCellIDEncoding(collName, this);
      if (!encoding.has_value()) {
        error() << "No CellID encoding found for collection '" << collName << "'" << endmsg;
        return StatusCode::FAILURE;
      }
      if (encoding.value() != expectedEncoding) {
        error() << "CellID encoding mismatch for collection '" << collName << "': expected '" << expectedEncoding
                << "', got '" << encoding.value() << "'" << endmsg;
        return StatusCode::FAILURE;
      }
      info() << "CellID encoding for '" << collName << "' is correct: '" << encoding.value() << "'" << endmsg;
    }
    return StatusCode::SUCCESS;
  }

  StatusCode execute(const EventContext&) const override { return StatusCode::SUCCESS; }

private:
  Gaudi::Property<std::map<std::string, std::string>> m_expectedEncodings{
      this, "ExpectedEncodings", {}, "Map of collection names to expected CellID encoding strings"};
};

DECLARE_COMPONENT(CellIDEncodingChecker)
