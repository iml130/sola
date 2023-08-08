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

#ifndef DAISI_SOLA_NS3_MANAGEMENT_MINHTON_HELPER_H_
#define DAISI_SOLA_NS3_MANAGEMENT_MINHTON_HELPER_H_

#include "SOLA/management_overlay_minhton.h"

namespace daisi::sola_ns3 {
/// Helper to set join IP in config
inline void setJoinIp(sola::ManagementOverlayMinhton::Config &config) {
  // TODO Do not always join on same (root) node; Ref #83
  static const std::string ip = "192.168.0.2";  // First AMR - Wifi

  auto info = config.getJoinInfo();
  info.ip = ip;
  config.setJoinInfo(info);
}
}  // namespace daisi::sola_ns3

#endif
