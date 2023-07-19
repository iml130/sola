// Copyright 2023 The SOLA authors
//
// This file is part of DAISI.
//
// DAISI is free software: you can redistribute it and/or modify it under the terms of the GNU
// General Public License as published by the Free Software Foundation; version 2.
//
// DAISI is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even
// the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
// Public License for more details.
//
// You should have received a copy of the GNU General Public License along with DAISI. If not, see
// <https://www.gnu.org/licenses/>.
//
// SPDX-License-Identifier: GPL-2.0-only

#include "cpps/common/cpps_application.h"

#include "../src/logging/logger.h"  // WORKAROUND: private header
#include "utils/socket_manager.h"

using namespace ns3;

namespace daisi::cpps {
ns3::TypeId CppsApplication::GetTypeId() {
  static TypeId tid =
      TypeId("CppsApplication").SetParent<Application>().AddConstructor<CppsApplication>();
  return tid;
}

void CppsApplication::cleanup() {
  application = std::monostate{};
  SocketManager::get().unregisterNode(GetNode());
}

void CppsApplication::init() {
  if (auto amr_logical_agent =
          std::get_if<std::shared_ptr<logical::AmrLogicalAgent>>(&application)) {
    (*amr_logical_agent)->init();
  } else if (auto mf_logical_agent =
                 std::get_if<std::shared_ptr<logical::MaterialFlowLogicalAgent>>(&application)) {
    (*mf_logical_agent)->init();
  } else if (auto amr_physical_asset =
                 std::get_if<std::shared_ptr<AmrPhysicalAsset>>(&application)) {
    (*amr_physical_asset)->init();
  }
}

void CppsApplication::start() {
  if (auto amr_logical_agent =
          std::get_if<std::shared_ptr<logical::AmrLogicalAgent>>(&application)) {
    (*amr_logical_agent)->start();
  } else if (auto mf_logical_agent =
                 std::get_if<std::shared_ptr<logical::MaterialFlowLogicalAgent>>(&application)) {
    (*mf_logical_agent)->start();
  }
}

}  // namespace daisi::cpps
