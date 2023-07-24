// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#ifndef NATTER_NATTER_H_
#define NATTER_NATTER_H_

#include <functional>
#include <memory>
#include <string>

#include "message.h"
#include "network_info_ipv4.h"
#include "solanet/uuid.h"

namespace natter {

using MsgReceiveFct = std::function<void(const Message &)>;
using MsgMissingFct = std::function<void(const std::string &)>;

/**
 * natter instance
 */
template <class T, class NodeInfoT> class Natter {
public:
  Natter() = default;
  using NodeInfo = NodeInfoT;

  // Disallow copy & move
  Natter(const Natter &) = delete;
  Natter &operator=(const Natter &other) = delete;
  Natter(Natter &&) = delete;
  Natter &operator=(Natter &&) = delete;
  ~Natter() = default;

  /**
   * Publish message to a topic via gossip
   * @param topic Topic to publish to
   * @param msg_content content to publish. Might be in any form. std::string acts as a container
   * only.
   * @return the message id
   */
  solanet::UUID publish(const std::string &topic, const std::string &msg_content) {
    return static_cast<T *>(this)->publishImpl(topic, msg_content);
  }

  ///////////////////////////
  ///// PEER MANAGEMENT /////
  ///////////////////////////

  /**
   * Add peer to the set which natter will use for message dissemination
   * @param topic Topic for which this node should be used
   * @param info Information about other peer (specific for selected dissemination algorithm)
   * @return true if peer was added successfully
   */
  bool addPeer(const std::string &topic, const NodeInfo &info) {
    return static_cast<T *>(this)->addPeerImpl(topic, info);
  }

  /**
   * Subscribe \p topic so natter accept new peers on that topic
   * @param topic topic name
   */
  void subscribeTopic(const std::string &topic, const NodeInfo &info) {
    static_cast<T *>(this)->subscribeTopicImpl(topic, info);
  }

  /**
   * Checks if this natter instance is subscribed to topic
   * @param topic
   * @return
   */
  bool isSubscribedToTopic(const std::string &topic) const {
    return static_cast<const T *>(this)->isSubscribedToTopicImpl(topic);
  }

  /**
   * Remove \p topic. This will remove all added peers for that topic.
   * @param topic
   */
  void unsubscribeTopic(const std::string &topic) {
    static_cast<T *>(this)->unsubscribeTopicImpl(topic);
  }

  /**
   * Remove peer from the set which natter will use to gossip with
   * @param topic Topic to unsubscribe
   * @param uuid unique-id of the node to be removed
   * @return true if peer was removed successfully
   */
  bool removePeer(const std::string &topic, const std::string &ip, uint16_t port) {
    return static_cast<T *>(this)->removePeerImpl(topic, ip, port);
  }

  /**
   * Returns UUID of this natter node
   * @return node uuid
   */
  solanet::UUID getUUID() const { return static_cast<const T *>(this)->getUUIDImpl(); }

  /**
   * Returns the node info for a given topic of this natter instance
   * @param topic
   * @return
   */
  NodeInfo getOwnNodeInfo(const std::string &topic) const {
    return static_cast<const T *>(this)->getOwnNodeInfoImpl(topic);
  }

  /**
   * Returns IPv4 and port of this natter instance
   * @return ip
   */
  NetworkInfoIPv4 getNetworkInfo() const {
    return static_cast<const T *>(this)->getNetworkInfoImpl();
  }
};

#define DECLARE_CRTP_METHODS                                                           \
  friend Natter;                                                                       \
  class Impl;                                                                          \
  std::unique_ptr<Impl> pimpl_;                                                        \
  solanet::UUID publishImpl(const std::string &topic, const std::string &msg_content); \
  bool addPeerImpl(const std::string &topic, const NodeInfo &info);                    \
  bool removePeerImpl(const std::string &topic, const std::string &ip, uint16_t port); \
  void subscribeTopicImpl(const std::string &topic, const NodeInfo &info);             \
  bool isSubscribedToTopicImpl(const std::string &topic) const;                        \
  void unsubscribeTopicImpl(const std::string &topic);                                 \
  NodeInfo getOwnNodeInfoImpl(const std::string &topic) const;                         \
  solanet::UUID getUUIDImpl() const;                                                   \
  NetworkInfoIPv4 getNetworkInfoImpl() const;

#define DEFINE_CRTP_METHODS(C)                                                                \
  C::C(MsgReceiveFct recv_callback, MsgMissingFct missing_callback)                           \
      : pimpl_(std::make_unique<Impl>(recv_callback, missing_callback)) {}                    \
  C::C(MsgReceiveFct recv_callback, MsgMissingFct missing_callback,                           \
       const std::vector<logging::LoggerPtr> &logger)                                         \
      : pimpl_(std::make_unique<Impl>(recv_callback, missing_callback, logger)) {}            \
  C::C(MsgReceiveFct recv_callback, MsgMissingFct missing_callback,                           \
       const std::vector<logging::LoggerPtr> &logger, solanet::UUID uuid)                     \
      : pimpl_(std::make_unique<Impl>(recv_callback, missing_callback, logger, uuid)) {}      \
  solanet::UUID C::publishImpl(const std::string &topic, const std::string &msg_content) {    \
    return pimpl_->publish(topic, msg_content);                                               \
  }                                                                                           \
  bool C::addPeerImpl(const std::string &topic, const C::NodeInfo &info) {                    \
    return pimpl_->addPeer(topic, info);                                                      \
  }                                                                                           \
  bool C::removePeerImpl(const std::string &topic, const std::string &ip, uint16_t port) {    \
    return pimpl_->removePeer(topic, ip, port);                                               \
  }                                                                                           \
  void C::subscribeTopicImpl(const std::string &topic, const C::NodeInfo &info) {             \
    pimpl_->subscribeTopic(topic, info);                                                      \
  }                                                                                           \
  bool C::isSubscribedToTopicImpl(const std::string &topic) const {                           \
    return pimpl_->isSubscribedToTopic(topic);                                                \
  }                                                                                           \
  void C::unsubscribeTopicImpl(const std::string &topic) { pimpl_->unsubscribeTopic(topic); } \
  C::NodeInfo C::getOwnNodeInfoImpl(const std::string &topic) const {                         \
    return pimpl_->getOwnNodeInfo(topic);                                                     \
  }                                                                                           \
  C::~C() = default;                                                                          \
  solanet::UUID C::getUUIDImpl() const { return pimpl_->getUUID(); }                          \
  NetworkInfoIPv4 C::getNetworkInfoImpl() const { return pimpl_->getNetworkInfo(); }

}  // namespace natter

#endif  // NATTER_NATTER_H_
