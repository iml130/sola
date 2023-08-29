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

#include "minhton_application.h"

#include "logging/logger_manager.h"
#include "ns3/uinteger.h"
#include "solanet-ns3/sola_message_ns3.h"
#include "utils/daisi_check.h"
#include "utils/sola_utils.h"

using namespace ns3;

namespace daisi::minhton_ns3 {

TypeId MinhtonApplication::GetTypeId() {
  static TypeId tid =
      TypeId("MinhtonApp").SetParent<Application>().AddConstructor<MinhtonApplication>();
  return tid;
}

void MinhtonApplication::StartApplication() {
  logger_ = daisi::global_logger_manager->createMinhtonLogger();
}

void MinhtonApplication::StopApplication() {}

void MinhtonApplication::initializeNode(minhton::ConfigNode config) {
  config.setLogger({logger_});
  minhton_node_ =
      std::make_unique<minhton::MinhtonNode>(config, minhton::NeighborCallbackFct{}, false);
}

void MinhtonApplication::executeSearchExactTest(uint32_t dest_level, uint32_t dest_number) {
  DAISI_CHECK(minhton_node_, "MINHTON not initialized");

  auto our_node_info = minhton_node_->getNodeInfo();
  if (our_node_info.getLevel() == dest_level && our_node_info.getNumber() == dest_number) {
    return;
  }

  auto dest_node = minhton::NodeInfo(dest_level, dest_number, our_node_info.getFanout());
  minhton::MinhtonMessageHeader header(minhton_node_->getNodeInfo(), dest_node);
  minhton::MessageEmpty query(header);
  minhton_node_->performSearchExactTest(dest_node,
                                        std::make_shared<minhton::MessageSEVariant>(query));
}

void MinhtonApplication::processSignal(const minhton::Signal &signal) {
  DAISI_CHECK(minhton_node_, "MINHTON not initialized");
  minhton_node_->processSignal(signal);
}

void MinhtonApplication::localTestDataInsert(const std::vector<minhton::Entry> &entries) {
  DAISI_CHECK(minhton_node_, "MINHTON not initialized");
  minhton_node_->insert(entries);
}

void MinhtonApplication::localTestDataUpdate(const std::vector<minhton::Entry> &entries) {
  DAISI_CHECK(minhton_node_, "MINHTON not initialized");
  minhton_node_->update(entries);
}

void MinhtonApplication::localTestDataRemove(const std::vector<minhton::NodeData::Key> &keys) {
  DAISI_CHECK(minhton_node_, "MINHTON not initialized");
  minhton_node_->remove(keys);
}

void MinhtonApplication::executeFindQuery(const minhton::FindQuery &query) {
  DAISI_CHECK(minhton_node_, "MINHTON not initialized");
  minhton_node_->find(query);
}

void MinhtonApplication::setStaticBuildNeighbors(const minhton::NodeInfo &self_pos,
                                                 const std::vector<minhton::NodeInfo> &neighbors,
                                                 const minhton::NodeInfo &adj_left,
                                                 const minhton::NodeInfo &adj_right) {
  DAISI_CHECK(minhton_node_, "MINHTON not initialized");
  minhton::FSMState init_state = minhton::FSMState::kConnected;
  minhton_node_->initFSM(init_state);

  minhton_node_->getRoutingInformation()->setPosition(self_pos.getLogicalNodeInfo());
  if (self_pos.getLevel() != 0) {
    const auto &l_node_info = self_pos.getLogicalNodeInfo();
    minhton_node_->getAccessContainer()->logger.logNode(
        {l_node_info.getUuid(), l_node_info.getLevel(), l_node_info.getNumber(),
         l_node_info.getFanout(), l_node_info.isInitialized()});
  }

  if (adj_left.isInitialized()) {
    minhton_node_->getRoutingInformation()->setAdjacentLeft(adj_left);
  }
  if (adj_right.isInitialized()) {
    minhton_node_->getRoutingInformation()->setAdjacentRight(adj_right);
  }

  for (auto const &neighbor : neighbors) {
    minhton_node_->getRoutingInformation()->updateNeighbor(neighbor);
  }
}

minhton::NodeInfo MinhtonApplication::getNodeInfo() {
  DAISI_CHECK(minhton_node_, "MINHTON not initialized");
  return minhton_node_->getNodeInfo();
}

}  // namespace daisi::minhton_ns3
