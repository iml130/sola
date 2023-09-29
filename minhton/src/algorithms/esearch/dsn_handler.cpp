// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#include "minhton/algorithms/esearch/dsn_handler.h"

#include <algorithm>

#include "minhton/core/routing_calculations.h"

namespace minhton {

DSNHandler::DSNHandler(std::shared_ptr<RoutingInformation> routing_info,
                       std::function<void(const NodeInfo &)> request_attribute_inquiry_callback)
    : routing_info_(routing_info),
      request_attribute_inquiry_callback_(std::move(request_attribute_inquiry_callback)) {
  auto neighbor_change_callback = [this](const minhton::NodeInfo &new_node,
                                         NeighborRelationship relationship,
                                         [[maybe_unused]] const minhton::NodeInfo &old_node,
                                         [[maybe_unused]] uint16_t position) {
    onNeighborChangeNotification(new_node, relationship);
  };

  auto node_info_change_callback = [this](const minhton::NodeInfo &previous_node_info,
                                          const minhton::NodeInfo &new_node_info) {
    onNodeInfoChangeNotification(previous_node_info, new_node_info);
  };

  routing_info->addNeighborChangeSubscription(neighbor_change_callback);
  routing_info->addNodeInfoChangeSubscription(node_info_change_callback);

  checkPosition();

  this->is_active_ = false;
}

bool DSNHandler::isActive() {
  was_previously_active_ = is_active_;
  checkPosition();
  return is_active_;
}

void DSNHandler::checkPosition() {
  this->is_active_ = false;  // only setting to true, if we have reached the end

  // node info changed
  // updating everything to maintain cover_data_ map

  previous_node_info_ = routing_info_->getSelfNodeInfo();

  // if we are not initialized, invalidating everything we knew
  if (!routing_info_->getSelfNodeInfo().isInitialized()) {
    cover_data_.clear();
    extended_cover_data_.clear();
    return;
  }

  // if we are initialized, checking if we are a dsn

  bool we_are_dsn = routing_info_->areWeDSN();
  bool we_are_temp_dsn = routing_info_->areWeTempDSN();

  if (!we_are_dsn && !we_are_temp_dsn) {
    // not a dsn
    // clearning previous dsn info

    cover_data_.clear();
    extended_cover_data_.clear();
    return;
  }

  if (we_are_dsn && we_are_temp_dsn) {
    throw std::logic_error("We cannot be a dsn and temp dsn at the same time.");
  }

  // is a dsn
  // updating new dsn info
  this->is_active_ = true;

  if (we_are_dsn && !was_previously_active_) {
    buildCoverArea();

    if (!routing_info_->nextDSNExists()) {
      buildExtendedCoverArea();
    } else if (!extended_cover_data_.empty()) {
      extended_cover_data_.clear();
    }

  } else if (we_are_temp_dsn && !was_previously_active_) {
    // a temp dsn cannot have an extended cover area
    buildTempCoverArea();
  }
}

void DSNHandler::buildCoverArea() {
  auto position = routing_info_->getSelfNodeInfo();

  auto cover_area_positions =
      getCoverArea(position.getLevel(), position.getNumber(), position.getFanout());

  this->cover_area_positions_ = cover_area_positions;

  // removing entries which are not in the new cover area
  // probably all of them, but you cannot be sure
  for (auto it_cover_data = cover_data_.begin(); it_cover_data != cover_data_.end();) {
    auto it_positions = std::find_if(cover_area_positions.begin(), cover_area_positions.end(),
                                     [&](const std::tuple<uint32_t, uint32_t> tup) {
                                       return it_cover_data->first.getLevel() == std::get<0>(tup) &&
                                              it_cover_data->first.getNumber() == std::get<1>(tup);
                                     });

    if (it_positions == cover_area_positions.end()) {
      it_cover_data = cover_data_.erase(it_cover_data);
    } else {
      cover_area_positions.erase(it_positions);
      ++it_cover_data;
    }
  }

  // current status:
  // cover_data_ has all information which is still valid from the previous dsn position
  // cover_area_positions has all positions which still need to be added to cover_data_

  for (auto const &[level, number] : cover_area_positions) {
    auto cover_node = routing_info_->getNodeInfoByPosition(level, number);

    if (cover_node.isInitialized()) {
      DistributedData distr_data(cover_node.getPhysicalNodeInfo());
      cover_data_[cover_node.getLogicalNodeInfo()] = distr_data;
    }
  }
}

void DSNHandler::buildTempCoverArea() {
  uint16_t fanout = routing_info_->getFanout();
  auto first_child = routing_info_->getChild(0);
  auto const &[dsn_level, dsn_number] =
      getCoveringDSN(first_child.getLevel(), first_child.getNumber(), fanout);

  auto cover_area_positions = getCoverArea(dsn_level, dsn_number, fanout);

  auto not_in_temp_ca = [&](const std::tuple<uint32_t, uint32_t> &tupl) {
    return std::get<0>(tupl) != first_child.getLevel();
  };

  cover_area_positions.erase(
      std::remove_if(cover_area_positions.begin(), cover_area_positions.end(), not_in_temp_ca));
  cover_area_positions_ = cover_area_positions;

  for (auto const &[level, number] : cover_area_positions) {
    auto cover_node = routing_info_->getNodeInfoByPosition(level, number);

    if (cover_node.isInitialized()) {
      DistributedData distr_data(cover_node.getPhysicalNodeInfo());
      cover_data_[cover_node.getLogicalNodeInfo()] = distr_data;
    }
  }
}

void DSNHandler::buildExtendedCoverArea() {
  auto next_dsn = routing_info_->getNextDSN();
  uint16_t fanout = routing_info_->getFanout();

  if (!next_dsn.isInitialized()) {
    // if there is not a next DSN, there are not more nodes to cover on this level
    return;
  }

  auto next_cover_area_positions = getCoverArea(next_dsn.getLevel(), next_dsn.getNumber(), fanout);

  auto not_in_extended_ca = [&](const std::tuple<uint32_t, uint32_t> &tupl) {
    return std::get<0>(tupl) != next_dsn.getLevel() && std::get<1>(tupl) >= next_dsn.getNumber();
  };

  next_cover_area_positions.erase(std::remove_if(
      next_cover_area_positions.begin(), next_cover_area_positions.end(), not_in_extended_ca));

  extended_cover_area_positions_ = next_cover_area_positions;

  for (auto const &[level, number] : extended_cover_area_positions_) {
    auto extended_cover_node = routing_info_->getNodeInfoByPosition(level, number);

    if (extended_cover_node.isInitialized()) {
      DistributedData distr_data(extended_cover_node.getPhysicalNodeInfo());
      extended_cover_data_[extended_cover_node.getLogicalNodeInfo()] = distr_data;
    }
  }
}

void DSNHandler::onNeighborChangeNotification(const minhton::NodeInfo &neighbor,
                                              [[maybe_unused]] NeighborRelationship relationship) {
  if (!isActive()) {
    if (was_previously_active_) {
      // we were a temporary dsn -> might need to push our information to our actual dsn
      // TODO
    }

    return;
  }

  auto it_positions = std::find_if(cover_area_positions_.begin(), cover_area_positions_.end(),
                                   [&](const std::tuple<uint32_t, uint32_t> tup) {
                                     return neighbor.getLevel() == std::get<0>(tup) &&
                                            neighbor.getNumber() == std::get<1>(tup);
                                   });

  if (it_positions == cover_area_positions_.end()) {
    return;  // not in cover area -> not relevant
  }

  auto it_cover_data = cover_data_.find(neighbor.getLogicalNodeInfo());

  // possible cases:
  // PhysicalNodeInfo init - inside -> update if different
  // PhysicalNodeInfo not init - inside -> delete
  // PhysicalNodeInfo init - not inside -> insert
  // PhysicalNodeInfo not init - not inside -> nothing

  bool inside = it_cover_data != cover_data_.end();
  bool init = neighbor.getPhysicalNodeInfo().isInitialized();

  if (inside && !init) {  // -> delete
    cover_data_.erase(it_cover_data);

  } else if (inside && init) {  // -> update if different PhysicalNodeInfo, otherwise nothing
    if (it_cover_data->second.getPhysicalNodeInfo() != neighbor.getPhysicalNodeInfo()) {
      // update
      it_cover_data->second.setPhysicalNodeInfo(neighbor.getPhysicalNodeInfo());
    }

  } else if (!inside && init) {  // -> insert
    DistributedData distr_data(neighbor.getPhysicalNodeInfo());
    cover_data_[neighbor.getLogicalNodeInfo()] = distr_data;
    requestAttributeInformation(neighbor.getLogicalNodeInfo(), distr_data);
  }
}

void DSNHandler::onNodeInfoChangeNotification(
    [[maybe_unused]] const minhton::NodeInfo &previous_node_info,
    [[maybe_unused]] const minhton::NodeInfo &new_node_info) {
  if (isActive() && !was_previously_active_) {
    // we became a dsn
    // need to pull our data, send inquiries to each existing
    for (auto &[peer, distr_data] : cover_data_) {
      requestAttributeInformation(peer, distr_data);
    }
  }
}

std::vector<std::tuple<NodeInfo, std::vector<NodeData::Key>>>
DSNHandler::getUndecidedNodesAndMissingKeys(FindQuery &query, bool all_information_present,
                                            const uint64_t &timestamp_now) {
  std::vector<std::tuple<NodeInfo, std::vector<NodeData::Key>>> undecided;

  for (auto &[peer, distr_data] : cover_data_) {
    FuzzyValue val = query.evaluate(distr_data, all_information_present, timestamp_now);
    if (val.isUndecided()) {
      NodeInfo undecided_node;
      undecided_node.setLogicalNodeInfo(peer);
      undecided_node.setPhysicalNodeInfo(distr_data.getPhysicalNodeInfo());

      std::vector<NodeData::Key> missing_keys =
          query.evaluateMissingAttributes(distr_data, timestamp_now);

      undecided.push_back({undecided_node, missing_keys});
    }
  }

  return undecided;
}

std::vector<NodeInfo> DSNHandler::getTrueNodes(FindQuery &query, const uint64_t &timestamp_now) {
  std::vector<NodeInfo> true_nodes;

  for (auto &[peer, distr_data] : cover_data_) {
    FuzzyValue val = query.evaluate(distr_data, true, timestamp_now);
    if (val.isTrue()) {
      NodeInfo true_node;
      true_node.setLogicalNodeInfo(peer);
      true_node.setPhysicalNodeInfo(distr_data.getPhysicalNodeInfo());

      true_nodes.push_back(true_node);
    }
  }

  return true_nodes;
}

void DSNHandler::updateInquiredOrSubscribedAttributeValues(
    const NodeInfo &inquired_or_updated_node,
    std::unordered_map<NodeData::Key, NodeData::ValueAndType> attribute_values_and_types,
    uint64_t update_timestamp) {
  auto inquired_distr_data_it = cover_data_.find(inquired_or_updated_node.getLogicalNodeInfo());
  if (inquired_distr_data_it == cover_data_.end()) {
    inquired_distr_data_it =
        extended_cover_data_.find(inquired_or_updated_node.getLogicalNodeInfo());
    if (inquired_distr_data_it == extended_cover_data_.end()) {
      // not in cover area nor extended cover area
      return;
    }
  }

  DistributedData &inquired_distr_data = inquired_distr_data_it->second;

  if (inquired_distr_data.getPhysicalNodeInfo() != inquired_or_updated_node.getPhysicalNodeInfo()) {
    if (!inquired_distr_data.getPhysicalNodeInfo().isInitialized() &&
        inquired_or_updated_node.getPhysicalNodeInfo().isInitialized()) {
      inquired_distr_data.setPhysicalNodeInfo(inquired_or_updated_node.getPhysicalNodeInfo());
    } else {
      throw std::logic_error("something has gone wrong");
    }
  }

  for (auto &[key, value_and_type] : attribute_values_and_types) {
    NodeData::Value value = std::get<0>(value_and_type);
    NodeData::ValueType type = std::get<1>(value_and_type);

    bool updated = inquired_distr_data.update(key, {value, update_timestamp, type});
    if (!updated) {
      bool inserted = inquired_distr_data.insert(key, {value, update_timestamp, type});
      if (!inserted) {
        throw std::logic_error("something has gone wrong");
      }
    }
  }
}

void DSNHandler::updateRemovedAttributes(const NodeInfo &inquired_node,
                                         std::vector<NodeData::Key> removed_keys) {
  auto inquired_distr_data_it = cover_data_.find(inquired_node.getLogicalNodeInfo());
  if (inquired_distr_data_it == cover_data_.end()) {
    // remove this later
    throw std::logic_error("something has gone wrong");
  }

  DistributedData &inquired_distr_data = inquired_distr_data_it->second;

  if (inquired_distr_data.getPhysicalNodeInfo() != inquired_node.getPhysicalNodeInfo()) {
    throw std::logic_error("something has gone wrong");
  }

  for (const auto &key : removed_keys) {
    inquired_distr_data.remove(key);
  }
}

std::unordered_map<LogicalNodeInfo, DistributedData, LogicalNodeInfoHasher>
DSNHandler::getCoverData() const {
  return this->cover_data_;
}

void DSNHandler::notifyAboutQueryRequest(const FindQuery &query, const uint64_t request_timestamp) {
  auto relevant_keys = query.getRelevantAttributes();

  for (auto const &key : relevant_keys) {
    auto it = request_timestamps_.find(key);
    if (it != request_timestamps_.end()) {
      it->second.push(request_timestamp);
      if (it->second.size() > this->timestamp_storage_limit_) {
        it->second.pop();
      }
    } else {
      std::queue<uint64_t> request_queue;
      request_queue.push(request_timestamp);
      request_timestamps_[key] = request_queue;
    }
  }
}

std::unordered_map<NodeInfo, std::vector<NodeData::Key>, NodeInfoHasher>
DSNHandler::getNodesAndKeysToSubscribe(uint64_t const &timestamp_now) {
  std::unordered_map<NodeInfo, std::vector<NodeData::Key>, NodeInfoHasher> sub;

  // TODO[epic=esearch] set proper thresholds for subscribing
  const uint8_t min_requests = 3;
  const double min_req_frequency = 0.01;  // whatever unit this is...

  for (auto const &[key, timestamps] : request_timestamps_) {
    if (timestamps.size() >= min_requests) {
      double req_frequency = DSNHandler::calculateFrequency(timestamps, timestamp_now);
      if (req_frequency >= min_req_frequency) {
        for (auto &[peer, distr_data] : cover_data_) {
          if (distr_data.getPhysicalNodeInfo().isInitialized()) {
            double update_frequency =
                DSNHandler::calculateFrequency(distr_data.getUpdateTimestamps(key), timestamp_now);

            if (update_frequency < req_frequency) {
              // NOTE[epic=esearch] maybe add delta for threshold

              NodeInfo node;
              node.setLogicalNodeInfo(peer);
              node.setPhysicalNodeInfo(distr_data.getPhysicalNodeInfo());

              // if not subscribed yet
              if (!distr_data.isKeySubscribed(key)) {
                auto sub_it = sub.find(node);
                if (sub_it != sub.end()) {
                  sub_it->second.push_back(key);
                } else {
                  sub[node] = {key};
                }
              }
            }
          }
        }
      }
    }
  }

  return sub;
}

std::unordered_map<NodeInfo, std::vector<NodeData::Key>, NodeInfoHasher>
DSNHandler::getNodesAndKeysToUnsubscribe(uint64_t const &timestamp_now) {
  std::vector<std::tuple<LogicalNodeInfo, DistributedData, NodeData::Key>> current_sub_tuples;
  std::vector<NodeData::Key> sub_keys;

  for (auto const &[peer, distr_data] : cover_data_) {
    if (distr_data.getPhysicalNodeInfo().isInitialized()) {
      auto sub_order_keys = distr_data.getSubscriptionOrderKeys();

      for (auto const &key : sub_order_keys) {
        current_sub_tuples.push_back({peer, distr_data, key});
        sub_keys.push_back(key);
      }
    }
  }

  if (current_sub_tuples.empty()) {
    return {};
  }

  // removing duplicates
  std::sort(sub_keys.begin(), sub_keys.end());
  sub_keys.erase(std::unique(sub_keys.begin(), sub_keys.end()), sub_keys.end());

  // calculating request frequency for each key in sub_keys
  std::unordered_map<NodeData::Key, double> request_frequencies;
  for (auto const &key : sub_keys) {
    auto timestamps = request_timestamps_[key];
    request_frequencies[key] = DSNHandler::calculateFrequency(timestamps, timestamp_now);
  }

  // calculating update frequency for each node-key tuple in current_sub_tuples
  // std::unordered_map<NodeData::Key, std::vector<std::tuple<NodeInfo, double>>>
  // update_frequencies;
  std::unordered_map<NodeData::Key, std::unordered_map<NodeInfo, double, NodeInfoHasher>>
      update_frequencies;
  for (auto &[peer, distr_data, key] : current_sub_tuples) {
    NodeInfo node;
    node.setLogicalNodeInfo(peer);
    node.setPhysicalNodeInfo(distr_data.getPhysicalNodeInfo());

    auto timestamps = distr_data.getUpdateTimestamps(key);
    double frequency = DSNHandler::calculateFrequency(timestamps, timestamp_now);

    if (update_frequencies.find(key) == update_frequencies.end()) {
      update_frequencies[key][node] = frequency;
    } else {
      update_frequencies[key] = {{node, frequency}};
    }
  }

  // comparing request and update frequencies and deciding which to unsubscribe
  std::unordered_map<NodeInfo, std::vector<NodeData::Key>, NodeInfoHasher> unsub;
  for (auto const &[key, request_freq] : request_frequencies) {
    auto update_it = update_frequencies.find(key);
    if (update_it == update_frequencies.end()) {
      throw std::logic_error("shouldnt happen");
    }

    for (auto const &[node, update_freq] : update_it->second) {
      if (update_freq > request_freq) {
        auto unsub_it = unsub.find(node);
        if (unsub_it != unsub.end()) {
          unsub_it->second.push_back(key);
        } else {
          unsub[node] = {key};
        }
      }
    }
  }

  return unsub;
}

double DSNHandler::calculateFrequency(std::queue<uint64_t> timestamps,
                                      uint64_t const &timestamp_now) {
  uint8_t requests = timestamps.size();
  if (requests <= 1) {
    return 0.0;
  }

  auto oldest_timestamp = timestamps.front();
  uint64_t period = timestamp_now - oldest_timestamp;

  // * 1000 because of ms
  double frequency = (requests * 1000.0) / (period * 1.0);
  return frequency;  // in Hz
}

void DSNHandler::setPlacedSubscriptionOrders(
    std::unordered_map<NodeInfo, std::vector<NodeData::Key>, NodeInfoHasher>
        subscription_orders_map) {
  for (const auto &[node, sub_keys] : subscription_orders_map) {
    auto it = cover_data_.find(node.getLogicalNodeInfo());

    if (it == cover_data_.end()) {
      throw std::logic_error("subscribed node is not in cover data");
    }

    for (auto const &key : sub_keys) {
      it->second.addSubscriptionOrderKey(key);
    }
  }
}

void DSNHandler::setPlacedUnsubscriptionOrders(
    std::unordered_map<NodeInfo, std::vector<NodeData::Key>, NodeInfoHasher>
        unsubscription_orders_map) {
  for (const auto &[node, unsub_keys] : unsubscription_orders_map) {
    auto it = cover_data_.find(node.getLogicalNodeInfo());

    if (it == cover_data_.end()) {
      throw std::logic_error("subscribed node is not in cover data");
    }

    for (auto const &key : unsub_keys) {
      it->second.removeSubscriptionOrderKey(key);
    }
  }
}

NodeData::Attributes DSNHandler::getNodeAttributes(const NodeInfo &node) {
  NodeData::Attributes attributes;

  auto it = cover_data_.find(node.getLogicalNodeInfo());
  if (it != cover_data_.end()) {
    for (auto &[key, value_timestamp_type] : it->second.getData()) {
      attributes.push_back({key, std::get<0>(value_timestamp_type)});
    }
  }

  return attributes;
}

void DSNHandler::requestAttributeInformation(LogicalNodeInfo peer,
                                             const DistributedData &distr_data) {
  if (distr_data.getPhysicalNodeInfo().isInitialized()) {
    NodeInfo request_node;
    request_node.setLogicalNodeInfo(peer);
    request_node.setPhysicalNodeInfo(distr_data.getPhysicalNodeInfo());

    request_attribute_inquiry_callback_(request_node);
  }
}

}  // namespace minhton
