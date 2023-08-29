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

#include "ns3/network-module.h"

namespace daisi {

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

std::vector<ns3::Ipv4Address> getAddressesForNode(const ns3::NodeContainer &container,
                                                  uint32_t node_container_idx);

/// Get the first non-local IP address from the address vector.
/// The vector must contain at least one non-local address.
ns3::Ipv4Address getNonLocalAddress(std::vector<ns3::Ipv4Address> addresses);

}  // namespace daisi
#endif
