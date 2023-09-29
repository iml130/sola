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

#ifndef DAISI_MINHTON_NS3_MINHTON_APPLICATION_H_
#define DAISI_MINHTON_NS3_MINHTON_APPLICATION_H_

#include <memory>

#include "minhton/core/node.h"
#include "minhton_logger_ns3.h"
#include "ns3/application.h"

namespace daisi::minhton_ns3 {

/// @brief Wrapper to run MINHTON as a ns-3 application.
class MinhtonApplication final : public ns3::Application {
public:
  static ns3::TypeId GetTypeId();
  MinhtonApplication() = default;
  ~MinhtonApplication() override = default;

  void initializeNode(minhton::ConfigNode config);

  void processSignal(const minhton::Signal &signal);

  minhton::NodeInfo getNodeInfo() const;

  void executeSearchExactTest(uint32_t dest_level, uint32_t dest_number);

  void executeFindQuery(const minhton::FindQuery &query);

  void localTestDataInsert(const std::vector<minhton::Entry> &entries);
  void localTestDataUpdate(const std::vector<minhton::Entry> &entries);
  void localTestDataRemove(const std::vector<minhton::NodeData::Key> &keys);

  void setStaticBuildNeighbors(const minhton::NodeInfo &self_pos,
                               const std::vector<minhton::NodeInfo> &neighbors,
                               const minhton::NodeInfo &adj_left,
                               const minhton::NodeInfo &adj_right);

  void StopApplication() override;

private:
  void StartApplication() override;

  std::unique_ptr<minhton::MinhtonNode> minhton_node_;

  std::shared_ptr<minhton::MinhtonLoggerNs3> logger_;
};
}  // namespace daisi::minhton_ns3
#endif
