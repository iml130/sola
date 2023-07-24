// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#ifndef MINHTON_CORE_LOGICAL_NODE_INFO_H_
#define MINHTON_CORE_LOGICAL_NODE_INFO_H_

#include <string>

#include "minhton/message/serialize.h"
#include "solanet/uuid.h"
#include "solanet/uuid_generator.h"

namespace minhton

{

/// LogicalNodeInfo represents the position of a peer within the p2p network.
/// Each peer has a unique position (level:number) in the range of 0..(m^level)-1
///
class LogicalNodeInfo {
public:
  LogicalNodeInfo();

  ///
  /// Creates a LogicalNodeInfo object without a given position. It's optional to give a fanout.
  /// As default the k_FANOUT_DEFAULT value gets set. Level and Number are being set as 0.
  /// This object is set as not initialized. There is no possibility of initializing it.
  /// This constructor is there to represent a non-existing peer.
  ///
  /// Typical usage:
  /// \code
  ///    LogicalNodeInfo l_node_info(3);
  /// \endcode
  ///
  /// \param fanout maximum number of children a node can have
  ///
  /// \returns nothing on success, otherwise throws std::invalid_argument in case of an invalid
  /// fanout
  explicit LogicalNodeInfo(uint16_t fanout);

  ///
  /// Creates a LogicalNodeInfo object with a given position but without a fanout.
  /// Directly after the fanout is known, setFanout has to be called
  /// to set the fanout. Otherwise the fanout stays set as 0 and an invalid
  /// position might not get recognized.
  ///
  /// Typical usage:
  /// \code
  ///    LogicalNodeInfo l_node_info(1, 0, true);
  /// \endcode
  ///
  /// \param level level in the tree, root has level 0
  /// \param number number in the tree, the leftmost node on the level has number 0
  ///
  /// \returns nothing
  LogicalNodeInfo(uint32_t level, uint32_t number);

  ///
  /// Creates a LogicalNodeInfo object with a given position and fanout. It gets set as initialized.
  /// It gets verified if the position with the given fanout is valid.
  ///
  /// Typical usage:
  /// \code
  ///    LogicalNodeInfo l_node_info(1, 1, 2);
  /// \endcode
  ///
  /// \param level level in the tree, root has level 0
  /// \param number number in the tree, the leftmost node on the level has number 0
  /// \param fanout maximum number of children a node can have
  ///
  /// \returns nothing on success, otherwise throws std::invalid_argument in case of an invalid
  /// level:number or fanout
  LogicalNodeInfo(uint32_t level, uint32_t number, uint16_t fanout);

  /// \returns the level of this peer within MINHTON tree
  uint32_t getLevel() const;

  /// \returns the number of this peer within MINHTON tree
  uint32_t getNumber() const;

  /// \returns a readable representation of information about this peer
  std::string getString() const;

  /// \returns the fanout of this MINHTON tree
  uint16_t getFanout() const;

  /// \returns the uuid string of this peer
  std::string getUuid() const;

  /// \returns the uuid of this peer
  solanet::UUID getRawUuid() const;

  /// \returns the horizontal value defined by the TreeMapper of this peer
  double getHorizontalValue() const;

  ///
  /// Sets the fanout of this peer
  ///
  /// Typical usage:
  /// \code
  ///   setFanout(3);
  /// \endcode
  ///
  /// \param fanout
  void setFanout(uint16_t fanout);

  ///
  /// Sets the position of a peer depending on the previous set fanout
  ///
  /// Typical usage:
  /// \code
  ///   setPosition(0,0);
  /// \endcode
  ///
  /// \param level for the new level
  /// \param number for the new number
  ///
  /// \returns nothing on success, otherwise throws std::invalid_argument in case of an invalid
  /// level:number
  void setPosition(uint32_t level, uint32_t number);

  ///
  /// Sets the position of peer based on the passed peer.
  /// All information get copied, also fanout and initialization status.
  ///
  /// Typical usage:
  /// \code
  ///   setPosition(other);
  /// \endcode
  ///
  /// \param other peer with the new level, number, fanout and initialized
  ///
  /// \returns nothing on succes, otherwise throws std::invalid_argument in case of an invalid
  /// level:number
  void setPosition(LogicalNodeInfo other);

  ///
  /// Returns the current state of the peer if the peer is proper initialized.
  /// It depends on the constructor call.
  ///
  /// Typical usage:
  /// \code
  ///   isInitialized();
  /// \endcode
  ///
  /// \returns true if the peer has proper level:number:fanout
  bool isInitialized() const;

  ///
  /// Checks if the current peer is the root peer, in fact level = 0 and number = 0
  ///
  /// Typical usage:
  /// \code
  ///   isRoot();
  /// \endcode
  ///
  /// \returns true if peer root, otherwise false.
  bool isRoot() const;

  ///
  /// Checks if another peer has the same level
  ///
  /// Typical usage:
  /// \code
  ///   isSameLevel(anotherPeer);
  /// \endcode
  ///
  /// \param other instance of a peer which is compared to.
  ///
  /// \returns true if same level, otherwise false.
  bool isSameLevel(LogicalNodeInfo other) const;

  ///
  /// Checks if another peer has a deeper/ higher (>) level.
  /// E.g. a peer with level 1 is deeper/higher than a peer (root) on level 0
  ///
  /// Typical usage:
  /// \code
  ///   isDeeperThan(anotherPeer);
  /// \endcode
  ///
  /// \param other instance of a peer which is compared to.
  ///
  /// \returns true if same level, otherwise false.
  bool isDeeperThan(LogicalNodeInfo other) const;

  ///
  /// Checks if another peer has is on the same or on a deeper/ higher (>=) level.
  /// E.g. a peer on level 2 is deeper/higher or on the same level than a peer (root) on level 0
  /// The same peer on level 2 is also on the same level as another peer on level 2
  ///
  /// Typical usage:
  /// \code
  ///   isDeeperThanOrSameLevel(anotherPeer);
  /// \endcode
  ///
  /// \param other instance of a peer which is compared to.
  ///
  /// \returns true if same level, otherwise false.
  bool isDeeperThanOrSameLevel(LogicalNodeInfo other) const;

  ///
  /// Checks if the current peer with its level:number is part of the prio-node set
  ///
  /// Typical usage:
  /// \code
  ///   isPrioNode();
  /// \endcode
  ///
  /// \returns true if peer is a prio-node, otherwise false.
  bool isPrioNode() const;

  friend bool operator==(const minhton::LogicalNodeInfo &p1, const minhton::LogicalNodeInfo &p2);
  friend bool operator!=(const minhton::LogicalNodeInfo &p1, const minhton::LogicalNodeInfo &p2);
  friend bool operator<(const minhton::LogicalNodeInfo &p1, const minhton::LogicalNodeInfo &p2);
  friend bool operator<=(const minhton::LogicalNodeInfo &p1, const minhton::LogicalNodeInfo &p2);
  friend bool operator>(const minhton::LogicalNodeInfo &p1, const minhton::LogicalNodeInfo &p2);
  friend bool operator>=(const minhton::LogicalNodeInfo &p1, const minhton::LogicalNodeInfo &p2);

  SERIALIZE(level_, number_, fanout_, uuid_, initialized_);

private:
  uint32_t level_ = 0;
  uint32_t number_ = 0;
  uint16_t fanout_ = 0;
  solanet::UUID uuid_ = solanet::generateUUID();
  bool initialized_ = false;
};

struct LogicalNodeInfoHasher {
  std::size_t operator()(const minhton::LogicalNodeInfo &l_node_info) const {
    using std::hash;
    using std::size_t;
    using std::string;

    std::string rep = std::to_string(l_node_info.getLevel()) + ":" +
                      std::to_string(l_node_info.getNumber()) + ":" +
                      std::to_string(l_node_info.getFanout()) +
                      (l_node_info.isInitialized() ? "init" : "uninit");
    std::size_t res = std::hash<std::string>()(rep);
    return res;
  }
};

}  // namespace minhton
#endif
