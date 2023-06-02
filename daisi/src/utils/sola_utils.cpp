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
#include "ns3/socket.h"

namespace daisi::solanet_ns3 {

extern std::deque<ns3::Ptr<ns3::Socket>> socket_global_;  // HACK
}

namespace daisi {

void SolaNetworkUtils::createSockets(const std::string &ip, uint32_t amount) {
  ns3::TypeId tid = ns3::TypeId::LookupByName("ns3::UdpSocketFactory");

  for (auto i = 0U; i < amount; i++) {
    auto socket = ns3::Socket::CreateSocket(nodes_.at(ip).node, tid);
    ns3::InetSocketAddress local_address =
        ns3::InetSocketAddress(ip.c_str(), nodes_.at(ip).next_free_port);
    nodes_.at(ip).next_free_port++;
    socket->Bind(local_address);

    if (socket->GetErrno() != ns3::Socket::ERROR_NOTERROR) {
      throw std::runtime_error("failed to open socket");
    }

    daisi::solanet_ns3::socket_global_.push_back(socket);
  }
}

void SolaNetworkUtils::registerNode(ns3::Ipv4Address address, const ns3::Ptr<ns3::Node> &node,
                                    uint16_t next_free_port) {
  std::string addr = getIpv4AddressString(address);
  assert(nodes_.find(addr) == nodes_.end());
  nodes_[addr] = {node, next_free_port};
}

void SolaNetworkUtils::unregisterNode(ns3::Ipv4Address address) {
  nodes_.erase(getIpv4AddressString(address));
}

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
    char replacement;
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

}  // namespace daisi
