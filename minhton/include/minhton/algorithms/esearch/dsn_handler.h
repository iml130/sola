// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#ifndef ALGORITHMS_ESEARCH_DSN_HANDLER_H_
#define ALGORITHMS_ESEARCH_DSN_HANDLER_H_

#include <memory>
#include <queue>
#include <unordered_map>
#include <vector>

#include "minhton/algorithms/esearch/distributed_data.h"
#include "minhton/algorithms/esearch/find_query.h"
#include "minhton/core/routing_information.h"

namespace minhton {

class DSNHandler {
public:
  DSNHandler(std::shared_ptr<RoutingInformation> routing_info,
             std::function<void(const NodeInfo &node)> request_attribute_inquiry_callback);

  /// TODO unit tests
  ///
  /// This method tells us whether this node is an active DSN or not.
  /// If we are not at a DSN position, it returns false.
  /// If we are at a DSN position but then change to a non-DSN position,
  /// it will return false and our information gets reset by the
  /// checkPosition() method.
  ///
  /// \returns true if this node is an active DSN
  bool isActive();

  ///
  /// When answering find queries we need to know which nodes are
  /// undecided and about which keys we need further information.
  /// For this we use our current cover area information.
  ///
  /// \param query find query which we want to answer
  /// \param all_information_present boolean whether we can assume that we received all information
  /// we need to know, or not yet
  /// \param timestamp_now current timestamp to decide which values are
  /// outdated
  ///
  /// \returns a vector of nodes which are undecided, including a vector of their missing keys
  std::vector<std::tuple<NodeInfo, std::vector<NodeData::Key>>> getUndecidedNodesAndMissingKeys(
      FindQuery &query, bool all_information_present, const uint64_t &timestamp_now);

  ///
  /// Determine which nodes are true for a given find query.
  /// For this we use our current cover area information.
  ///
  /// \param query find query which we want to answer
  /// \param timestamp_nowcurrent timestamp to decide which values are
  /// outdated
  ///
  /// \returns a vector of nodes which are definitely true
  std::vector<NodeInfo> getTrueNodes(FindQuery &query, const uint64_t &timestamp_now);

  ///
  /// If a neighbor in routing information is being changed,
  /// this method will be called.
  /// If this change happened within our cover area,
  /// we need to update our cover area information accordingly.
  ///
  /// \param neighbor the node that has been changed
  /// \param relationship our relationship to the neighbor
  void onNeighborChangeNotification(const minhton::NodeInfo &neighbor,
                                    NeighborRelationship relationship);

  void onNodeInfoChangeNotification(const minhton::NodeInfo &previous_node_info,
                                    const minhton::NodeInfo &new_node_info);

  void requestAttributeInformation(PeerInfo peer, const DistributedData &distr_data);

  ///
  /// If we have subscribed or inquired an attributes value
  /// and get a response via a AttributeInquiryAnswer
  /// or SubscriptionUpdate message, this method will be called.
  ///
  /// We update our stored information about the node accordingly.
  ///
  /// \param inquired_node the node about which we gain updated information
  /// \param attribute_values_and_types map from attribute keys to its values and types
  /// \param update_timestamp current timestamp
  void updateInquiredOrSubscribedAttributeValues(
      const NodeInfo &inquired_or_updated_node,
      std::unordered_map<NodeData::Key, NodeData::ValueAndType> attribute_values_and_types,
      uint64_t update_timestamp);

  // TODO unit tests
  void updateRemovedAttributes(const NodeInfo &inquired_node,
                               std::vector<NodeData::Key> removed_keys);

  /// \returns current cover data
  std::unordered_map<PeerInfo, DistributedData, PeerInfoHasher> getCoverData();

  // TODO unit tests
  void notifyAboutQueryRequest(const FindQuery &query, uint64_t request_timestamp);

  // LATER unit tests
  std::unordered_map<NodeInfo, std::vector<NodeData::Key>, NodeInfoHasher>
  getNodesAndKeysToSubscribe(uint64_t const &timestamp_now);

  // LATER unit tests
  std::unordered_map<NodeInfo, std::vector<NodeData::Key>, NodeInfoHasher>
  getNodesAndKeysToUnsubscribe(uint64_t const &timestamp_now);

  // TODO unit tests
  void setPlacedSubscriptionOrders(
      std::unordered_map<NodeInfo, std::vector<NodeData::Key>, NodeInfoHasher>
          subscription_orders_map);

  // TODO unit tests
  void setPlacedUnsubscriptionOrders(
      std::unordered_map<NodeInfo, std::vector<NodeData::Key>, NodeInfoHasher>
          unsubscription_orders_map);

  NodeData::Attributes getNodeAttributes(const NodeInfo &node);

private:
  /// Checking whether the position of our node has changed.
  /// If it did change, the cover data will and area be updated.
  void checkPosition();

  /// Initializing the cover data and area after a change in positions
  /// if the new position is a DSN.
  void buildCoverArea();

  void buildExtendedCoverArea();

  void buildTempCoverArea();

  /// If this node is an active dominating set node
  bool is_active_;

  bool was_previously_active_ = false;

  /// Our previous node information, to recognize a change in position.
  NodeInfo previous_node_info_;

  /// Access to the nodes routing information through a shared pointer.
  std::shared_ptr<RoutingInformation> routing_info_;

  /// The heart of the DSN Handler.
  /// A map from peer info to distributed data
  /// to store information about each node in our cover area.
  /// It requires constant maintanance if a neighbor or our own position changes
  std::unordered_map<PeerInfo, DistributedData, PeerInfoHasher> cover_data_;

  /// Precalculated vector of all positions within the cover area,
  /// depending on our current position.
  std::vector<std::tuple<uint32_t, uint32_t>> cover_area_positions_;

  std::vector<std::tuple<uint32_t, uint32_t>> extended_cover_area_positions_;
  std::unordered_map<PeerInfo, DistributedData, PeerInfoHasher> extended_cover_data_;

  /// Storing how often keys have been requested, by storing the timestamps of the request.
  /// With those, the request frequency can be calculated.
  std::unordered_map<NodeData::Key, std::queue<uint64_t>> request_timestamps_;

  /// Limit on how many request and subscription update timestamps will be stored per key.
  uint8_t timestamp_storage_limit_;

  std::function<void(const NodeInfo &)> request_attribute_inquiry_callback_;

  /// Helper method to calculate the frequency of request or subscription updates
  /// depending on the stored timestamps.
  ///
  /// \param timestamps queue of past timestamps
  /// \param timestamp_now current timestamp
  ///
  /// \returns frequency in Hz
  static double calculateFrequency(std::queue<uint64_t> timestamps, uint64_t const &timestamp_now);
};

}  // namespace minhton

#endif
