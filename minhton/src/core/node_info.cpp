// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#include "minhton/core/node_info.h"
namespace minhton {

NodeInfo::NodeInfo(uint32_t level, uint32_t number, uint16_t fanout) {
  this->network_info_ = minhton::NetworkInfo();
  this->peer_info_ = minhton::PeerInfo(level, number, fanout);
}

NodeInfo::NodeInfo(uint32_t level, uint32_t number, uint16_t fanout, const std::string &address,
                   uint16_t port) {
  this->network_info_ = minhton::NetworkInfo(address, port);
  this->peer_info_ = minhton::PeerInfo(level, number, fanout);
}

std::string NodeInfo::getString() const {
  std::string init = this->isInitialized() ? "init" : "uninit";

  std::string temp = "( " + this->getPeerInfo().getString() + " | " +
                     this->getNetworkInfo().getString() + " | " + init + " )";

  return temp;
}

PeerInfo NodeInfo::getPeerInfo() const { return this->peer_info_; }

void NodeInfo::setPeerInfo(const minhton::PeerInfo &peer_info) { this->peer_info_ = peer_info; }

NetworkInfo NodeInfo::getNetworkInfo() const { return this->network_info_; }
void NodeInfo::setNetworkInfo(const minhton::NetworkInfo &network_info) {
  this->network_info_ = network_info;
}

bool NodeInfo::isInitialized() const {
  return (this->peer_info_.isInitialized() && this->network_info_.isInitialized());
}

void NodeInfo::setPosition(uint32_t level, uint32_t number) {
  this->peer_info_.setPosition(level, number);
}

void NodeInfo::setPosition(PeerInfo other) { this->peer_info_.setPosition(other); }

void NodeInfo::setFanout(uint16_t fanout) { this->peer_info_.setFanout(fanout); }

uint32_t NodeInfo::getLevel() const { return this->peer_info_.getLevel(); }

uint32_t NodeInfo::getNumber() const { return this->peer_info_.getNumber(); }

uint16_t NodeInfo::getFanout() const { return this->peer_info_.getFanout(); }

void NodeInfo::setPort(uint16_t port) { this->network_info_.setPort(port); }

void NodeInfo::setAddress(const std::string &address) { this->network_info_.setAddress(address); }

uint16_t NodeInfo::getPort() const { return this->network_info_.getPort(); }

std::string NodeInfo::getAddress() const { return this->network_info_.getAddress(); }

uint32_t NodeInfo::getAddressValue() const { return this->network_info_.getAddressValue(); }

NodeStatus NodeInfo::getStatus() const { return this->status_; }
void NodeInfo::setStatus(NodeStatus status) { this->status_ = status; }

bool NodeInfo::isValidPeer() const { return this->peer_info_.isInitialized(); }

bool operator==(const minhton::NodeInfo &n1, const minhton::NodeInfo &n2) {
  return n1.getPeerInfo() == n2.getPeerInfo() && n1.getNetworkInfo() == n2.getNetworkInfo();
}

bool operator!=(const minhton::NodeInfo &n1, const minhton::NodeInfo &n2) { return !(n1 == n2); }

bool operator<(const minhton::NodeInfo &n1, const minhton::NodeInfo &n2) {
  return n1.getPeerInfo() < n2.getPeerInfo();
}

bool operator<=(const minhton::NodeInfo &n1, const minhton::NodeInfo &n2) {
  return n1.getPeerInfo() <= n2.getPeerInfo();
}

bool operator>(const minhton::NodeInfo &n1, const minhton::NodeInfo &n2) { return !(n1 <= n2); }

bool operator>=(const minhton::NodeInfo &n1, const minhton::NodeInfo &n2) { return !(n1 < n2); }

}  // namespace minhton
