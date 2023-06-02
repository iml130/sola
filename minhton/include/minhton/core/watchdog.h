// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#ifndef MINHTON_WATCHDOG_H_
#define MINHTON_WATCHDOG_H_

#include <functional>
#include <memory>

#include "minhton/core/constants.h"

namespace minhton::core {

/**
 * Helper class to execute functions after a given time asynchronous
 */
class WatchDog {
public:
  WatchDog();
  ~WatchDog();

  WatchDog(const WatchDog &) = delete;
  WatchDog &operator=(const WatchDog &) = delete;
  WatchDog(const WatchDog &&) = delete;
  WatchDog &operator=(const WatchDog &&) = delete;

  /**
   * Add new job to watchdog (only one job can exist at a time for now)
   * @param function function to execute
   * @param milliseconds milliseconds until \p function will be executed.
   */
  void addJob(std::function<void()> function, uint32_t milliseconds, TimeoutType &timeout_type);

  void cancelJob(const TimeoutType &timeout_type);

private:
  class Impl;
  std::unique_ptr<Impl> pimpl_;
};
}  // namespace minhton::core

#endif  // MINHTON_WATCHDOG_H_
