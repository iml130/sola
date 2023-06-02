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

#include "paxos_acceptor.h"

#include "constants.h"
#include "path_planning/consensus/paxos/message/promise_message.h"
#include "path_planning/intersection_set.h"
#include "path_planning/message/serializer.h"
#include "utils/daisi_check.h"

namespace daisi::path_planning::consensus {

PaxosAcceptor::PaxosAcceptor(std::shared_ptr<PaxosContainer> container)
    : container_(std::move(container)) {}

void PaxosAcceptor::processPrepareMessage(const PrepareMessage &msg) {
  auto it = container_->instance_to_promised.find(msg.instance_id);
  std::pair<uint32_t, uint32_t> prepare = {msg.prepare_id, msg.station_id};

  if (it == container_->instance_to_promised.end() || prepare > it->second) {
    // New / Greater -> Promise and save
    container_->instance_to_promised[msg.instance_id] = prepare;
    PromiseMessage promise{msg.instance_id, msg.prepare_id, msg.station_id};
    uint32_t current_instance = container_->finally_accepted_data.size();
    if (current_instance == msg.instance_id &&
        container_->accepted_data_current_instance.initialized()) {
      promise.already_accepted = true;
      promise.accepted_prepare_id = container_->accepted_data_current_instance.proposal_id;
      promise.accepted_station_id = container_->accepted_data_current_instance.station_id;
    }

    if (current_instance > msg.instance_id) {
      // There is definitely already some decided values for all smaller instances
      // Do not send a promise
      return;
    }

    container_->sola->publishMessage(kPaxosTopic, message::serialize<PaxosMessage>(promise));
    container_->logger->logSendPathPlanningTopicTraffic(
        kPaxosTopic, std::to_string(container_->node_id), promise.instance_id,
        static_cast<uint32_t>(ConsensusMsgTypes::kPromise));
  }
}

void PaxosAcceptor::processAcceptMessage(const AcceptMessage &msg) {
  std::pair<uint32_t, uint32_t> sender = {msg.proposal_id, msg.station_id};
  if (sender != container_->instance_to_promised[msg.instance_id]) {
    // Not (longer) promised
    return;
  }

  // Insert into own data
  assert(!msg.intersections.empty());
  container_->accepted_data_current_instance =
      AcceptedProposal{msg.instance_id, msg.proposal_id, msg.station_id, msg.intersections};
  current_accepted_instance_ = msg.instance_id;
  current_accepted_proposal_id_ = msg.proposal_id;
  current_accepted_station_ = msg.station_id;
  remaining_oks_ =
      container_->settings.number_paxos_participants - 2;  // Excluding initial sender and own

  auto it = std::find_if(already_received_.begin(), already_received_.end(),
                         [msg](const AlreadyReceivedOKS &ok) {
                           return ok.proposal_id == msg.proposal_id &&
                                  ok.instance == msg.instance_id && ok.station == msg.station_id;
                         });
  if (it != already_received_.end()) {
    remaining_oks_ = remaining_oks_ - it->already_received;
    already_received_.erase(it);
  }

  // Send ok
  OKMessage ok{msg.instance_id, msg.proposal_id, msg.station_id, container_->node_id};
  container_->sola->publishMessage(kPaxosTopic, message::serialize<PaxosMessage>(ok));
  container_->logger->logSendPathPlanningTopicTraffic(
      kPaxosTopic, std::to_string(container_->node_id), ok.instance_id,
      static_cast<uint32_t>(ConsensusMsgTypes::kOk));

  if (remaining_oks_ == 0) {
    sendResponseMessage();
  }
}
void PaxosAcceptor::processOKMessage(const OKMessage &msg) {
  if (msg.station_id == container_->node_id)
    return;  // Message should go to proposer. We are initiator

  if (!container_->accepted_data_current_instance.initialized() ||
      container_->accepted_data_current_instance.instance_id != msg.instance_id ||
      container_->accepted_data_current_instance.proposal_id != msg.proposal_id ||
      container_->accepted_data_current_instance.station_id != msg.station_id) {
    // Temporary save
    auto it = std::find_if(already_received_.begin(), already_received_.end(),
                           [msg](const AlreadyReceivedOKS &ok) {
                             return ok.proposal_id == msg.proposal_id &&
                                    ok.instance == msg.instance_id && ok.station == msg.station_id;
                           });
    if (it != already_received_.end()) {
      it->already_received++;
      assert(it->already_received <= 6);
    } else {
      already_received_.push_back({msg.instance_id, msg.proposal_id, msg.station_id, 1});
    }
    return;
  }

  if (msg.instance_id != current_accepted_instance_ ||
      msg.proposal_id != current_accepted_proposal_id_ ||
      msg.station_id != current_accepted_station_) {
    throw std::runtime_error("values not equal");
  }

  assert(msg.instance_id == current_accepted_instance_);
  assert(msg.proposal_id == current_accepted_proposal_id_);
  assert(msg.station_id == current_accepted_station_);

  remaining_oks_--;
  if (remaining_oks_ == 0) {
    sendResponseMessage();
  }
}
void PaxosAcceptor::sendResponseMessage() {
  container_->finally_accepted_data.push_back(container_->accepted_data_current_instance.data);

  if (container_->finally_accepted_data.size() - 1 != current_accepted_instance_)
    throw std::runtime_error("incorrect size");

  container_->accepted_data_current_instance = AcceptedProposal{};
  ResponseMessage response{current_accepted_instance_, current_accepted_proposal_id_,
                           current_accepted_station_};

  for (auto point :
       container_->finally_accepted_data[container_->finally_accepted_data.size() - 1]) {
    // Verify that point is actually an intersection in this scenario
    DAISI_CHECK(
        getAllIntersections().count({point.intersection.first, point.intersection.second}) == 1,
        "Invalid point");
    container_->agreed_data[point.intersection][point.time_at_intersection] = container_->node_id;
  }

  // Send for replication
  if (container_->settings.replication) {
    ReplicationMessage replication{current_accepted_instance_, current_accepted_proposal_id_,
                                   current_accepted_station_};
    container_->sola->publishMessage(kReplicationTopic, message::serialize<>(replication));
    container_->logger->logSendPathPlanningTopicTraffic(
        kReplicationTopic, std::to_string(container_->node_id), current_accepted_instance_,
        static_cast<uint32_t>(ConsensusMsgTypes::kReplication));
  }

  container_->instance_to_promised[current_accepted_instance_] = {
      UINT32_MAX, UINT32_MAX};  // Value is decided, block other values

  current_accepted_instance_ = UINT32_MAX;
  current_accepted_proposal_id_ = UINT32_MAX;
  current_accepted_station_ = UINT32_MAX;

  container_->sola->publishMessage(kPaxosTopic, message::serialize<PaxosMessage>(response));
  container_->logger->logSendPathPlanningTopicTraffic(
      kPaxosTopic, std::to_string(container_->node_id), response.instance_id,
      static_cast<uint32_t>(ConsensusMsgTypes::kResponse));
}

}  // namespace daisi::path_planning::consensus
