// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#include "SOLA/event_dissemination_minhcast.h"
#include "sola_check.h"

namespace sola {

void EventDisseminationMinhcast::getResult(const std::string &topic,
                                           const std::function<void()> &on_result) {
  result_.at(topic).wait();

  using namespace std::chrono_literals;
  auto res = result_.at(topic).wait_for(0ms);
  SOLA_CHECK(res == std::future_status::ready, "Result future is not ready yet");

  on_result();
}

void EventDisseminationMinhcast::checkTopicJoin(const std::string &topic, bool should_exist) {
}  // No checking

}  // namespace sola
