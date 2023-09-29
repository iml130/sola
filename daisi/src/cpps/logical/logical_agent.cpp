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

#include "logical_agent.h"

#include <functional>

#include "logging/logger_manager.h"
#include "minhton/utils/config_reader.h"
#include "sola-ns3/config_helper_ns3.h"
#include "sola-ns3/management_minhton_helper.h"
#include "solanet/uuid.h"
#include "solanet/uuid_generator.h"

namespace daisi::cpps::logical {

LogicalAgent::LogicalAgent(std::shared_ptr<CppsLoggerNs3> logger, AlgorithmConfig config_algo,
                           bool first_node)
    : logger_(std::move(logger)),
      algorithm_config_(std::move(config_algo)),
      first_node_(first_node) {}

void LogicalAgent::initCommunication() {
  const std::string config_file =
      first_node_ ? "configurations/root.yml" : "configurations/join.yml";

  sola::ManagementOverlayMinhton::Config config_mo = minhton::config::readConfig(config_file);
  sola_ns3::configureLogger(config_mo);

  if (!first_node_) {
    daisi::sola_ns3::setJoinIp(config_mo);
  }

  sola::EventDisseminationMinhcast::Config config_ed;
  sola_ns3::configureLogger(config_ed);

  uuid_ = solanet::uuidToString(solanet::generateUUID());
  logger_->setApplicationUUID(uuid_);

  auto message_recv_fct = [this](const solanet::Message &msg) {
    this->messageReceiveFunction(msg);
  };
  auto topic_message_recv_fct = [this](const sola::TopicMessage &msg) {
    this->topicMessageReceiveFunction(msg);
  };

  communicator_ = std::make_shared<daisi::cpps::common::CppsCommunicator>(
      config_mo, config_ed, topic_message_recv_fct,
      daisi::global_logger_manager->createSolaLogger(), message_recv_fct);
}

void LogicalAgent::processMessage(const Message &msg) {
  for (const auto &algorithm : algorithms_) {
    bool processed =
        std::visit([&algorithm](const auto &msg) { return algorithm->process(msg); }, msg);
    if (processed) {
      return;
    }
  }

  throw std::runtime_error("Failed to process message");
}

void LogicalAgent::messageReceiveFunction(const solanet::Message &msg) {
  auto logical_message = deserialize(msg.getMessage());
  processMessage(logical_message);
}

void LogicalAgent::topicMessageReceiveFunction(const sola::TopicMessage &msg) {
  auto logical_message = deserialize(msg.content);
  processMessage(logical_message);
}

bool LogicalAgent::isRunning() const { return communicator_->sola.isStorageRunning(); }

bool LogicalAgent::canStop() const { return communicator_->sola.canStop(); }

void LogicalAgent::prepareStop() { communicator_->sola.stop(); }

}  // namespace daisi::cpps::logical
