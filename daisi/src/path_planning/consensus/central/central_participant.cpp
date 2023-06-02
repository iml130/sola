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

#include "central_participant.h"

#include <utility>

#include "path_planning/message/serializer.h"
#include "utils/sola_utils.h"

namespace daisi::path_planning::consensus {

CentralParticipant::CentralParticipant(const std::shared_ptr<sola_ns3::SOLAWrapperNs3> &sola,
                                       const CentralSettings &settings,
                                       std::shared_ptr<PathPlanningLoggerNs3> logger)
    : ConsensusBase(std::make_shared<CentralSettings>(settings)), logger_(std::move(logger)) {
  // Create new network interface with same IP as SOLA
  std::string endpoint = sola->getConectionString();
  std::string ip = endpoint.substr(0, endpoint.find(':'));
  SolaNetworkUtils::get().createSockets(ip);
  network_ = std::make_unique<solanet::Network>(
      ip, [this](const solanet::Message &msg) { processMessage(msg); });
}
void CentralParticipant::processMessage(const solanet::Message &msg) {
  auto response = message::deserialize<Response>(msg.getMessage());
  logger_->logRecvPathPlanningTraffic("CENTRAL", "PLACEHOLDER", 5);

  assert(requests_.find(response.request_id) != requests_.end());

  // Call the callback, depending on whether the request was successful or failed
  if (!response.success) {
    requests_[response.request_id].fail_cb(0);
  } else {
    requests_[response.request_id].success_cb(0, response.start_offset);
  }

  // Delete request from outstanding requests
  requests_.erase(response.request_id);
}

void CentralParticipant::findConsensusImpl(const PointTimePairs &points,
                                           double seconds_earliest_start,
                                           std::function<void(uint32_t, double)> success_cb,
                                           std::function<void(uint32_t)> fail_cb) {
  std::vector<std::tuple<float, float, double>> intersections;  // Contains relative times
  for (auto point : points) {
    intersections.emplace_back(point.first.x, point.first.y, point.second);
  }

  CurrentRequest req{next_request_id_++, std::move(success_cb), std::move(fail_cb), intersections};

  // Global earliest start time
  const uint64_t earliest_start_ms = ns3::Simulator::Now().GetMilliSeconds() +
                                     static_cast<uint64_t>(seconds_earliest_start * 1000);

  // Send request to server
  Request req_msg{req.request_id, earliest_start_ms, intersections};

  requests_[req.request_id] = std::move(req);

  solanet::Message msg(container_->server_ip, container_->server_port,
                       message::serialize<Request>(req_msg));
  network_->send(msg);
  logger_->logSendPathPlanningTraffic("PLACEHOLDER", "CENTRAL", 4);
}

void CentralParticipant::recvTopicMessageImpl(const std::string &topic, const std::string &msg) {
  throw std::runtime_error("Central should not receive topic messages");
}

}  // namespace daisi::path_planning::consensus
