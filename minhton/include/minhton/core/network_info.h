// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#ifndef MINHTON_CORE_NETWORK_INFO_H_
#define MINHTON_CORE_NETWORK_INFO_H_

#include <cstdint>
#include <string>
#include <vector>

#include "minhton/message/serialize.h"

namespace minhton {
#define PORT_MIN 1024
#define PORT_MAX UINT16_MAX

#define INVALID_PORT "Invalid Port"
#define INVALID_IP_ADDRESS "Adress has to be valid"

#define IPv4_ADDRESS "127.0.0.1"
#define IPv4_PORT 2906

///
/// A NetworkInfo represents the remote host, by having a IPv4/IPv6 IP address and a port
///
class NetworkInfo {
public:
  ///
  /// Constructing an empty uninitialized NetworkInfo object.
  ///
  NetworkInfo();

  ///
  /// Constructing an initialized NetworkInfo object with the given information.
  ///
  /// \param address valid IPv4address string
  /// \param port valid port number
  ///
  NetworkInfo(const std::string &address, uint16_t port);

  /// \returns the port of the object
  uint16_t getPort() const;

  ///
  /// Sets the port of the object, which needs to be in range of
  /// k_RANDOM_PORT_MINIMUM..k_RANDOM_PORT_MAXIMUM
  ///
  /// \param port valid port number
  ///
  ///
  void setPort(uint16_t port);

  /// \returns the address of the object as a string
  std::string getAddress() const;

  ///
  /// Sets the address of the object
  ///
  /// \param address valid IPv4address string
  ///
  void setAddress(std::string address);

  /// \returns a calculated unique value of the ip address as an uint32_t
  uint32_t getAddressValue() const;

  /// \returns a simple string representation of the object
  std::string getString() const;

  ///
  /// Returns the current state of the networkinfo if the node is proper initialized.
  ///
  /// Typical usage:
  /// \code
  ///   isInitialized();
  /// \endcode
  ///
  /// \returns true if the node has proper network information, otherwise false
  bool isInitialized() const;

  /// \returns true if the address and port are the same
  friend bool operator==(const minhton::NetworkInfo &n1, const minhton::NetworkInfo &n2);

  /// \returns true if n1 and n2 are not equal
  friend bool operator!=(const minhton::NetworkInfo &n1, const minhton::NetworkInfo &n2);

  /// \returns true if the calculated unique value of n1 is smaller than that of n2
  friend bool operator<(const minhton::NetworkInfo &n1, const minhton::NetworkInfo &n2);

  /// \returns true if the calculated unique value of n1 is smaller or equal than that of n2
  friend bool operator<=(const minhton::NetworkInfo &n1, const minhton::NetworkInfo &n2);

  /// \returns true if the calculated unique value of n1 is larger than that of n2
  friend bool operator>(const minhton::NetworkInfo &n1, const minhton::NetworkInfo &n2);

  /// \returns true if the calculated unique value of n1 is larger or equal than that of n2
  friend bool operator>=(const minhton::NetworkInfo &n1, const minhton::NetworkInfo &n2);

  SERIALIZE(address_, port_);

private:
  uint16_t port_;
  std::string address_;

  std::vector<std::string> splitString(const std::string &, char token);

  /// \returns true if the string is a valid IPv4address
  static bool isIpv4Address(const std::string &str);

  /// \returns true if the string is a valid ipv6 address
  static bool isIpv6Address(const std::string &str);
};

struct NetworkInfoHasher {
  std::size_t operator()(const minhton::NetworkInfo &net) const {
    using std::hash;
    using std::size_t;
    using std::string;

    std::string rep = (net.getAddress().empty() ? "invalidaddress" : net.getAddress()) + ":" +
                      std::to_string(net.getPort()) + ":" +
                      (net.isInitialized() ? "init" : "uninit");
    std::size_t res = std::hash<std::string>()(rep);
    return res;
  }
};

}  // namespace minhton
#endif
