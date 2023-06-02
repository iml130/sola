// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#include "minhton/core/watchdog.h"

#include <event2/event.h>
#include <event2/thread.h>

#include <atomic>
#include <cassert>
#include <stdexcept>
#include <thread>
#include <vector>

namespace minhton::core {
// Forward declaration
static void execute(evutil_socket_t socket, short ev_flags, void *entry);
static int addToList(const struct event_base *base, const struct event *event, void *entry);

/**
 * Wrapper for libevent event
 */
class Event {
public:
  /**
   *
   * @param function function to execute
   * @param base libevent base
   */
  Event(std::function<void()> function, event_base *base)
      : fct(std::move(function)), event(event_new(base, -1, 0, execute, this)) {}

  ~Event() { event_free(event); }

  Event(const Event &) = delete;
  Event &operator=(const Event &) = delete;
  Event(const Event &&) = delete;
  Event &operator=(const Event &&) = delete;

  std::function<void()> fct;
  struct event *event;
};

class WatchDog::Impl {
public:
  Impl() {
    evthread_use_pthreads();
    base_ = event_base_new();
    runner_ = std::thread(&WatchDog::Impl::run, this);
  }

  ~Impl() {
    [[maybe_unused]] int res = event_base_loopexit(base_, nullptr);
    assert(res == 0);
    if (runner_.joinable()) runner_.join();

    // delete all pending events
    std::vector<const struct event *> events;
    res = event_base_foreach_event(base_, addToList, &events);
    assert(res == 0);
    for (size_t i = 1; i < events.size(); i++) {
      auto event_arg =
          reinterpret_cast<Event *>(  // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
              event_get_callback_arg(events[i]));
      delete event_arg;  // NOLINT(cppcoreguidelines-owning-memory)
    }
    event_base_free(base_);
  }

  Impl(const Impl &) = delete;
  Impl &operator=(const Impl &) = delete;
  Impl(const Impl &&) = delete;
  Impl &operator=(const Impl &&) = delete;

  void addJob(std::function<void()> function, uint32_t miliseconds,
              [[maybe_unused]] TimeoutType &timeout_type) {
    if (running_) {
      struct timeval tv {};
      tv.tv_sec = miliseconds /
                  1000;  // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
      tv.tv_usec =
          1000 *  // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
          (miliseconds %
           1000);  // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
      auto event = new Event(std::move(function), base_);
      event_add(event->event, &tv);
    }
  }

  void cancelJob([[maybe_unused]] const minhton::TimeoutType &timeout_type) {
    // TODO Implement
  }

private:
  struct event_base *base_;
  std::thread runner_;
  std::atomic_bool running_ = true;

  void run() {
    if (base_ == nullptr) {
      throw std::runtime_error("base not initialized");
    }
    event_base_loop(base_, EVLOOP_NO_EXIT_ON_EMPTY);
  }
};

static void execute([[maybe_unused]] evutil_socket_t socket, [[maybe_unused]] short ev_flags,
                    void *entry) {
  auto event =
      reinterpret_cast<Event *>(entry);  // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
  event->fct();
  delete event;  // NOLINT(cppcoreguidelines-owning-memory)
}

static int addToList([[maybe_unused]] const struct event_base *base, const struct event *event,
                     void *entry) {
  auto events = reinterpret_cast<  // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
      std::vector<const struct event *> *>(entry);
  events->push_back(event);
  return 0;
}

WatchDog::WatchDog() : pimpl_(std::make_unique<Impl>()) {}

WatchDog::~WatchDog() = default;

void WatchDog::addJob(std::function<void()> function, uint32_t milliseconds,
                      TimeoutType &timeout_type) {
  pimpl_->addJob(function, milliseconds, timeout_type);
}

void WatchDog::cancelJob(const TimeoutType &timeout_type) { pimpl_->cancelJob(timeout_type); }

}  // namespace minhton::core
