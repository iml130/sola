// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#ifndef MINHTON_UTILS_CONFIG_NODE_H_
#define MINHTON_UTILS_CONFIG_NODE_H_

#include <cstdint>
#include <vector>

#include "minhton/core/constants.h"
#include "minhton/logging/logger.h"
#include "minhton/utils/algorithm_types_container.h"
#include "minhton/utils/timeout_lengths_container.h"

namespace minhton {
///
/// MINHTON needs few information before it can start to work. This class represents the parameters
/// to start MINHTON
///

struct JoinInfo {
  enum { kIp, kNone, kDiscovery } join_mode;
  std::string ip;
  uint16_t port = 0;
};

class ConfigNode {
private:
  // IP to bind to
  std::string own_ip_;
  /// is only used when the node is root. But it is not required to set since the default fanout
  /// value of a MINHTON network is 2.
  uint16_t fanout_ = 0;

  /// is only used when the node is root. But it is not required to set since the default treemapper
  /// value of a MINHTON network is 100.
  uint16_t treemapper_ = kTreeMapperRootValue;  // missing getter

  /// logger type
  std::vector<Logger::LoggerPtr> logger_;

  /// Join info
  JoinInfo join_info_ = JoinInfo{JoinInfo::kNone, ""};

  /// Log level
  std::string log_level_;

  bool root_ = false;

  bool verbose_ = false;

  TimeoutLengthsContainer timeout_lengths_container_{};
  AlgorithmTypesContainer algorithm_types_container_{};

public:
  uint16_t getFanout() const;
  void setFanout(uint16_t fanout);

  std::string getOwnIP() const;
  void setOwnIP(const std::string &own_ip);

  bool isRoot() const;
  void setIsRoot(bool isRoot);

  bool getVerbose() const;
  void setVerbose(bool verbose);

  uint16_t getTreemapper() const;
  void setTreemapper(uint16_t treemapper);

  std::vector<Logger::LoggerPtr> getLogger() const;
  void setLogger(std::vector<Logger::LoggerPtr> logger);

  JoinInfo getJoinInfo() const;
  void setJoinInfo(JoinInfo info);

  std::string getLogLevel() const;
  void setLogLevel(const std::string &log_level);

  void setAlgorithmTypesContainer(const AlgorithmTypesContainer &algorithm_types_container);
  AlgorithmTypesContainer getAlgorithmTypesContainer() const;

  void setTimeoutLengthsContainer(const TimeoutLengthsContainer &timeout_lengths_container);
  TimeoutLengthsContainer getTimeoutLengthsContainer() const;
};
}  // namespace minhton

#endif
