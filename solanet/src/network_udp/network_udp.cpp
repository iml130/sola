// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#include "solanet/network_udp/network_udp.h"

#include <ifaddrs.h>

#include <array>
#include <thread>
#include <utility>

#ifndef CPPCHECK_IGNORE
#include "asio.hpp"
#endif

#include "queue.h"
#include "solanet/network_udp/message.h"

#define MAX_DATAGRAM_SIZE 65535

namespace solanet {

class Network::Impl {
public:
  Impl(const std::string &ip, std::function<void(const Message &)> callback);

  ~Impl();

  void send(const Message &msg);

  uint16_t getPort() const;
  std::string getIP() const;

private:
  void readFromSocket();

  void receiveFunction();

  void networkFunction();

  void sendFunction();

  // Some basic heuristics to find own IP, only picking the first one
  static std::string readIPFromInterfaces();

  std::thread receiver_thread_, sender_thread_, network_thread_;
  Queue<Message> receiving_queue_, sending_queue_;
  asio::io_service io_service_;
  std::string ip_;
  asio::ip::udp::socket receiver_socket_;
  asio::ip::udp::socket sender_socket_;
  std::function<void(const Message &)> receive_callback_;
  std::string buffer_;
  asio::ip::udp::endpoint endpoint_;
};

void Network::Impl::receiveFunction() {
  while (true) {
    Message message = receiving_queue_.pop();
    if (message.isEmpty()) {
      break;
    }
    receive_callback_(message);  // Callback to application
  }
}

void Network::Impl::sendFunction() {
  while (true) {
    Message message = sending_queue_.pop();

    // Quit loop if message is empty (most likely queue_.stop() was called)
    if (message.isEmpty()) break;

    // Send message
    asio::ip::udp::endpoint receiver(asio::ip::address::from_string(message.getIp()),
                                     message.getPort());

    std::size_t transferred = sender_socket_.send_to(
        asio::buffer(message.getMessage().data(), message.getMessage().size()), receiver);
    if (message.getMessage().size() != transferred) {
      throw std::runtime_error("Failed on message send!");
    }
  }
}

void Network::Impl::networkFunction() { io_service_.run(); }

void Network::Impl::readFromSocket() {
  receiver_socket_.async_receive_from(asio::buffer(buffer_.data(), buffer_.size()), endpoint_,
                                      [&](asio::error_code ec, std::size_t size) {
                                        if (ec)
                                          throw std::runtime_error("Failed to receive message");

                                        Message msg(endpoint_.address().to_string(),
                                                    endpoint_.port(), buffer_.substr(0, size));
                                        receiving_queue_.push(msg);
                                        readFromSocket();
                                      });
}

Network::Impl::Impl(const std::string &ip, std::function<void(const Message &)> callback)
    : ip_(ip.empty() ? readIPFromInterfaces() : ip),
      receiver_socket_(io_service_, {asio::ip::address::from_string(ip_), 0}),
      sender_socket_(io_service_),
      receive_callback_(std::move(callback)) {
  sender_socket_.open(asio::ip::udp::v4());
  buffer_.resize(MAX_DATAGRAM_SIZE);

  // Start threads
  receiver_thread_ = std::thread(&Network::Impl::receiveFunction, this);
  sender_thread_ = std::thread(&Network::Impl::sendFunction, this);

  readFromSocket();
  network_thread_ = std::thread(&Network::Impl::networkFunction, this);
}

Network::Impl::~Impl() {
  // Stop all
  io_service_.stop();
  receiving_queue_.stop();
  sending_queue_.stop();

  // Wait till all threads terminate
  receiver_thread_.join();
  sender_thread_.join();
  network_thread_.join();
}

void Network::Impl::send(const Message &msg) {
  if (msg.getMessage().size() > MAX_DATAGRAM_SIZE)
    throw std::runtime_error("Cannot send message. Message too large!");

  sending_queue_.push(msg);
}

std::string Network::Impl::readIPFromInterfaces() {
  std::string ip_addr;
  struct ifaddrs *addresses = nullptr;
  if (getifaddrs(&addresses) != 0) throw std::runtime_error("failed to fetch");

  for (auto address = addresses; address != nullptr && ip_addr.empty();
       address = address->ifa_next) {
    if (address->ifa_addr == nullptr || strcmp(address->ifa_name, "lo") == 0 ||
        address->ifa_addr->sa_family != AF_INET) {
      // Ignore loopback and non-internet addresses
      continue;
    }
    std::array<char, NI_MAXHOST> ip;
    int res = getnameinfo(address->ifa_addr, sizeof(struct sockaddr_in), ip.data(), NI_MAXHOST,
                          nullptr, 0, NI_NUMERICHOST);
    if (res != 0) throw std::runtime_error("unable to get IP address");
    ip_addr = ip.data();
  }
  freeifaddrs(addresses);

  if (ip_addr.empty()) throw std::runtime_error("unable to find IP address");

  return ip_addr;
}

uint16_t Network::Impl::getPort() const { return receiver_socket_.local_endpoint().port(); }

std::string Network::Impl::getIP() const { return ip_; }

////////////////////////////////////
////// PIMP IMPLEMENTATION /////////
////////////////////////////////////

Network::Network(const std::function<void(const Message &)> &callback)
    : pimpl_(std::make_unique<Impl>("", callback)) {}

Network::Network(const std::string &ip, const std::function<void(const Message &)> &callback)
    : pimpl_(std::make_unique<Impl>(ip, callback)) {}

Network::~Network() = default;  // Declared as default here (and not in header) because otherwise
                                // class Impl has incomplete type

void solanet::Network::send(const Message &msg) { pimpl_->send(msg); }

uint16_t solanet::Network::getPort() const { return pimpl_->getPort(); }

std::string solanet::Network::getIP() const { return pimpl_->getIP(); }

}  // namespace solanet
