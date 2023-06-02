// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#ifndef MINHTON_UTILS_SERIALIZER_H_
#define MINHTON_UTILS_SERIALIZER_H_

#include <memory>

#include "minhton/message/message.h"
#include "minhton/message/types_all.h"

namespace minhton {
///
/// Interface defintion for a Serializer, based on the MessageVariant. Before sending a message from
/// a node to another node that data needs to be serialized. Before a node can process a received
/// packet, the data needs to be de-serialized. This two methods are taking care of die
/// de-/serialization of MessageVariants.
///
class ISerializer {
public:
  virtual ~ISerializer() = default;
  /// Serialize message
  /// \param msg Message to be serialized
  /// \returns Message as serialized string
  virtual std::string serialize(const minhton::MessageVariant &msg) = 0;

  /// Deserialize message
  /// \param input Message as string which should be deserialized
  /// \returns shared_ptr to the deserialized message
  virtual minhton::MessageVariant deserialize(const std::string &input) = 0;
};
}  // namespace minhton
#endif
