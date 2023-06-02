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

#include "central_server.h"

#include <ns3/simulator.h>

#include <algorithm>
#include <cmath>
#include <set>
#include <utility>

#include "cpps/common/uuid_generator.h"
#include "path_planning/consensus/route_calculation_helper.h"
#include "path_planning/message/serializer.h"

namespace daisi::path_planning::consensus {

CentralServer::CentralServer(CentralSettings settings,
                             std::shared_ptr<PathPlanningLoggerNs3> logger)
    : settings_(std::move(settings)), logger_(std::move(logger)) {
  network_ = std::make_unique<solanet::Network>(
      [this](const solanet::Message &msg) { processMessage(msg); });
  logger_->setApplicationUUID(UUIDGenerator::get()());
}

void CentralServer::setIntersections(const std::set<PPVector> &intersections) {
  for (auto intersection : intersections) {
    intersection_occupancy_[intersection] = {};
  }
}

void CentralServer::processMessage(const solanet::Message &msg) {
  auto request = message::deserialize<Request>(msg.getMessage());
  logger_->logRecvPathPlanningTraffic("PLACEHOLDER", "CENTRAL", 4);

  // Calculate possible start time
  double earliest_start_s = request.earliest_possible_start_ms / 1000.0;

  PointTimePairs points;
  std::transform(request.intersection_times.cbegin(), request.intersection_times.cend(),
                 std::back_inserter(points), [](const auto &point) -> PointTimePairs::value_type {
                   const ns3::Vector2D vec = {std::get<0>(point), std::get<1>(point)};
                   const float time = std::get<2>(point);
                   return {vec, time};
                 });

  double possible_start_s = RouteCalculationHelper::calculatePossibleStartTime(
      points, earliest_start_s, settings_.max_preplanning_time, settings_.time_between_intersects,
      intersection_occupancy_);

  // Send response to client
  Response response{};
  response.request_id = request.request_id;

  if (std::isnan(possible_start_s)) {
    response.success = false;
  } else {
    response.success = true;
    response.start_offset = possible_start_s;

    // Update our global intersection knowledge
    for (auto point : request.intersection_times) {
      intersection_occupancy_.at(
          {std::get<0>(point), std::get<1>(point)})[possible_start_s + std::get<2>(point)] =
          0;  // id = 0 just to mark as occupied
    }
  }

  solanet::Message net_msg(msg.getIp(), msg.getPort(), message::serialize<Response>(response));
  network_->send(net_msg);
  logger_->logSendPathPlanningTraffic("CENTRAL", "PLACEHOLDER", 5);
}

}  // namespace daisi::path_planning::consensus
