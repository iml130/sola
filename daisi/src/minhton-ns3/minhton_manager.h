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

#ifndef DAISI_MINHTON_NS3_MINHTON_MANAGER_H_
#define DAISI_MINHTON_NS3_MINHTON_MANAGER_H_

#include "manager/manager.h"
#include "minhton_application.h"
#include "minhton_logger_ns3.h"

namespace daisi::minhton_ns3 {

class MinhtonManager : public Manager<MinhtonApplication> {
public:
  class Scheduler;

  explicit MinhtonManager(const std::string &scenariofile_path);
  void setup() override;

private:
  void initNode(uint32_t id, minhton::ConfigNode config);
  void setupNodeConfigurations();
  uint64_t getNumberOfNodes() override;
  void scheduleEvents() override;
  std::string getDatabaseFilename() override;

  std::shared_ptr<Scheduler> scheduler_;
};

}  // namespace daisi::minhton_ns3

#endif
