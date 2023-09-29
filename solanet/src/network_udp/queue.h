// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#ifndef SOLANET_NETWORK_UDP_QUEUE_H_
#define SOLANET_NETWORK_UDP_QUEUE_H_

#include <condition_variable>
#include <mutex>
#include <queue>

namespace solanet {
/**
 * Thread-safe queue with blocking pop
 */
template <typename T> class Queue {
public:
  ~Queue() {
    cv_queue_.notify_all();  // Unblock all waiting
  }

  /**
   * Push message to queue
   * @param value new message
   */
  void push(const T &value) {
    {
      std::scoped_lock lock(queue_mutex_);
      queue_.push(value);
    }

    // Notify a thread waiting on pop() that a new message is available
    cv_queue_.notify_one();
  }

  /**
   * Unblock blocking pop() call.
   * Every call to pop() after calling stop() will directly return an empty message.
   */
  void stop() {
    running_ = false;
    cv_queue_.notify_all();  // Unblock all waiting
  }

  /**
   * Pops and returns the first element in the queue. Blocks as long the queue is empty or
   * stop/destructor was called. If stop/destructor was called an empty message will be returned
   * @return first element in the queue or empty message if queue was stopped.
   */
  T pop() {
    std::unique_lock<std::mutex> lk(queue_mutex_);
    if (queue_.empty()) cv_queue_.wait(lk, [this] { return !running_ || !queue_.empty(); });

    if (!running_) return {};  // Return empty message if we should stop

    // Get first entry, pop it and return
    T item = queue_.front();
    queue_.pop();
    return item;
  }

private:
  std::queue<T> queue_;
  std::mutex queue_mutex_;
  bool running_ = true;
  std::condition_variable cv_queue_;
};
}  // namespace solanet

#endif  // SOLANET_NETWORK_UDP_QUEUE_H_
