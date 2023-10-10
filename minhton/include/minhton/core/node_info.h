// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#ifndef MINHTON_CORE_NODE_INFO_H_
#define MINHTON_CORE_NODE_INFO_H_

#include <string>

#include "minhton/core/logical_node_info.h"
#include "minhton/core/physical_node_info.h"
#include "solanet/serializer/serialize.h"

namespace minhton {

enum NodeStatus : uint8_t { kUninit = 0, kRunning = 1, kLeft = 2, kFailed = 3 };

///
/// A NodeInfo is based on a LogicalNodeInfo and PhysicalNodeInfo. These two classes build the core
/// of a node in our peer2peer network
///
class NodeInfo {
public:
  NodeInfo() = default;

  ///
  /// Constructing an initialized NodeInfo object with the given information.
  ///
  /// \param level valid level of a MINHTON m-ary tree
  /// \param number valid number of a MINHTON m-ary tree
  /// \param fanout valid port of a MINHTON m-ary tree
  ///
  NodeInfo(uint32_t level, uint32_t number, uint16_t fanout);

  ///
  /// Constructing an initialized NodeInfo object with the given information.
  ///
  /// \param level valid level of a MINHTON m-ary tree
  /// \param number valid number of a MINHTON m-ary tree
  /// \param fanout valid port of a MINHTON m-ary tree
  /// \param address valid IPv4 address string
  /// \param uint16_t valid network port
  ///
  NodeInfo(uint32_t level, uint32_t number, uint16_t fanout, const std::string &address,
           uint16_t port);

  /// \returns A LogicalNodeInfo, which represents the position (Level/Number) of a peer within the
  /// p2p network
  minhton::LogicalNodeInfo getLogicalNodeInfo() const;

  ///
  /// Sets the LogicalNodeInfo within the NodeInfo object
  ///
  /// \param l_node_info valid LogicalNodeInfo instance
  ///
  void setLogicalNodeInfo(const minhton::LogicalNodeInfo &l_node_info);

  /// \returns A PhysicalNodeInfo, which represents the remote host, by having a IPv4/IPv6 IP
  /// address and a port
  minhton::PhysicalNodeInfo getPhysicalNodeInfo() const;

  ///
  /// Sets the PhysicalNodeInfo within the NodeInfo object
  ///
  /// \param p_node_info valid LogicalNodeInfo instance
  ///
  void setPhysicalNodeInfo(const minhton::PhysicalNodeInfo &p_node_info);

  std::string getString() const;

  ///
  /// Returns the current state of the node if the node is properly initialized.
  /// If a node is initialized, it exists.
  /// This method depends on initialized state of the LogicalNodeInfo and PhysicalNodeInfo
  ///
  /// Typical usage:
  /// \code
  ///   isInitialized();
  /// \endcode
  ///
  /// \returns true if the node is proper initialized, otherwise false
  bool isInitialized() const;

  // -------------- LogicalNodeInfo access Helper methods -------------------

  /// Helper method, which sets the position for the internal LogicalNodeInfo object
  void setPosition(uint32_t level, uint32_t number);

  /// Helper method, which sets the position for the internal LogicalNodeInfo object
  void setPosition(LogicalNodeInfo other);

  /// Helper method, which sets the fanout for the internal LogicalNodeInfo object
  void setFanout(uint16_t fanout);

  /// \returns the level of LogicalNodeInfo
  uint32_t getLevel() const;

  /// \returns the number of LogicalNodeInfo
  uint32_t getNumber() const;

  /// \returns the fanout of LogicalNodeInfo
  uint16_t getFanout() const;

  /// \returns the status of the node
  NodeStatus getStatus() const;

  /// sets the status of the node
  void setStatus(NodeStatus status);

  /// \returns true if the node consists of an actual known peer, otherwise false
  bool isValidPeer() const;

  // -------------- PhysicalNodeInfo access Helper methods -------------------

  /// Helper method, which sets the port for PhysicalNodeInfo
  void setPort(uint16_t port);

  /// Helper method, which sets the address for PhysicalNodeInfo
  void setAddress(const std::string &address);

  /// \returns the port of PhysicalNodeInfo
  uint16_t getPort() const;

  /// \returns the address of PhysicalNodeInfo
  std::string getAddress() const;

  /// \returns a unique number representing the ip address of PhysicalNodeInfo
  uint32_t getAddressValue() const;

  friend bool operator==(const minhton::NodeInfo &n1, const minhton::NodeInfo &n2);
  friend bool operator!=(const minhton::NodeInfo &n1, const minhton::NodeInfo &n2);
  friend bool operator<(const minhton::NodeInfo &n1, const minhton::NodeInfo &n2);
  friend bool operator<=(const minhton::NodeInfo &n1, const minhton::NodeInfo &n2);
  friend bool operator>(const minhton::NodeInfo &n1, const minhton::NodeInfo &n2);
  friend bool operator>=(const minhton::NodeInfo &n1, const minhton::NodeInfo &n2);

  SERIALIZE(l_node_info_, p_node_info_)

private:
  minhton::LogicalNodeInfo l_node_info_;
  minhton::PhysicalNodeInfo p_node_info_;

  NodeStatus status_{kUninit};
};

struct NodeInfoHasher {
  std::size_t operator()(const minhton::NodeInfo &node) const {
    using std::hash;
    using std::size_t;

    std::size_t peer_hash = minhton::LogicalNodeInfoHasher()(node.getLogicalNodeInfo());
    std::size_t network_hash = minhton::PhysicalNodeInfoHasher()(node.getPhysicalNodeInfo());

    return peer_hash ^ network_hash;
  }
};

}  // namespace minhton
#endif
