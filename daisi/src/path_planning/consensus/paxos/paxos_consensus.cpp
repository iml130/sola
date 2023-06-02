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

#include "paxos_consensus.h"

#include <utility>

#include "constants.h"
#include "path_planning/message/serializer.h"
#include "utils/daisi_check.h"

namespace daisi::path_planning::consensus {

PaxosConsensus::PaxosConsensus(std::shared_ptr<sola_ns3::SOLAWrapperNs3> sola, uint32_t node_id,
                               PaxosSettings settings,
                               std::shared_ptr<PathPlanningLoggerNs3> logger)
    : ConsensusBase(std::make_shared<PaxosContainer>(std::move(sola), node_id, std::move(settings),
                                                     std::move(logger))),
      proposer_(container_),
      acceptor_(container_) {
  DAISI_CHECK(
      container_->settings.pickup_active_participate,
      "This class can only be used if the corresponding node is actively participating with Paxos");

  container_->sola->subscribeTopic(kPaxosTopic);

  if (container_->settings.replication) container_->sola->subscribeTopic(kReplicationTopic);
}

void PaxosConsensus::findConsensusImpl(const PointTimePairs &points, double seconds_earliest_start,
                                       std::function<void(uint32_t, double)> success_cb,
                                       std::function<void(uint32_t)> fail_cb) {
  return proposer_.findConsensus(points, seconds_earliest_start, std::move(success_cb),
                                 std::move(fail_cb));
}

void PaxosConsensus::recvTopicMessageImpl(const std::string &topic, const std::string &msg) {
  if (topic == kReplicationTopic) {
    assert(container_->settings.replication);
    auto replication = message::deserialize<ReplicationMessage>(msg);
    container_->logger->logRecvPathPlanningTopicTraffic(
        kReplicationTopic, std::to_string(container_->node_id), replication.instance_id,
        static_cast<uint32_t>(consensus::ConsensusMsgTypes::kReplication));
  } else if (topic == kPaxosTopic) {
    processPaxosMessage(topic, msg);
  } else {
    throw std::runtime_error("invalid topic");
  }
}

void PaxosConsensus::processPaxosMessage(const std::string &topic, const std::string &msg_content) {
  // Depending on topic, forward message to proposer or acceptor
  const auto msg = message::deserialize<PaxosMessage>(msg_content);
  const std::string own_id = std::to_string(container_->node_id);
  if (auto prepare = std::get_if<PrepareMessage>(&msg)) {
    container_->logger->logRecvPathPlanningTopicTraffic(
        topic, own_id, prepare->instance_id, static_cast<uint32_t>(ConsensusMsgTypes::kPrepare));
    acceptor_.processPrepareMessage(*prepare);
  } else if (auto accept = std::get_if<AcceptMessage>(&msg)) {
    container_->logger->logRecvPathPlanningTopicTraffic(
        topic, own_id, accept->instance_id, static_cast<uint32_t>(ConsensusMsgTypes::kAccept));
    acceptor_.processAcceptMessage(*accept);
  } else if (auto promise = std::get_if<PromiseMessage>(&msg)) {
    container_->logger->logRecvPathPlanningTopicTraffic(
        topic, own_id, promise->instance_id, static_cast<uint32_t>(ConsensusMsgTypes::kPromise));
    proposer_.processPromiseMessage(*promise);
  } else if (auto response = std::get_if<ResponseMessage>(&msg)) {
    container_->logger->logRecvPathPlanningTopicTraffic(
        topic, own_id, response->instance_id, static_cast<uint32_t>(ConsensusMsgTypes::kResponse));
    proposer_.processResponseMessage(*response);
  } else if (auto ok = std::get_if<OKMessage>(&msg)) {
    container_->logger->logRecvPathPlanningTopicTraffic(
        topic, own_id, ok->instance_id, static_cast<uint32_t>(ConsensusMsgTypes::kOk));
    proposer_.processOKMessage(*ok);
    acceptor_.processOKMessage(*ok);
  } else {
    throw std::runtime_error("unknown message type");
  }
}

}  // namespace daisi::path_planning::consensus
