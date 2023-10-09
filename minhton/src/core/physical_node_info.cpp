// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#include "minhton/core/physical_node_info.h"

#include <arpa/inet.h>

#include <cmath>
#include <regex>
#include <stdexcept>

namespace minhton {

static constexpr int kBitshift1{24};
static constexpr int kBitshift2{16};
static constexpr int kBitshift3{8};

PhysicalNodeInfo::PhysicalNodeInfo(const std::string &address, uint16_t port) {
  try {
    this->setAddress(address);
  } catch (const std::exception &e) {
    throw std::invalid_argument(e.what());
  }

  try {
    this->setPort(port);
  } catch (const std::exception &e) {
    throw std::invalid_argument("Port has to be valid");
  }
}

uint16_t PhysicalNodeInfo::getPort() const { return this->port_; }
std::string PhysicalNodeInfo::getAddress() const { return this->address_; }

void PhysicalNodeInfo::setPort(uint16_t port) {
  if (port < PORT_MIN || port >= PORT_MAX) throw std::invalid_argument(INVALID_PORT);

  this->port_ = port;
}
void PhysicalNodeInfo::setAddress(std::string address) {
  if (address.length() == 0) {
    throw std::invalid_argument(INVALID_IP_ADDRESS);
  }

  if (!PhysicalNodeInfo::isIpv4Address(address) && !PhysicalNodeInfo::isIpv6Address(address)) {
    throw std::invalid_argument(INVALID_IP_ADDRESS);
  }

  this->address_ = address;
}

// methods for validation of an ip address
bool PhysicalNodeInfo::isIpv4Address(const std::string &str) {
  struct sockaddr_in sa {};
  return inet_pton(AF_INET, str.c_str(), &(sa.sin_addr)) != 0;
}

bool PhysicalNodeInfo::isIpv6Address(const std::string &str) {
  struct sockaddr_in6 sa {};
  return inet_pton(AF_INET6, str.c_str(), &(sa.sin6_addr)) != 0;
}

std::string PhysicalNodeInfo::getString() const {
  return this->address_ + ":" + std::to_string(this->port_);
}

///
/// We are initialized, if our address and port are valid.
///
bool PhysicalNodeInfo::isInitialized() const {
  return this->port_ != 0 && !this->address_.empty() &&
         PhysicalNodeInfo::isIpv4Address(this->address_);
}

uint32_t PhysicalNodeInfo::getAddressValue() const {
  if (!this->isInitialized()) {
    return 0;
  }

  std::string addr_string = this->address_;
  std::regex regex("\\d{1,3}");

  auto bytes_begin = std::sregex_iterator(addr_string.begin(), addr_string.end(), regex);
  auto bytes_end = std::sregex_iterator();

  std::vector<int16_t> bytes;
  for (std::sregex_iterator i = bytes_begin; i != bytes_end; ++i) {
    std::smatch match = *i;
    std::string match_str = match.str();
    int16_t num = std::stoi(match_str);
    bytes.push_back(num);
  }

  // shifting IP into single 32bit integer
  uint32_t ip = bytes[0];
  ip = ip << kBitshift1;
  ip |= bytes[1] << kBitshift2;
  ip |= bytes[2] << kBitshift3;
  ip |= bytes[3];

  return ip;
}

bool operator==(const minhton::PhysicalNodeInfo &n1, const minhton::PhysicalNodeInfo &n2) {
  return n1.getPort() == n2.getPort() && n1.getAddress() == n2.getAddress();
}

bool operator!=(const minhton::PhysicalNodeInfo &n1, const minhton::PhysicalNodeInfo &n2) {
  return !(n1 == n2);
}

bool operator<(const minhton::PhysicalNodeInfo &n1, const minhton::PhysicalNodeInfo &n2) {
  return n1.getAddressValue() + n1.getPort() * pow(256,  // NOLINT(readability-magic-numbers)
                                                   4) <  // NOLINT(readability-magic-numbers)
         n2.getAddressValue() + n2.getPort() * pow(256,  // NOLINT(readability-magic-numbers)
                                                   4);   // NOLINT(readability-magic-numbers)
}

bool operator<=(const minhton::PhysicalNodeInfo &n1, const minhton::PhysicalNodeInfo &n2) {
  return n1.getAddressValue() + n1.getPort() * pow(256,   // NOLINT(readability-magic-numbers)
                                                   4) <=  // NOLINT(readability-magic-numbers)
         n2.getAddressValue() + n2.getPort() * pow(256,   // NOLINT(readability-magic-numbers)
                                                   4);    // NOLINT(readability-magic-numbers)
}

bool operator>(const minhton::PhysicalNodeInfo &n1, const minhton::PhysicalNodeInfo &n2) {
  return !(n1 <= n2);
}

bool operator>=(const minhton::PhysicalNodeInfo &n1, const minhton::PhysicalNodeInfo &n2) {
  return !(n1 < n2);
}

}  // namespace minhton
