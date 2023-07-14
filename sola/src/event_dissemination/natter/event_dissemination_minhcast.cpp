// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#include "SOLA/event_dissemination_minhcast.h"

#include <algorithm>
#include <map>
#include <tuple>

namespace sola {
EventDisseminationMinhcast::EventDisseminationMinhcast(TopicMessageReceiveFct msgRecvFct,
                                                       std::shared_ptr<Storage> storage,
                                                       std::string ip, const Config &config)
    : minhcast_(std::make_unique<natter::minhcast::NatterMinhcast>(
          [=](const natter::Message &m) {
            msgRecvFct({m.topic, natter::uuidToString(m.sender_id), m.content, m.message_id});
          },
          [](const std::string & /*unused*/) {}, config.logger)),
      ip_(std::move(ip)),
      storage_(std::move(storage)) {}

void EventDisseminationMinhcast::publish(const std::string &topic, const std::string &message) {
  if (stopping_) throw std::runtime_error("already stopping!");
  minhcast_->publish(topic, message);
}

void EventDisseminationMinhcast::unsubscribe(const std::string &topic) {
  if (topic_trees_.find(topic) == topic_trees_.end())
    throw std::runtime_error("not part of topic!");

  topic_trees_.at(topic)->stop();

  minhcast_->unsubscribeTopic(topic);
}

void EventDisseminationMinhcast::subscribe(const std::string &topic,
                                           std::vector<minhton::Logger::LoggerPtr> logger) {
  if (stopping_) throw std::runtime_error("already stopping!");

  if (topic_trees_.find(topic) != topic_trees_.end())
    throw std::runtime_error("already part of topic");

  if (result_.find(topic) != result_.end()) {
    throw std::runtime_error("joining topic already in progress");
  }

  sola::Request req;
  req.all = false;
  req.permissive = true;
  req.request = "(HAS " + topic + ")";
  result_[topic] = storage_->find(req);
  minhton_loggers_.push_back(MinhtonTopicLogger{topic, logger});

  waitForResults(topic);
}

void EventDisseminationMinhcast::stop() {
  stopping_ = true;
  for (const auto &topic : topic_trees_) {
    unsubscribe(topic.first);
  }
}

bool EventDisseminationMinhcast::canStop() const {
  if (!stopping_) return false;
  bool stopped = true;
  for (auto &topic : topic_trees_) {
    minhton::State a = topic.second->getState();
    stopped &= a == minhton::State::kIdle;
  }
  return stopped;
}

void EventDisseminationMinhcast::waitForResults(const std::string &topic) {
  if (!result_.at(topic).valid()) throw std::runtime_error("invalid future");

  getResult(topic, [this, topic]() {
    auto it = std::find_if(minhton_loggers_.begin(), minhton_loggers_.end(),
                           [topic](const MinhtonTopicLogger &log) { return log.topic == topic; });
    if (it == minhton_loggers_.end()) {
      throw std::runtime_error("no logger found");
    }

    joinMinhton(result_.at(topic).get(), topic, it->logger);
    result_.erase(topic);
    minhton_loggers_.erase(it);
  });
}

void EventDisseminationMinhcast::joinMinhton(
    minhton::FindResult result, const std::string &topic,
    const std::vector<minhton::Logger::LoggerPtr> &logger) {
  if (stopping_) throw std::runtime_error("already stopping!");

  auto callback = [=](const minhton::ConnectionInfo &neighbor) {
    if (peers_added_natter_.find({neighbor.node.getAddress(), neighbor.node.getPort()}) ==
        peers_added_natter_.end()) {
      peers_added_natter_[{neighbor.node.getAddress(), neighbor.node.getPort()}] = 0;
    }

    peers_added_natter_.at({neighbor.node.getAddress(), neighbor.node.getPort()}) += 1;
    uint32_t number_current_added =
        peers_added_natter_.at({neighbor.node.getAddress(), neighbor.node.getPort()});
    if (!minhcast_->isSubscribedToTopic(topic)) {
      // Subscribe when receiving first neighbor update
      minhcast_->subscribeTopic(topic, {{neighbor.ourself.getLevel(), neighbor.ourself.getNumber(),
                                         neighbor.ourself.getFanout()},
                                        {neighbor.ourself.getPhysicalNodeInfo().getAddress(),
                                         neighbor.ourself.getPhysicalNodeInfo().getPort()}});
    }

    if (number_current_added == 1) {
      // New peer
      minhcast_->addPeer(
          topic, {{neighbor.node.getLevel(), neighbor.node.getNumber(), neighbor.node.getFanout()},
                  {neighbor.node.getAddress(), 2002},
                  neighbor.node.getLogicalNodeInfo().getRawUuid()});
    }

    if (!neighbor.ip_old.empty()) {
      peers_added_natter_.at({neighbor.ip_old, neighbor.port_old}) -= 1;
      uint32_t number_old_added = peers_added_natter_.at({neighbor.ip_old, neighbor.port_old});
      if (number_old_added == 0) {
        minhcast_->removePeer(topic, neighbor.ip_old, 2002);
        peers_added_natter_.erase({neighbor.ip_old, neighbor.port_old});
      }
    }
  };
  minhton::ConfigNode config;

  minhton::AlgorithmTypesContainer algorithms{};
  algorithms.bootstrap = minhton::kBootstrapGeneral;
  algorithms.join = minhton::kJoinMinhton;
  algorithms.leave = minhton::kLeaveMinhton;
  algorithms.response = minhton::kResponseGeneral;
  algorithms.search_exact = minhton::kSearchExactMinhton;
  config.setAlgorithmTypesContainer(algorithms);

  minhton::TimeoutLengthsContainer timeouts{};
  timeouts.bootstrap_response = 500;
  timeouts.join_response = 500;
  timeouts.join_accept_ack_response = 500;
  timeouts.replacement_offer_response = 500;
  timeouts.replacement_ack_response = 500;
  timeouts.dsn_aggregation = 1000;
  timeouts.inquiry_aggregation = 500;
  config.setTimeoutLengthsContainer(timeouts);

  config.setFanout(2);
  config.setOwnIP(ip_);

  std::string connection_string;

  if (!result.empty()) {
    auto values = result[0];
    for (auto &val : values) {
      std::string key = std::get<0>(val);
      if (key == topic) {
        auto variant = std::get<1>(val);
        connection_string = std::get<std::string>(variant);
      }
    }

    assert(!connection_string.empty());

    minhton::JoinInfo info;
    info.ip = connection_string.substr(0, connection_string.find(':'));
    info.port = std::stoi(
        connection_string.substr(connection_string.find(':') + 1, connection_string.length()));
    info.join_mode = minhton::JoinInfo::kIp;
    config.setJoinInfo(info);
    config.setIsRoot(false);
    checkTopicJoin(topic, true);
  } else {
    minhton::JoinInfo info;
    info.join_mode = minhton::JoinInfo::kNone;
    config.setIsRoot(true);

    checkTopicJoin(topic, false);
  }

  config.setLogger(logger);

  topic_trees_[topic] = std::make_unique<minhton::Minhton>(callback, config);

  if (topic_trees_[topic]->getState() == minhton::State::kConnected) {
    // Directly connected (root)
    // FIXME not thread/network safe
    auto node_info = topic_trees_[topic]->getNode().getNodeInfo();
    minhcast_->subscribeTopic(topic,
                              {{node_info.getLevel(), node_info.getNumber(), node_info.getFanout()},
                               {node_info.getAddress(), node_info.getPort()}});
  }

  if (result.empty()) {
    // Add to storage to be found later on
    minhton::NodeInfo info = topic_trees_[topic]->getNode().getNodeInfo();
    std::string address = info.getAddress() + ":" + std::to_string(info.getPort());
    storage_->insert({{topic, address}});
  }
}
}  // namespace sola
