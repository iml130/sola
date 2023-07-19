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

#include "sola_utils.h"

#include <unistd.h>

#include <algorithm>
#include <cassert>
#include <charconv>
#include <deque>
#include <filesystem>
#include <fstream>
#include <random>
#include <string>

#include "daisi_check.h"
#include "ns3/ipv4.h"
#include "ns3/simulator.h"
#include "ns3/socket.h"

namespace daisi {

std::string getIpv4AddressString(ns3::Ipv4Address address) {
  std::ostringstream os;
  address.Print(os);
  std::string address_string = os.str();
  return address_string;
}

///
/// the scenariofile_name has to be located in
/// src/sola-ns3/scenariofiles/<application>/<scenariofile_name>
///
std::string getScenariofilePath(std::string scenariofile_name) {
  using namespace std::filesystem;

  std::string cwd = current_path().string();
  /// development
  if (exists(cwd + "/scenarios/" + scenariofile_name))
    return cwd + "/scenarios/" + scenariofile_name;

  if (exists(scenariofile_name))
    /// absolute path
    return scenariofile_name;

  throw std::runtime_error("Scenariofile does not exist\n");
}

//! Dependency-free method to decode all percent-encoded elements of a string,
//! that occur in our scenariostrings
std::string decodeScenarioString(std::string scenario_string) {
  size_t current_pos = scenario_string.find('%');
  while (current_pos != std::string::npos) {
    DAISI_CHECK(current_pos + 3 <= scenario_string.length(),
                "Last encoded % is shorter than string")

    // Get hex number after % and convert to char
    char replacement = 0;
    std::string code = scenario_string.substr(current_pos + 1, 2);
    std::from_chars(code.begin().operator->(), code.end().operator->(), replacement, 16);

    std::string replacement_string;
    replacement_string += replacement;
    scenario_string.replace(current_pos, 3, replacement_string);
    current_pos = scenario_string.find('%');
  }

  DAISI_CHECK(scenario_string.find('%') == scenario_string.npos,
              "Found unknown percent-encoded element in scenariostring")
  return scenario_string;
}

int getTestFile(std::string &scenariofile_name, std::string &scenariofile_string,
                std::string &scenariofile_path) {
  std::string default_scenariofile_name = "default.yml";
  bool use_scenariostring = false;
  // check if both are set
  if (scenariofile_name.length() > 0 && scenariofile_string.length() > 0) {
    std::cout
        << "Only one param can be set. Please either set param 'scenario' or 'scenariostring'."
        << std::endl;
    std::cout << "Quitting simulation..." << std::endl;
    return -1;
  }
  scenariofile_name = scenariofile_name.empty() ? default_scenariofile_name : scenariofile_name;

  if (scenariofile_string.length() > 0) {
    use_scenariostring = true;
    scenariofile_string = decodeScenarioString(scenariofile_string);
  }

  if (use_scenariostring) {
    scenariofile_path = scenariofile_string;
  } else {
    /* code */
    scenariofile_path = getScenariofilePath(scenariofile_name);
  }
  return 0;
}

std::string fillUpWithZeros(uint8_t n_zero, uint16_t number) {
  std::string old_string = std::to_string(number);
  return std::string(n_zero - old_string.length(), '0') + old_string;
}

// Generate random name for e.g. for temporary file
std::string generateRandomString() {
  constexpr int kFilenameLength = 6;
  std::vector<char> choices{'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
                            'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'};
  std::random_device rd;
  std::mt19937 g(rd());
  std::shuffle(choices.begin(), choices.end(), g);

  return {choices.begin(), choices.begin() + kFilenameLength};
}

std::string generateDBNameWithMinhtonInfo(const std::string &app_name, uint16_t fanout,
                                          uint16_t number_of_nodes) {
  auto time = std::time(nullptr);
  auto local_time = *std::localtime(&time);
  std::stringstream stream;
  stream << app_name << "_";
  stream << std::put_time(&local_time, "%Y-%m-%d_%H-%M-%S") << "_" << generateRandomString() << "_m"
         << fillUpWithZeros(2, fanout) << "_" << fillUpWithZeros(5, number_of_nodes) << ".db";
  return stream.str();
}

std::string generateDBName(const std::string &app_name, const std::string &identification_string) {
  if (app_name.empty()) {
    throw std::runtime_error("no app name set");
  }

  auto time = std::time(nullptr);
  auto local_time = *std::localtime(&time);
  std::stringstream stream;
  stream << app_name << "_" << std::put_time(&local_time, "%Y-%m-%d_%H-%M-%S") << "_";
  stream << identification_string << ".db";
  return stream.str();
}

std::string generateDBName(const std::string &app_name) {
  return generateDBName(app_name, generateRandomString());
}

std::vector<ns3::Ipv4Address> getAddressesForNode(const ns3::NodeContainer &container,
                                                  uint32_t node_container_idx) {
  // Normally contains one loopback and one real NetDevice
  std::vector<ns3::Ipv4Address> addresses;

  for (int i = 0; i < container.Get(node_container_idx)->GetNDevices(); i++) {
    const ns3::Ptr<ns3::NetDevice> dev = container.Get(node_container_idx)->GetDevice(i);

    const ns3::Ptr<ns3::Ipv4> ipv4 = container.Get(node_container_idx)->GetObject<ns3::Ipv4>();
    const int32_t interface = ipv4->GetInterfaceForDevice(dev);

    // We only use a single address per interface
    assert(ipv4->GetNAddresses(interface) == 1);

    ns3::Ipv4Address addr = ipv4->GetAddress(interface, 0).GetAddress();
    addresses.push_back(addr);
  }

  return addresses;
}

}  // namespace daisi
