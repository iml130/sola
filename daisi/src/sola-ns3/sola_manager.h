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

#ifndef DAISI_SOLA_NS3_SOLA_MANAGER_H_
#define DAISI_SOLA_NS3_SOLA_MANAGER_H_

#include <deque>

#include "manager/core_network.h"
#include "manager/general_scenariofile.h"
#include "manager/manager.h"
#include "ns3/node-container.h"
#include "sola_application.h"
#include "sola_logger_ns3.h"
#include "sola_scenariofile.h"

namespace daisi::sola_ns3 {

/// Basic manager to test SOLA features standalone within simulation.
class SolaManager : public Manager {
public:
  explicit SolaManager(const std::string &scenariofile_path);

private:
  void setupImpl() override;
  std::string getDatabaseFilename() const override;
  GeneralScenariofile getGeneralScenariofile() const override;

  void startSOLA(uint32_t id);
  void subscribeTopic(const std::string &topic, uint32_t id);
  void publishTopic(uint32_t id, const std::string &topic, uint64_t msg_size);
  void leaveTopic(uint32_t id);
  void findService(uint32_t id);
  void addService(uint32_t id);
  void updateService(uint32_t id);
  void removeService(uint32_t id);

  void scheduleEvents();
  uint64_t getNumberOfNodes() const;

  // Scheduling methods, implemented in sola_manager_scheduler.cpp
  void schedule(StartSOLA start, ns3::Time &current_time);
  void schedule(SubscribeTopic subscribe, ns3::Time &current_time);
  void schedule(Delay delay, ns3::Time &current_time);
  void schedule(Publish publish, ns3::Time &current_time);

  ns3::Ptr<SolaApplication> getApplication(uint32_t id) const;

  SolaScenariofile scenariofile_;

  ns3::NodeContainer nodes_;
  CoreNetwork core_network_;
};

}  // namespace daisi::sola_ns3

#endif
