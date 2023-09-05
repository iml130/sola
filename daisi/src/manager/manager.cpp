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

#include "manager.h"

#include <cstdint>

#include "logging/logger_manager.h"
#include "ns3/core-module.h"
#include "utils/daisi_check.h"
#include "utils/random_engine.h"

namespace daisi {

Manager::~Manager() {
  using namespace ns3;
  daisi::global_logger_manager->updateTestSetupTime();

  const uint64_t event_count = Simulator::GetEventCount();
  daisi::global_logger_manager->updateTestSetupEventCount(event_count);
  std::cout << "Events executed: " << event_count << std::endl;

  Simulator::Destroy();

  daisi::global_logger_manager.reset();
}

void Manager::markAsFailed(const char *exception) {
  daisi::global_logger_manager->setFailed(exception);
}

void Manager::setup() {
  daisi::global_random_engine = std::mt19937_64(getGeneralScenariofile().random_seed);

  daisi::global_logger_manager = std::make_unique<daisi::LoggerManager>(
      getGeneralScenariofile().getOutputPath(), getDatabaseFilename());

  setupImpl();
}

void Manager::run() {
  DAISI_CHECK(daisi::global_logger_manager, "Logger not initialized");

  using namespace ns3;

  daisi::LoggerInfoTestSetup info{getGeneralScenariofile().getFileContent(),
                                  getAdditionalParameters()};

  daisi::global_logger_manager->logTestSetup(info);

  Simulator::Stop(getGeneralScenariofile().stop_time);

  std::cout << "Simulation Start" << std::endl;
  Simulator::Run();
  std::cout << "Simulation Finished" << std::endl;
}

}  // namespace daisi
