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

#ifndef DAISI_MANAGER_HELPER_H_
#define DAISI_MANAGER_HELPER_H_

#include "ns3/application-container.h"
#include "ns3/ipv4-address.h"
#include "ns3/object-factory.h"
#include "ns3/uinteger.h"
#include "utils/socket_manager.h"
#include "utils/sola_utils.h"

namespace daisi {

template <typename T> void installApplication(ns3::Ptr<ns3::Node> node) {
  ns3::ObjectFactory factory;
  factory.SetTypeId(T::GetTypeId());
  ns3::Ptr<ns3::Application> app = factory.Create<T>();
  node->AddApplication(app);
}

inline void registerNodes(ns3::NodeContainer container) {
  for (uint64_t i = 0; i < container.GetN(); i++) {
    std::vector<ns3::Ipv4Address> addresses = daisi::getAddressesForNode(container, i);
    SocketManager::get().registerNode(addresses, container.Get(i), 2000);
  }
}

// @brief Helper to setup the same application on all passed nodes,
// starting at 0 seconds
template <typename App> inline void setupApplication(ns3::NodeContainer container) {
  for (uint64_t i = 0; i < container.GetN(); i++) {
    installApplication<App>(container.Get(i));
    container.Get(i)->GetApplication(0)->SetStartTime(ns3::MilliSeconds(0));
  }
}

}  // namespace daisi
#endif
