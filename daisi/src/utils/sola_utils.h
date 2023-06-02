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

#ifndef DAISI_UTILS_SOLA_UTILS_H_
#define DAISI_UTILS_SOLA_UTILS_H_

#include <cstdio>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <istream>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

#include "ns3/ipv4-address.h"
#include "ns3/node.h"

namespace daisi {

struct NodeNetworkInfo {
  ns3::Ptr<ns3::Node> node;
  uint16_t next_free_port;
};

class SolaNetworkUtils {
public:
  static SolaNetworkUtils &get() {
    static SolaNetworkUtils utils;
    return utils;
  }

  void createSockets(const std::string &ip, uint32_t amount = 1);

  void registerNode(ns3::Ipv4Address address, const ns3::Ptr<ns3::Node> &node,
                    uint16_t next_free_port);

  // Unregister as active node
  void unregisterNode(ns3::Ipv4Address address);

private:
  SolaNetworkUtils() = default;

  std::unordered_map<std::string, NodeNetworkInfo> nodes_;
};

std::string getScenariofilePath(std::string scenariofile_name);
int getTestFile(std::string &scenariofile_name, std::string &scenariofile_string,
                std::string &scenariofile_path);
std::string getIpv4AddressString(ns3::Ipv4Address address);

std::string generateRandomString();

// we want to have the information stored as "(yyyy-mm-dd_hh-mm-ss_m00_n00000.db)"
std::string generateDBNameWithMinhtonInfo(const std::string &app_name, uint16_t fanout,
                                          uint16_t number_of_nodes);

std::string generateDBName(const std::string &app_name);
std::string generateDBName(const std::string &app_name, const std::string &identification_string);

}  // namespace daisi
#endif
