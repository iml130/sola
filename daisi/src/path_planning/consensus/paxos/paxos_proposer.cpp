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

#include "paxos_proposer.h"

#include <utility>

#include "constants.h"
#include "path_planning/consensus/paxos/message/prepare_message.h"
#include "path_planning/consensus/route_calculation_helper.h"
#include "path_planning/intersection_set.h"
#include "path_planning/message/serializer.h"
#include "utils/daisi_check.h"

namespace daisi::path_planning::consensus {
PaxosProposer::PaxosProposer(std::shared_ptr<PaxosContainer> container)
    : container_(std::move(container)) {}

double PaxosProposer::calculatePossibleStartTime(const PointTimePairs &points,
                                                 const double seconds_till_start) const {
  // Global earliest start time
  double earliest_start_s =
      (ns3::Simulator::Now().GetMilliSeconds() + seconds_till_start * 1000) / 1000.0;

  double possible_start_s = RouteCalculationHelper::calculatePossibleStartTime(
      points, earliest_start_s, container_->settings.max_preplanning_time,
      container_->settings.time_delta_intersections, container_->agreed_data);
  return possible_start_s;
}

void PaxosProposer::findConsensus(const PointTimePairs &points, double seconds_till_start,
                                  std::function<void(uint32_t, double)> success_cb,
                                  std::function<void(uint32_t)> fail_cb) {
  // Calculate possible start time
  double possible_start_s = calculatePossibleStartTime(points, seconds_till_start);
  if (std::isnan(possible_start_s)) {
    fail_cb(0);
    return;
  }

  // Next instance ID is the next free list index of accepted data
  uint32_t instance = container_->finally_accepted_data.size();

  // Get next proposal ID for the current instance
  uint32_t proposal_id = instance_to_proposal_id_[instance]++;

  // Timeout after 500 milliseconds if we did not get any quorum
  const uint32_t timeout_ms = 500;
  ns3::EventId timeout_event = ns3::Simulator::Schedule(
      ns3::MilliSeconds(timeout_ms), &PaxosProposer::prepareTimeout, this, proposal_id);

  // Currently our quorum contains all other participants (except ourselves)
  const uint32_t needed_for_quorum = container_->settings.number_paxos_participants - 1;
  const uint32_t outstanding_oks = container_->settings.number_paxos_participants - 1;
  DAISI_CHECK(!current_proposal_.initialized(),
              "We cannot start a new proposal while one proposal of ourselves is still running");
  current_proposal_ = Proposal{points,
                               possible_start_s,
                               instance,
                               proposal_id,
                               needed_for_quorum,
                               outstanding_oks,
                               timeout_event,
                               Phase::kPrepare,
                               std::move(success_cb),
                               std::move(fail_cb)};

  // Create prepare message
  PrepareMessage msg{instance, current_proposal_.proposal_id, container_->node_id};

  // Prepare on ourselves. If that is not even possible do not send any prepares to acceptors
  {
    std::pair<uint32_t, uint32_t> prepare = {msg.prepare_id, msg.station_id};

    // Get the prepare/station ID for the instance where we (our acceptor) already promised for
    auto it = container_->instance_to_promised.find(msg.instance_id);

    if (it == container_->instance_to_promised.end() || prepare > it->second) {
      // Our prepare is newer/greater -> Promise ourselves and save
      container_->instance_to_promised[msg.instance_id] = prepare;
    } else {
      // We do not promise ourselves
      // Wait for timeout
      return;
    }
  }

  // Send prepare message
  container_->sola->publishMessage(kPaxosTopic, message::serialize<PaxosMessage>(msg));
  container_->logger->logSendPathPlanningTopicTraffic(
      kPaxosTopic, std::to_string(container_->node_id), msg.instance_id,
      static_cast<uint32_t>(ConsensusMsgTypes::kPrepare));
}

void PaxosProposer::prepareTimeout(uint32_t proposal_id) {
  if (!current_proposal_.initialized() || current_proposal_.proposal_id != proposal_id) {
    // Already finished
    return;
  }

  if (current_proposal_.needed_for_quorum != 0) {
    // Failed to get quorum
    current_proposal_.fail_cb(0);
    current_proposal_ = Proposal{};
  } else if (current_proposal_.outstanding_oks != 0) {
    // Got quorum -> timeout with oks
    current_proposal_.fail_cb(0);
    current_proposal_ = Proposal{};
  }
}

void PaxosProposer::processPromiseMessage(const PromiseMessage &msg) {
  if (msg.station_id != container_->node_id) {
    // Promise not meant for us
    return;
  }
  if (!current_proposal_.initialized() || current_proposal_.needed_for_quorum == 0) {
    // Already timed out or got quorum.
    // If we already got a quorum, this is a duplicate message and should not happen.
    return;
  }

  if (msg.already_accepted && instance_to_proposal_id_[msg.instance_id] < msg.accepted_prepare_id) {
    // If we need to retry later, set the proposal ID one greater than the latest known proposal ID
    instance_to_proposal_id_[msg.instance_id] = msg.accepted_prepare_id + 1;
  }

  if (current_proposal_.needed_for_quorum > 0) {
    current_proposal_.needed_for_quorum--;
  }
  if (current_proposal_.needed_for_quorum == 0) {
    // Got quorum -> Change to accept phase
    current_proposal_.phase = Phase::kAccept;

    // cancel timeout event
    ns3::Simulator::Cancel(current_proposal_.timeout_event);

    // setup timeout for response
    current_proposal_.timeout_event =
        ns3::Simulator::Schedule(ns3::MilliSeconds(500), &PaxosProposer::prepareTimeout, this,
                                 current_proposal_.proposal_id);

    std::pair<uint32_t, uint32_t> sender = {msg.prepare_id, msg.station_id};
    if (sender != container_->instance_to_promised[msg.instance_id]) {
      // No longer promised ourselves
      // Wait for timeout
      return;
    }

    // insert into own data for current accepted instance
    std::vector<IntersectionTimeInfo> info;
    for (auto &entry : current_proposal_.points) {
      info.push_back({{entry.first.x, entry.first.y}, entry.second});
    }

    container_->accepted_data_current_instance = AcceptedProposal{
        current_proposal_.instance_id, current_proposal_.proposal_id, container_->node_id, info};

    // send accepts
    std::vector<IntersectionTimeInfo> intersects;
    for (auto point : current_proposal_.points) {
      intersects.push_back(
          {{point.first.x, point.first.y}, point.second + current_proposal_.start_time});
    }
    AcceptMessage accept{current_proposal_.instance_id, current_proposal_.proposal_id,
                         container_->node_id, intersects};

    // After the OK phase all acceptors will send us an response
    remaining_responses_ = container_->settings.number_paxos_participants;

    container_->sola->publishMessage(kPaxosTopic, message::serialize<PaxosMessage>(accept));
    container_->logger->logSendPathPlanningTopicTraffic(
        kPaxosTopic, std::to_string(container_->node_id), accept.instance_id,
        static_cast<uint32_t>(consensus::ConsensusMsgTypes::kAccept));
  }
}

void PaxosProposer::processResponseMessage(const ResponseMessage &msg) {
  if (msg.station_id != container_->node_id) {
    // Not for us. We are not the initiator.
    return;
  }

  remaining_responses_--;
  if (remaining_responses_ == 0) {
    // Got response from all acceptors. We can now accept
    current_proposal_.phase = Phase::kFinished;
    current_proposal_.success_cb(current_proposal_.instance_id, current_proposal_.start_time);
    ns3::Simulator::Cancel(current_proposal_.timeout_event);
    current_proposal_ = Proposal{};
  }
}

void PaxosProposer::processOKMessage(const OKMessage &msg) {
  if (msg.station_id != container_->node_id) {
    // Message should go to acceptor. We are not initiator
    return;
  }

  current_proposal_.outstanding_oks--;
  if (current_proposal_.outstanding_oks == 0) {
    // "Send" a response message to ourselves
    processResponseMessage({msg.instance_id, msg.proposal_id, msg.station_id});

    // Set finalized data
    container_->finally_accepted_data.push_back(container_->accepted_data_current_instance.data);
    container_->accepted_data_current_instance = AcceptedProposal{};
    for (auto point : current_proposal_.points) {
      // Verify that point is actually an intersection in this scenario
      DAISI_CHECK(getAllIntersections().count(point.first) == 1, "Invalid point");
      container_->agreed_data[{point.first.x, point.first.y}]
                             [point.second + current_proposal_.start_time] = container_->node_id;
    }
  }
}

}  // namespace daisi::path_planning::consensus
