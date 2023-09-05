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

#ifndef DAISI_MANAGER_MANAGER_H_
#define DAISI_MANAGER_MANAGER_H_

#include <string>

#include "general_scenariofile.h"

namespace daisi {

class Manager {
public:
  virtual ~Manager();

  void setup();

  void markAsFailed(const char *exception);

  void run();

private:
  virtual std::string getDatabaseFilename() const = 0;
  virtual GeneralScenariofile getGeneralScenariofile() const = 0;
  virtual void setupImpl() = 0;
  virtual std::string getAdditionalParameters() const { return ""; }
};
}  // namespace daisi

#endif
