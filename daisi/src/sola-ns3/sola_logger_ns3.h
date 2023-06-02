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

#ifndef DAISI_SOLA_NS3_SOLA_LOGGER_NS3_H_
#define DAISI_SOLA_NS3_SOLA_LOGGER_NS3_H_

#include <ctime>
#include <unordered_map>

#include "cpps/common/cpps_logger_ns3.h"
#include "logging/sqlite/sqlite_helper.h"

namespace daisi::sola_ns3 {
// Ref #56
class SolaLoggerNs3 : public daisi::cpps::CppsLoggerNs3 {
public:
  SolaLoggerNs3() = delete;
  explicit SolaLoggerNs3(LogDeviceApp log_device_application, LogFunction log);

  virtual ~SolaLoggerNs3() = default;

private:
};

}  // namespace daisi::sola_ns3

#endif
