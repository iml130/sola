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

#include "agv_logical.h"

#include <cassert>
#include <cstddef>
#include <functional>
#include <variant>

#include "cpps/amr/message/amr_state.h"
#include "cpps/amr/message/serializer.h"
#include "cpps/amr/physical/functionality.h"
#include "cpps/common/uuid_generator.h"
#include "cpps/message/task_info.h"
#include "cpps/negotiation/participant/iterated_auction_participant_pubsub_modified.h"
#include "cpps/negotiation/participant/iterated_auction_participant_pubsub_tepssi.h"
#include "cpps/negotiation/task_management/basic_list_task_management.h"
#include "cpps/negotiation/task_management/greedy_tsp_list_task_management.h"
#include "cpps/negotiation/task_management/stn_task_management.h"
#include "cpps/negotiation/utility/utility_evaluator.h"
#include "cpps/negotiation/utils/overload.h"
#include "cpps/packet.h"
#include "minhton/utils/config_reader.h"
#include "natter/uuid.h"
#include "utils/sola_utils.h"

namespace daisi::cpps {

AgvLogicalNs3::AgvLogicalNs3(TopologyNs3 topology, const MRTAConfig &mrta_config,
                             int id_for_friendly, uint32_t device_id)
    : device_id_(device_id),
      topology_(std::move(topology)),
      uuid_(UUIDGenerator::get()()),
      mrta_config_(mrta_config),
      first_node_(id_for_friendly == 0) {
  logger_ = daisi::global_logger_manager->createAMRLogger(device_id_);

  last_position_ = std::make_shared<ns3::Vector>();
  ability_ = std::make_shared<amr::AmrStaticAbility>();
  kinematics_ = std::make_shared<Kinematics>();
}

void AgvLogicalNs3::init(ns3::Ptr<ns3::Socket> socket) {
  // SOLA --------------

  const std::string config_file =
      first_node_ ? "configurations/root.yml" : "configurations/join.yml";

  sola::ManagementOverlayMinhton::Config config_mo = minhton::config::readConfig(config_file);

  if (!first_node_) {
    sola_ns3::SOLAWrapperNs3::setJoinIp(config_mo);
  }

  // Nothing to configure (yet)
  sola::EventDisseminationMinhcast::Config config_ed;

  sola_ = std::make_shared<sola_ns3::SOLAWrapperNs3>(
      config_mo, config_ed,
      [this](const sola::Message &m) {
        auto msg = deserialize(m.content);
        logReceivedSolaMessage(m, msg);
        task_allocation_participant_->receiveMessage(msg);
      },
      [this](const sola::TopicMessage &m) {
        auto msg = deserialize(m.content);
        auto info = getSolaMessageLoggingInfo(msg);
        std::string content_str = info.order + ";" + info.sender_ip + ";" + info.target_ip + ";" +
                                  std::to_string(info.message_type) + ";" + info.content;
        logger_->logTopicMessage(m.topic, natter::uuidToString(m.uuid), uuid_, content_str, true);

        task_allocation_participant_->receiveMessage(msg);
      },
      logger_, uuid_, device_id_);

  // Negotiation Participant -----------------

  createParticipant();

  // ns3 ----------------------------

  socket_ = socket;
  socket_->Listen();
  socket_->SetAcceptCallback(ns3::MakeCallback(&AgvLogicalNs3::connectionRequest, this),
                             ns3::MakeCallback(&AgvLogicalNs3::newConnectionCreated, this));

  ns3::Address addr;
  socket_->GetSockName(addr);
  ns3::InetSocketAddress iaddr = ns3::InetSocketAddress::ConvertFrom(addr);
  agv_logical_asset_ip_ = daisi::getIpv4AddressString(iaddr.GetIpv4());
  agv_logical_asset_port_ = iaddr.GetPort();
}

daisi::cpps::NegotiationTrafficLoggingInfo AgvLogicalNs3::getSolaMessageLoggingInfo(
    const Message &deserialized_msg) {
  daisi::cpps::NegotiationTrafficLoggingInfo info;

  auto overload = Overload{
      [&info](const SSICallForProposal &msg) {
        info.message_type = MessageTypes::kSsiCallForProposal;
        info.content = msg.getLoggingContent();
      },
      [&info](const SSIIterationNotification &msg) {
        info.message_type = MessageTypes::kSsiIterationNotification;
        info.content = msg.getLoggingContent();
      },
      [&info](const SSIWinnerNotification &msg) {
        info.order = msg.getTaskUuid();
        info.message_type = MessageTypes::kSsiWinnerNotification;
        info.content = msg.getLoggingContent();
      },
      [&info](auto /*unused*/) { info.message_type = MessageTypes::kUnknownNegotiationMsgType; }};

  std::visit(overload, deserialized_msg);

  return info;
}

void AgvLogicalNs3::logReceivedSolaMessage(const sola::Message &sola_msg,
                                           const Message &deserialized_msg) {
  auto info = getSolaMessageLoggingInfo(deserialized_msg);

  info.sender_ip = sola_msg.sender;
  info.sender_port = 2000;
  info.target_ip = sola_->getIP();
  info.target_port = sola_->getPort();

  logger_->logNegotiationTraffic(info);
}

bool AgvLogicalNs3::connectionRequest(ns3::Ptr<ns3::Socket> socket, const ns3::Address &addr) {
  // Accept all connections
  return true;
}

void AgvLogicalNs3::readFromSocket(ns3::Ptr<ns3::Socket> socket) {
  auto packet = socket->Recv();
  daisi::cpps::CppsTCPMessage header;
  packet->RemoveHeader(header);
  for (const auto &msg : header.getMessages()) {
    amr::Message m = amr::deserialize(msg.payload);
    if (auto description = std::get_if<AmrDescription>(&m))
      processMessageDescription(*description);
    else if (auto order_update = std::get_if<AmrOrderUpdate>(&m))
      processOrderStatusUpdate(*order_update);
    else if (auto status_update = std::get_if<AmrStatusUpdate>(&m))
      processMessageUpdate(*status_update);
    else
      throw std::runtime_error("invalid packet type for logical agv");
  }
}

void AgvLogicalNs3::processMessageDescription(const AmrDescription &description) {
  assert(!service_received_);  // Only 1 service supported
  service_received_ = true;
  uuid_ = UUIDGenerator::get()();

  task_management_->setAgvUUID(uuid_);
  logAGV();
  AmrKinematics k = description.getKinematics();
  AmrLoadHandlingUnit l = description.getLoadHandling();
  // Kinematics uses miliseconds whereas AmrKinematics uses seconds
  kinematics_->set(Kinematics(k.getMaxVelocity(), k.getMinVelocity(), k.getMaxAcceleration(),
                              k.getMaxDeceleration(), l.getLoadTime() * 1000.0,
                              l.getUnloadTime() * 1000.0));
  ability_ = std::make_shared<amr::AmrStaticAbility>(l.getAbility());
  service_.friendly_name =
      "service_" +
      description.getProperties().getFriendlyName();  // As we are only having a single service
  service_.uuid = UUIDGenerator::get()();
  service_.key_values.insert({"servicetype", std::string("transport")});
  service_.key_values.insert({"maxpayload", l.getAbility()});

  std::string type = l.getAbility().getLoadCarrier().getTypeAsString();

  service_.key_values.insert({"loadcarriertype", type});

  service_.key_values.insert({"agvuuid", uuid_});

  sola_->addService(service_);
  logger_->logTransportService(service_, true);

  std::string endpoint = sola_->getConectionString();
  std::string ip = endpoint.substr(0, endpoint.find(":"));
  SolaNetworkUtils::get().createSockets(ip);

  task_allocation_participant_->init();
}

void AgvLogicalNs3::processMessageUpdate(const AmrStatusUpdate &payload) {
  last_position_->x = payload.getPosition().x;
  last_position_->y = payload.getPosition().y;
  last_position_->z = 0;
  task_management_->updateCurrentOrderPosition();

  AmrState new_agv_state = payload.getState();

  if (update_counter_ == 0) {
    daisi::cpps::AGVPositionLoggingInfo info;
    info.uuid = uuid_;
    info.x = last_position_->x;
    info.y = last_position_->y;
    info.z = last_position_->z;
    info.state = static_cast<uint8_t>(new_agv_state);

    logger_->logPositionUpdate(info);
  }

  ++update_counter_;
  if (update_counter_ == 10) update_counter_ = 0;

  if (last_agv_state_ == AmrState::kWorking && new_agv_state == AmrState::kIdle) {
    checkOrderQueue();
  }
  last_agv_state_ = new_agv_state;
}

void AgvLogicalNs3::processOrderStatusUpdate(const AmrOrderUpdate &payload) {
  task_management_->setCurrentOrderStatus(payload.getState());

  const Task &current_order = task_management_->getCurrentOrder();
  TaskUpdate update(current_order.getUUID(), current_order.getOrderState(),
                    current_order.getCurrentPosition());
  sola_->sendData(serialize(update), sola::Endpoint(current_order.getConnection()));
}

void AgvLogicalNs3::newConnectionCreated(ns3::Ptr<ns3::Socket> socket, const ns3::Address &addr) {
  socket_agv_ = socket;

  ns3::InetSocketAddress iaddr = ns3::InetSocketAddress::ConvertFrom(addr);
  agv_physical_ip_ = daisi::getIpv4AddressString(iaddr.GetIpv4());
  agv_physical_port_ = iaddr.GetPort();

  socket_agv_->SetRecvCallback(MakeCallback(&AgvLogicalNs3::readFromSocket, this));
}

void AgvLogicalNs3::logAGV() const {
  logger_->setApplicationUUID(uuid_);

  daisi::cpps::AGVLoggingInfo info;
  info.friendly_name = data_model_.agv_properties.friendly_name;
  info.manufacturer = data_model_.agv_properties.manufacturer;
  info.model_name = data_model_.agv_properties.model_name;
  info.model_number = data_model_.agv_properties.model_number;
  info.serial_number = data_model_.agv_device_descr.serial_number;
  info.ip_logical_core = sola_->getIP();
  info.port_logical_core = sola_->getPort();
  info.ip_logical_asset = agv_logical_asset_ip_;
  info.port_logical_asset = agv_logical_asset_port_;
  info.ip_physical = agv_physical_ip_;
  info.port_physical = agv_physical_port_;

  info.load_time = data_model_.agv_properties.kinematic.getLoadTime();
  info.unload_time = data_model_.agv_properties.kinematic.getUnloadTime();
  info.load_carrier_type = data_model_.agv_properties.ability.getLoadCarrier().getTypeAsString();
  info.max_weight = data_model_.agv_properties.ability.getMaxPayloadWeight();

  info.max_velocity = data_model_.agv_properties.kinematic.getMaxVelocity();
  info.min_velocity = data_model_.agv_properties.kinematic.getMinVelocity();
  info.max_acceleration = data_model_.agv_properties.kinematic.getMaxAcceleration();
  info.min_acceleration = data_model_.agv_properties.kinematic.getMinAcceleration();

  logger_->logAGV(info);
}

void AgvLogicalNs3::checkOrderQueue() {
  if (last_agv_state_ == AmrState::kIdle && task_management_->canExecuteNextOrder()) {
    auto finished_order = task_management_->getCurrentOrder();
    if (!finished_order.getUUID().empty())
      assert(finished_order.getOrderState() == OrderStates::kFinished);

    task_management_->setNextOrderToExecute();
    sendOrderToAGV();

  } else {
    ns3::Simulator::Schedule(ns3::MilliSeconds(100), &AgvLogicalNs3::checkOrderQueue, this);
  }
}

void AgvLogicalNs3::sendOrderToAGV() {
  const Task &current_order = task_management_->getCurrentOrder();
  ns3::Vector pickup = current_order.getPickupLocation();
  ns3::Vector delivery = current_order.getDeliveryLocation();

  std::vector<FunctionalityVariant> functionalities;
  functionalities.push_back(MoveTo({pickup.x, pickup.y}));
  functionalities.push_back(Load({pickup.x, pickup.y}));
  functionalities.push_back(MoveTo({delivery.x, delivery.y}));
  functionalities.push_back(Unload({delivery.x, delivery.y}));

  AmrOrderInfo order_info(functionalities, current_order.getAbilityRequirement());

  daisi::cpps::CppsTCPMessage message;
  message.addMessage({amr::serialize(order_info), 0});
  ns3::Ptr<ns3::Packet> packet = ns3::Create<ns3::Packet>();
  packet->AddHeader(message);
  socket_agv_->Send(packet);
}

void AgvLogicalNs3::createParticipant() {
  // Cost Calculation -------

  std::shared_ptr<UtilityEvaluator> utility_evaluator =
      std::make_shared<UtilityEvaluator>(mrta_config_.utility_evaluation_type);

  switch (mrta_config_.task_management_type) {
    case TaskManagementType::kBasicList:
      task_management_ = std::make_unique<BasicListTaskManagement>(
          kinematics_, ability_, last_position_, logger_, utility_evaluator);
      break;

    case TaskManagementType::kGreedyTSPList:
      task_management_ = std::make_unique<GreedyTSPListTaskManagement>(
          kinematics_, ability_, last_position_, logger_, utility_evaluator);
      break;

    case TaskManagementType::kSimpleTemporalNetwork:
      task_management_ = std::make_unique<STNTaskManagement>(kinematics_, ability_, last_position_,
                                                             logger_, utility_evaluator);
      break;

    default:
      throw std::invalid_argument("Invalid MRTA Config for Task Management");
  }

  switch (mrta_config_.interaction_protocol_type) {
    case InteractionProtocolType::kIteratedAuctionPubSubModified:
      task_allocation_participant_ =
          std::make_unique<IteratedAuctionParticipantPubsubModified>(sola_, task_management_);
      break;

    case InteractionProtocolType::kIteratedAuctionPubSubTePSSI:
      task_allocation_participant_ =
          std::make_unique<IteratedAuctionParticipantPubsubTePSSI>(sola_, task_management_);
      break;

    default:
      throw std::invalid_argument("Invalid MRTA Config for Interaction Protocol");
  }
}

}  // namespace daisi::cpps
