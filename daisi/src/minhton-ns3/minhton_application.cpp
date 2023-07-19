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

#include "minhton_application.h"

#include "ns3/uinteger.h"
#include "solanet-ns3/sola_message_ns3.h"
#include "utils/sola_utils.h"

using namespace ns3;

namespace minhton {
extern std::shared_ptr<minhton::LoggerInterface> logger;
}
namespace daisi::minhton_ns3 {

TypeId MinhtonApplication::GetTypeId() {
  static TypeId tid =
      TypeId("MinhtonApp").SetParent<Application>().AddConstructor<MinhtonApplication>();
  return tid;
}

Ptr<MinhtonNodeNs3> MinhtonApplication::getMinhtonNode() { return minhton_node_; }

void MinhtonApplication::DoDispose() {
  StopApplication();
  Application::DoDispose();
}

void MinhtonApplication::StartApplication() {
  logger_ = daisi::global_logger_manager->createMinhtonLogger(GetNode()->GetId());
}

void MinhtonApplication::StopApplication() {}

void MinhtonApplication::initializeNode(minhton::ConfigNode config) {
  config.setLogger({logger_});

  minhton_node_ = Create<MinhtonNodeNs3>(config);
}

}  // namespace daisi::minhton_ns3
