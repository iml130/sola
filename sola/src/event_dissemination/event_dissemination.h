// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#ifndef SOLA_EVENT_DISSEMINATION_H_
#define SOLA_EVENT_DISSEMINATION_H_

#include <string>
#include <vector>

namespace sola {
template <typename LoggerT> class EventDissemination {
public:
  virtual ~EventDissemination() = default;
  virtual void publish(const std::string &topic, const std::string &message) = 0;
  virtual void subscribe(const std::string &topic, std::vector<LoggerT> logger) = 0;
  virtual void unsubscribe(const std::string &topic) = 0;

  virtual void stop() = 0;
  virtual bool canStop() const = 0;
};
}  // namespace sola

#endif
