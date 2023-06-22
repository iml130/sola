// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#ifndef MINHTON_MESSAGE_MESSAGE_H_
#define MINHTON_MESSAGE_MESSAGE_H_

#include <cstdint>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <variant>

#include "minhton/core/node_info.h"
#include "minhton/message/message_header.h"
#include "minhton/message/message_logging.h"
#include "minhton/message/types.h"

namespace minhton {

///
/// Representation of the minimal set of properties for a MinhtonMessage. Each MinhtonMessage
/// consists of a MinhtonMessageHeader which is inside a specific message type next to its payload.
///
template <typename T> class MinhtonMessage {
public:
  MinhtonMessage();

  /// Helper method to access the header
  /// \returns the header of the message
  MinhtonMessageHeader getHeader() const;

  /// Helper method to access the sender from the header
  /// equivalent to header_.getSender()
  /// \returns the NodeInfo of the sender
  NodeInfo getSender() const;

  /// Helper method to access the sender from the header
  /// equivalent to header_.getTarget()
  /// \returns the NodeInfo of the target
  NodeInfo getTarget() const;

  /// Helper method to change the target. Only to be used for Search Exact or Entity Search, where
  /// the query target needs to be adjusted
  /// \param new_target New Target
  void setTarget(NodeInfo new_target);

  T getFullMessage() const;

private:
  friend T;

  /// Validates the message and checks if all parameters are set correctly. Throws an
  /// InvalidMessageException in case of an error.
  void validate() const;

  bool validateHeader(const MinhtonMessageHeader &header) const;
};

template <typename T> MinhtonMessage<T>::MinhtonMessage() {}

template <typename T> MinhtonMessageHeader MinhtonMessage<T>::getHeader() const {
  return static_cast<const T *>(this)->header_;
}

template <typename T> NodeInfo MinhtonMessage<T>::getSender() const {
  return static_cast<const T *>(this)->header_.getSender();
}

template <typename T> NodeInfo MinhtonMessage<T>::getTarget() const {
  return static_cast<const T *>(this)->header_.getTarget();
}

template <typename T> T MinhtonMessage<T>::getFullMessage() const {
  return *static_cast<const T *>(this);
}

template <typename T>
bool MinhtonMessage<T>::validateHeader(const MinhtonMessageHeader &header) const {
  return header.getSender().getNetworkInfo().isInitialized() &&
         (header.getTarget().getNetworkInfo().isInitialized() ||
          header.getTarget().getLogicalNodeInfo().isInitialized());
}

template <typename T> void MinhtonMessage<T>::validate() const {
  if (!(validateHeader(static_cast<const T *>(this)->header_))) {
    throw std::invalid_argument("Invalid MINHTON message header");
  }
  if (!(static_cast<const T *>(this)->validateImpl())) {
    throw std::invalid_argument("Invalid MINHTON message payload");
  }
}

template <typename T> void MinhtonMessage<T>::setTarget(NodeInfo new_target) {
  static_cast<T *>(this)->header_.setTarget(new_target);
  validate();
}

}  // namespace minhton

#endif
