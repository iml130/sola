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

#ifndef DAISI_CPPS_COMMON_CPPS_COMMUNICATOR_H_
#define DAISI_CPPS_COMMON_CPPS_COMMUNICATOR_H_

#include "SOLA/event_dissemination_minhcast.h"
#include "SOLA/management_overlay_minhton.h"
#include "SOLA/sola.h"
#include "solanet/network_udp/network_udp.h"

namespace daisi::cpps::common {

/// @brief Aggregation object holding communication related members
struct CppsCommunicator {
  /// Constructor required for initializing this struct as a shared ptr.
  /// TODO This constructs solanet::Network and SOLA inplace as long as they are not moveable.
  CppsCommunicator(const sola::ManagementOverlayMinhton::Config &storage_config,
                   const sola::EventDisseminationMinhcast::Config &event_dissemination_config,
                   sola::TopicMessageReceiveFct topic_recv, sola::LoggerPtr logger,
                   const std::function<void(const solanet::Message &)> &callback)
      : sola(storage_config, event_dissemination_config, std::move(topic_recv), logger),
        network(callback) {}

  sola::SOLA<sola::ManagementOverlayMinhton, sola::EventDisseminationMinhcast> sola;
  solanet::Network network;
};

using CppsCommunicatorPtr = std::shared_ptr<daisi::cpps::common::CppsCommunicator>;

}  // namespace daisi::cpps::common

#endif
