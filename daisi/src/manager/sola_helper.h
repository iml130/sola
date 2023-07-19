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

template <typename T> void installApplication(ns3::Ptr<ns3::Node> node) {
  ns3::ObjectFactory factory;
  factory.SetTypeId(T::GetTypeId());
  ns3::Ptr<ns3::Application> app = factory.Create<T>();
  node->AddApplication(app);
}

}  // namespace daisi
#endif
