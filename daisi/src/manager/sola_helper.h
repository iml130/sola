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

namespace daisi {

template <typename T> class SolaHelper {
public:
  SolaHelper(std::vector<ns3::Ipv4Address> local_ip_address, uint16_t listening_port) {
    assert(!local_ip_address.empty() && local_ip_address.size() <= 2);
    factory_.SetTypeId(T::GetTypeId());
    setAttribute("LocalIpAddress", ns3::Ipv4AddressValue(local_ip_address[0]));
    setAttribute("ListeningPort", ns3::UintegerValue(listening_port));

    if (local_ip_address.size() == 2) {
      setAttribute("LocalIpAddressTCP", ns3::Ipv4AddressValue(local_ip_address[1]));
      setAttribute("ListeningPortTCP", ns3::UintegerValue(3000));
    }
  }

  ns3::ApplicationContainer install(ns3::Ptr<ns3::Node> node) const {
    ns3::Ptr<ns3::Application> app = factory_.Create<T>();
    node->AddApplication(app);
    return {app};
  }

private:
  void setAttribute(std::string name, const ns3::AttributeValue &value) {
    factory_.Set(name, value);
  }

  ns3::ObjectFactory factory_;
};

}  // namespace daisi
#endif
