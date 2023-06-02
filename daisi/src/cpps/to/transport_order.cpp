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

#include "cpps/to/transport_order.h"

#include <cassert>
#include <memory>

#include "cpps/message/serializer.h"
#include "cpps/negotiation/initiator/iterated_auction_initiator_pubsub_modified.h"
#include "cpps/negotiation/initiator/iterated_auction_initiator_pubsub_tepssi.h"
#include "minhton/utils/config_reader.h"
#include "natter/uuid.h"
#include "ns3/log.h"
#include "utils/sola_utils.h"

namespace daisi::cpps {

std::ostream &operator<<(std::ostream &os, const TransportOrderApplicationNs3 &to) { return os; }

TransportOrderApplicationNs3::TransportOrderApplicationNs3(const MRTAConfig &mrta_config,
                                                           uint32_t device_id)
    : logger_(daisi::global_logger_manager->createTOLogger(device_id)),
      device_id_(device_id),
      mrta_config_(mrta_config),
      first_node_(false),
      waiting_for_start_(false) {}

void TransportOrderApplicationNs3::clearMaterialFlowInformation() {
  material_flow_model_ = nullptr;
  task_allocation_initiator_ = nullptr;

  logger_->logMaterialFlow(ns3::Simulator::Now().GetMilliSeconds(), uuid_, sola_->getIP(),
                           sola_->getPort(), 1);
}

bool TransportOrderApplicationNs3::isBusy() const {
  return task_allocation_initiator_ != nullptr || waiting_for_start_;
}

void TransportOrderApplicationNs3::init() {
  const std::string config_file =
      first_node_ ? "configurations/root.yml" : "configurations/join.yml";

  sola::ManagementOverlayMinhton::Config config_mo = minhton::config::readConfig(config_file);

  if (!first_node_) {
    sola_ns3::SOLAWrapperNs3::setJoinIp(config_mo);
  }

  // Nothing to configure (yet)
  sola::EventDisseminationMinhcast::Config config_ed;

  uuid_ = UUIDGenerator::get()();
  logger_->setApplicationUUID(uuid_);

  sola_ = std::make_shared<sola_ns3::SOLAWrapperNs3>(
      config_mo, config_ed,
      [&](const sola::Message &m) {
        auto message = deserialize(m.content);
        logReceivedSolaMessage(m, message);
        if (auto to_update = std::get_if<TaskUpdate>(&message)) {
          processMessageUpdate(*to_update);
        } else {
          task_allocation_initiator_->receiveMessage(message);
        }
      },
      [&](const sola::TopicMessage &m) {
        auto message = deserialize(m.content);
        auto info = getSolaMessageLoggingInfo(message);
        std::string content_str = info.order + ";" + info.sender_ip + ";" + info.target_ip + ";" +
                                  std::to_string(info.message_type) + ";" + info.content;

        if (task_allocation_initiator_) {
          logger_->logTopicMessage(m.topic, natter::uuidToString(m.uuid), uuid_, content_str, true);
          task_allocation_initiator_->receiveMessage(message);
        }
      },
      logger_, uuid_, device_id_);

  logger_->logMaterialFlow(ns3::Simulator::Now().GetMilliSeconds(), uuid_, sola_->getIP(),
                           sola_->getPort(), 0);
}

daisi::cpps::NegotiationTrafficLoggingInfo TransportOrderApplicationNs3::getSolaMessageLoggingInfo(
    const Message &deserialized_msg) {
  daisi::cpps::NegotiationTrafficLoggingInfo info;

  auto overload = Overload{
      [&info](const SSISubmission &msg) {
        info.order = msg.getTaskUuid();
        info.message_type = MessageTypes::kSsiSubmission;
        info.content = msg.getLoggingContent();
      },
      [&info](const SSIWinnerResponse &msg) {
        info.order = msg.getTaskUuid();
        info.message_type = MessageTypes::kSsiWinnerResponse;
        info.content = msg.getLoggingContent();
      },
      [&info](const TaskUpdate &msg) {
        info.order = msg.getTaskUuid();
        info.message_type = MessageTypes::kTaskUpdate;
        info.content = msg.getLoggingContent();
      },
      [&info](auto /*unused*/) { info.message_type = MessageTypes::kUnknownNegotiationMsgType; }};

  std::visit(overload, deserialized_msg);

  return info;
}

void TransportOrderApplicationNs3::logReceivedSolaMessage(const sola::Message &sola_msg,
                                                          const Message &deserialized_msg) {
  auto info = getSolaMessageLoggingInfo(deserialized_msg);

  info.sender_ip = sola_msg.sender;
  info.sender_port = 2000;
  info.target_ip = sola_->getIP();
  info.target_port = sola_->getPort();

  logger_->logNegotiationTraffic(info);
}

void TransportOrderApplicationNs3::completeYourself(
    const std::shared_ptr<ScenariofileParser::Table> &model_description,
    const std::vector<ns3::Vector> &locations) {
  waiting_for_start_ = false;

  material_flow_model_ = std::make_shared<MaterialFlowModel>();
  material_flow_model_->loadFromScenarioTable(model_description, locations,
                                              (double)ns3::Simulator::Now().GetMilliSeconds());

  for (auto &order : material_flow_model_->getVertices()) {
    logger_->logTransportOrder(order, 1, 1, uuid_);  // ! TODO: Get station ids!
    logger_->logTransportOrderUpdate(order);
  }

  switch (mrta_config_.interaction_protocol_type) {
    case InteractionProtocolType::kIteratedAuctionPubSubModified:
      task_allocation_initiator_ = std::make_unique<IteratedAuctionInitiatorPubsubModified>(
          sola_, logger_, material_flow_model_);
      break;

    case InteractionProtocolType::kIteratedAuctionPubSubTePSSI:
      task_allocation_initiator_ = std::make_unique<IteratedAuctionInitiatorPubsubTePSSI>(
          sola_, logger_, material_flow_model_);
      break;

    default:
      throw std::logic_error("Invalid MRTA Configuration");
  }

  task_allocation_initiator_->init();
}

void TransportOrderApplicationNs3::processMessageUpdate(const TaskUpdate &to_update) {
  std::vector<Task> &vertices = material_flow_model_->getVertices();
  auto it = std::find_if(vertices.begin(), vertices.end(),
                         [&](const Task &o) { return o.getUUID() == to_update.getTaskUuid(); });
  if (it != vertices.end()) {
    it->setCurrentPosition(to_update.getPosition());
    material_flow_model_->setOrderState(to_update.getTaskUuid(), to_update.getOrderState());
    if (to_update.getOrderState() == OrderStates::kFinished) {
      std::cout << "FINISHED TO" << std::endl;
    }
  } else {
    throw std::logic_error("We received TaskUpdate, but couldn't find the Task");
  }
}

OrderStates TransportOrderApplicationNs3::getState() const {
  if (!material_flow_model_) {
    return OrderStates::kCreated;
  }

  auto vertices = material_flow_model_->getVertices();
  if (vertices.empty()) {
    return OrderStates::kCreated;
  }

  bool all_finished = std::all_of(vertices.begin(), vertices.end(), [](const Task &order) {
    return order.getOrderState() == OrderStates::kFinished;
  });
  if (all_finished) {
    return OrderStates::kFinished;
  }

  bool all_queued = std::all_of(vertices.begin(), vertices.end(), [](const Task &order) {
    return order.getOrderState() == OrderStates::kQueued;
  });
  if (all_queued) {
    return OrderStates::kQueued;
  }

  bool all_started = std::all_of(vertices.begin(), vertices.end(), [](const Task &order) {
    return order.getOrderState() != OrderStates::kCreated &&
           order.getOrderState() != OrderStates::kQueued &&
           order.getOrderState() != OrderStates::kError;
  });
  if (all_started) {
    return OrderStates::kStarted;
  }

  return OrderStates::kCreated;
}

void TransportOrderApplicationNs3::setWaitingForStart() { waiting_for_start_ = true; }

};  // namespace daisi::cpps
