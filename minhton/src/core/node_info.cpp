// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#include "minhton/core/node_info.h"
namespace minhton {

NodeInfo::NodeInfo(uint32_t level, uint32_t number, uint16_t fanout) {
  this->p_node_info_ = minhton::PhysicalNodeInfo();
  this->l_node_info_ = minhton::LogicalNodeInfo(level, number, fanout);
}

NodeInfo::NodeInfo(uint32_t level, uint32_t number, uint16_t fanout, const std::string &address,
                   uint16_t port) {
  this->p_node_info_ = minhton::PhysicalNodeInfo(address, port);
  this->l_node_info_ = minhton::LogicalNodeInfo(level, number, fanout);
}

std::string NodeInfo::getString() const {
  std::string init = this->isInitialized() ? "init" : "uninit";

  std::string temp = "( " + this->getLogicalNodeInfo().getString() + " | " +
                     this->getPhysicalNodeInfo().getString() + " | " + init + " )";

  return temp;
}

LogicalNodeInfo NodeInfo::getLogicalNodeInfo() const { return this->l_node_info_; }

void NodeInfo::setLogicalNodeInfo(const minhton::LogicalNodeInfo &l_node_info) {
  this->l_node_info_ = l_node_info;
}

PhysicalNodeInfo NodeInfo::getPhysicalNodeInfo() const { return this->p_node_info_; }
void NodeInfo::setPhysicalNodeInfo(const minhton::PhysicalNodeInfo &p_node_info) {
  this->p_node_info_ = p_node_info;
}

bool NodeInfo::isInitialized() const {
  return (this->l_node_info_.isInitialized() && this->p_node_info_.isInitialized());
}

void NodeInfo::setPosition(uint32_t level, uint32_t number) {
  this->l_node_info_.setPosition(level, number);
}

void NodeInfo::setPosition(LogicalNodeInfo other) { this->l_node_info_.setPosition(other); }

void NodeInfo::setFanout(uint16_t fanout) { this->l_node_info_.setFanout(fanout); }

uint32_t NodeInfo::getLevel() const { return this->l_node_info_.getLevel(); }

uint32_t NodeInfo::getNumber() const { return this->l_node_info_.getNumber(); }

uint16_t NodeInfo::getFanout() const { return this->l_node_info_.getFanout(); }

void NodeInfo::setPort(uint16_t port) { this->p_node_info_.setPort(port); }

void NodeInfo::setAddress(const std::string &address) { this->p_node_info_.setAddress(address); }

uint16_t NodeInfo::getPort() const { return this->p_node_info_.getPort(); }

std::string NodeInfo::getAddress() const { return this->p_node_info_.getAddress(); }

uint32_t NodeInfo::getAddressValue() const { return this->p_node_info_.getAddressValue(); }

NodeStatus NodeInfo::getStatus() const { return this->status_; }
void NodeInfo::setStatus(NodeStatus status) { this->status_ = status; }

bool NodeInfo::isValidPeer() const { return this->l_node_info_.isInitialized(); }

bool operator==(const minhton::NodeInfo &n1, const minhton::NodeInfo &n2) {
  return n1.getLogicalNodeInfo() == n2.getLogicalNodeInfo() &&
         n1.getPhysicalNodeInfo() == n2.getPhysicalNodeInfo();
}

bool operator!=(const minhton::NodeInfo &n1, const minhton::NodeInfo &n2) { return !(n1 == n2); }

bool operator<(const minhton::NodeInfo &n1, const minhton::NodeInfo &n2) {
  return n1.getLogicalNodeInfo() < n2.getLogicalNodeInfo();
}

bool operator<=(const minhton::NodeInfo &n1, const minhton::NodeInfo &n2) {
  return n1.getLogicalNodeInfo() <= n2.getLogicalNodeInfo();
}

bool operator>(const minhton::NodeInfo &n1, const minhton::NodeInfo &n2) { return !(n1 <= n2); }

bool operator>=(const minhton::NodeInfo &n1, const minhton::NodeInfo &n2) { return !(n1 < n2); }

}  // namespace minhton
