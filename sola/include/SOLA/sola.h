// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#ifndef SOLA_SOLA_H_
#define SOLA_SOLA_H_

#include <any>
#include <cstdint>
#include <functional>
#include <future>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "SOLA/logger_interface.h"
#include "event_dissemination/event_dissemination.h"
#include "message.h"
#include "service.h"
#include "solanet/uuid.h"
#include "solanet/uuid_generator.h"
#include "storage/storage.h"

namespace sola {

template <typename StorageT, typename EventDisseminationT> class SOLA {
  static_assert(std::is_base_of<Storage, StorageT>());
  static_assert(std::is_base_of<EventDissemination, EventDisseminationT>());

public:
  /*! \brief Starts the SOLA instance
   *
   * @param storage_config configuration for storage
   * @param event_dissemination_config configuration for event dissemination
   * @param receive_fct function to be called when a new message is received
   * @param topic_recv function to be called when a new message on a topic arrives
   */
  SOLA(const typename StorageT::Config &storage_config,
       const typename EventDisseminationT::Config &event_dissemination_config,
       TopicMessageReceiveFct topic_recv, LoggerPtr logger)
      : storage_(std::make_shared<StorageT>(storage_config)),
        ed_(std::make_unique<EventDisseminationT>(
            [this, topic_recv](const TopicMessage &m) {
              logger_->logReceiveTopicMessage(m);
              topic_recv(m);
            },
            storage_, event_dissemination_config, logger)),
        logger_(std::move(logger)) {
    logger_->setApplicationUUID(solanet::uuidToString(uuid_));
  }

  void stop() {
    storage_->stop();
    ed_->stop();
  }

  bool canStop() const { return storage_->canStop() && ed_->canStop(); }

  //***** CRUD operations for service
  void addService(Service service) {
    std::cout << "PUBLISH SERVICE" << std::endl;
    std::vector<Entry> entry;

    for (auto [key, value] : service.key_values) {
      if (value.type() == typeid(std::string)) {
        entry.emplace_back(key, std::any_cast<std::string>(value));
      } else if (value.type() == typeid(float)) {
        entry.emplace_back(key, std::any_cast<float>(value));
      } else if (value.type() == typeid(int)) {
        entry.emplace_back(key, std::any_cast<int>(value));
      } else {
        throw std::runtime_error("invalid any");
      }
    }

    storage_->insert(entry);
  }

  std::future<minhton::FindResult> findService(Request r) { return storage_->find(r); }

  // event dissemination
  void subscribeTopic(const std::string &topic) {
    logger_->logSubscribeTopic(topic);
    ed_->subscribe(topic);
  }

  void unsubscribeTopic(const std::string &topic) {
    logger_->logUnsubscribeTopic(topic);
    ed_->unsubscribe(topic);
  }

  solanet::UUID publishMessage(const std::string &topic, const std::string &message) {
    sola::TopicMessage msg{topic, uuid_, message, solanet::generateUUID()};
    logger_->logPublishTopicMessage(msg);
    ed_->publish(msg);
    return msg.uuid;
  }

  bool isStorageRunning() const { return storage_->isRunning(); }

private:
  std::shared_ptr<StorageT> storage_;
  std::unique_ptr<EventDisseminationT> ed_;
  LoggerPtr logger_;
  solanet::UUID uuid_ = solanet::generateUUID();  /// UUID of this SOLA instance
};
}  // namespace sola

#endif  // SOLA_SOLA_H_
