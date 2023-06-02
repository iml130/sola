// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#include <stdio.h> /* defines FILENAME_MAX */
#include <unistd.h>

#include <string>
#define GetCurrentDir getcwd

#include <yaml-cpp/yaml.h>

#include "minhton/core/constants.h"
#include "minhton/utils/config_reader.h"

namespace minhton::config {

template <typename T> static T readRequired(const std::string &value, const YAML::Node &node) {
  try {
    return node[value].as<T>();
  } catch (const std::exception &e) {
    throw std::runtime_error("missing required attribute");
  }
}

minhton::ConfigNode readConfig(std::string path_to_file) {
  ConfigNode config;
  char *minhton_path_ptr = std::getenv("MINHTONDIR");
  if (minhton_path_ptr == nullptr) {
    throw std::runtime_error("MINHTONDIR environment variable not specified!");
  }
  std::string minhton_path(minhton_path_ptr);

  if (path_to_file.front() == '/') {
    path_to_file.erase(0);
  }

  if (minhton_path.back() != '/') {
    minhton_path.push_back('/');
  }

  std::string abs_path = minhton_path + path_to_file;

  try {
    YAML::Node config_file = YAML::LoadFile(abs_path);

    config.setVerbose(config_file["verbose"].as<bool>(false));
    config.setFanout(config_file["fanout"].as<uint16_t>(2));
    config.setTreemapper(config_file["treemapper"].as<uint16_t>(k_TREEMAPPER_ROOT_VALUE));
    config.setIsRoot(config_file["root"].as<bool>(false));

    std::string join_mode = readRequired<std::string>("join_mode", config_file);
    if (join_mode == "none") {
      config.setJoinInfo({JoinInfo::kNone, ""});
    } else if (join_mode == "ip") {
      std::string ip = readRequired<std::string>("join_ip", config_file);
      uint16_t port = readRequired<uint16_t>("join_port", config_file);
      config.setJoinInfo({JoinInfo::kIp, ip, port});
    } else if (join_mode == "discovery") {
      config.setJoinInfo({JoinInfo::kDiscovery, ""});
    }

    TimeoutLengthsContainer timeout_lengths_container{};

    timeout_lengths_container.bootstrap_response =
        config_file["bootstrap_response"].as<uint64_t>(minhton::kDefaultTimeoutLength);
    timeout_lengths_container.join_response =
        config_file["join_response"].as<uint64_t>(minhton::kDefaultTimeoutLength);
    timeout_lengths_container.join_accept_ack_response =
        config_file["join_accept_ack_response"].as<uint64_t>(minhton::kDefaultTimeoutLength);
    timeout_lengths_container.replacement_offer_response =
        config_file["replacement_offer_response"].as<uint64_t>(minhton::kDefaultTimeoutLength);
    timeout_lengths_container.replacement_ack_response =
        config_file["replacement_ack_response"].as<uint64_t>(minhton::kDefaultTimeoutLength);
    timeout_lengths_container.dsn_aggregation =
        config_file["dsn_aggregation"].as<uint64_t>(minhton::kDefaultTimeoutLength);
    timeout_lengths_container.inquiry_aggregation =
        config_file["inquiry_aggregation"].as<uint64_t>(minhton::kDefaultTimeoutLength);

    config.setTimeoutLengthsContainer(timeout_lengths_container);

    config.setLogLevel(config_file["log_level"].as<std::string>("info"));
  } catch (const std::exception &e) {
    throw std::runtime_error("parsing config file failed");
  }
  return config;
}

}  // namespace minhton::config
