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

#include "endpoint.h"
#include "event_dissemination/event_dissemination.h"
#include "message.h"
#include "service.h"
#include "solanet/network_udp/network_udp.h"
#include "storage/storage.h"
#include "uuid.h"

namespace sola {

template <typename StorageT, typename EventDisseminationT> class SOLA {
  static_assert(std::is_base_of<Storage, StorageT>());
  static_assert(
      std::is_base_of<EventDissemination<typename StorageT::Logger>, EventDisseminationT>());

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
       MessageReceiveFct receive_fct, TopicMessageReceiveFct topic_recv)
      : receive_fct_(receive_fct),
        network_(std::make_unique<solanet::Network>([&](const solanet::Message &msg) {
          receive_fct_({msg.getIp(), msg.getMessage()});
        })),
        storage_(std::make_shared<StorageT>(storage_config)),
        ed_(std::make_unique<EventDisseminationT>(topic_recv, storage_, network_->getIP(),
                                                  event_dissemination_config)) {}

  void stop() {
    storage_->stop();
    ed_->stop();
  }

  bool canStop() const { return storage_->canStop() && ed_->canStop(); }

  //***** CRUD operations for service
  void addService(Service service) {
    std::cout << "PUBLISH SERVICE" << std::endl;
    std::vector<Entry> entry;
    entry.push_back({"endpoint", network_->getIP() + ":" + std::to_string(network_->getPort())});

    for (auto [key, value] : service.key_values) {
      if (value.type() == typeid(std::string)) {
        entry.push_back({key, std::any_cast<std::string>(value)});
      } else if (value.type() == typeid(float)) {
        entry.push_back({key, std::any_cast<float>(value)});
      } else if (value.type() == typeid(int)) {
        entry.push_back({key, std::any_cast<int>(value)});
      } else {
        throw std::runtime_error("invalid any");
      }
    }

    storage_->insert(entry);
  }
  void updateService(UUID uuid, Service service);
  void removeService(UUID uuid){};

  std::future<minhton::FindResult> findService(Request r) { return storage_->find(r); }

  // Functions for negotation
  void sendData(const std::string &data, Endpoint endpoint) {
    network_->send({endpoint.ip, endpoint.port, data});
  }

  // event dissemination
  void subscribeTopic(const std::string &topic,
                      std::vector<typename StorageT::Logger> logger = {}) {
    ed_->subscribe(topic, logger);
  }
  void unsubscribeTopic(const std::string &topic) { ed_->unsubscribe(topic); }
  void publishMessage(const std::string &topic, const std::string &message) {
    ed_->publish(topic, message);
  }

  // returns ip/port for SOLA high-level communication
  std::string getConectionString() const {
    return network_->getIP() + ":" + std::to_string(network_->getPort());
  }
  std::string getIP() const { return network_->getIP(); }
  uint16_t getPort() const { return network_->getPort(); }

  bool isStorageRunning() { return storage_->isRunning(); }

private:
  MessageReceiveFct receive_fct_;
  std::unique_ptr<solanet::Network> network_;
  std::shared_ptr<StorageT> storage_;
  std::unique_ptr<EventDisseminationT> ed_;
};
}  // namespace sola

#endif  // SOLA_SOLA_H_
