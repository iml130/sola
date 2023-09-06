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

#include "manager/core_network.h"
#include "manager/manager.h"
#include "minhton_application.h"
#include "minhton_logger_ns3.h"
#include "minhton_scenariofile.h"

namespace daisi::minhton_ns3 {

class MinhtonManager : public Manager {
public:
  class Scheduler;

  explicit MinhtonManager(const std::string &scenariofile_path);

private:
  void setupImpl() override;
  std::string getDatabaseFilename() const override;
  GeneralScenariofile getGeneralScenariofile() const override { return scenariofile_; };
  std::string getAdditionalParameters() const override;

  void initNode(uint32_t id, minhton::ConfigNode config);
  void setupNodeConfigurations();
  uint64_t getNumberOfNodes() const;
  void scheduleEvents();

  std::shared_ptr<Scheduler> scheduler_;
  MinhtonScenariofile scenariofile_;

  ns3::NodeContainer nodes_;

  CoreNetwork core_network_;

  friend class MinhtonManager::Scheduler;
};

}  // namespace daisi::minhton_ns3

#endif
