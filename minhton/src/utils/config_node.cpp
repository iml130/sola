// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#include "minhton/utils/config_node.h"

#include "minhton/core/constants.h"
namespace minhton {
std::string ConfigNode::getOwnIP() const { return own_ip_; }
void ConfigNode::setOwnIP(const std::string &own_ip) { own_ip_ = own_ip; }

uint16_t ConfigNode::getFanout() const { return this->fanout_; }
void ConfigNode::setFanout(uint16_t fanout) { this->fanout_ = fanout; }

bool ConfigNode::isRoot() const { return this->root_; }
void ConfigNode::setIsRoot(bool isRoot) { this->root_ = isRoot; }

void ConfigNode::setVerbose(bool verbose) { this->verbose_ = verbose; }
bool ConfigNode::getVerbose() const { return this->verbose_; }

uint16_t ConfigNode::getTreemapper() const { return this->treemapper_; }
void ConfigNode::setTreemapper(uint16_t treemapper) { this->treemapper_ = treemapper; }

std::vector<Logger::LoggerPtr> ConfigNode::getLogger() const { return this->logger_; }
void ConfigNode::setLogger(const std::vector<Logger::LoggerPtr> logger) { this->logger_ = logger; }

std::string ConfigNode::getLogLevel() const { return this->log_level_; }
void ConfigNode::setLogLevel(const std::string &log_level) { this->log_level_ = log_level; }

JoinInfo ConfigNode::getJoinInfo() const { return join_info_; }
void ConfigNode::setJoinInfo(JoinInfo info) { join_info_ = std::move(info); }

void ConfigNode::setAlgorithmTypesContainer(
    const AlgorithmTypesContainer &algorithm_types_container) {
  algorithm_types_container_ = algorithm_types_container;
}

AlgorithmTypesContainer ConfigNode::getAlgorithmTypesContainer() const {
  return algorithm_types_container_;
}

void ConfigNode::setTimeoutLengthsContainer(
    const TimeoutLengthsContainer &timeout_lengths_container) {
  timeout_lengths_container_ = timeout_lengths_container;
}

TimeoutLengthsContainer ConfigNode::getTimeoutLengthsContainer() const {
  return timeout_lengths_container_;
}

}  // namespace minhton
