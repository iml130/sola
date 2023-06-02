// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#ifndef MINHTON_MESSAGE_MESSAGE_LOGGING_H_
#define MINHTON_MESSAGE_MESSAGE_LOGGING_H_

#include <cstdint>
#include <string>

struct MessageLoggingAdditionalInfo {
  std::string primary_other_uuid;
  std::string secondary_other_uuid;
  std::string content;
};

template <class Archive> void serialize(Archive &archive, MessageLoggingAdditionalInfo &m) {
  archive(m.primary_other_uuid, m.secondary_other_uuid, m.content);
}

struct MessageLoggingInfo {
  int cmd_type = -1;
  int mode = -1;
  uint64_t event_id = 0;
  uint64_t ref_event_id = 0;
  std::string sender_uuid;
  std::string target_uuid;
  MessageLoggingAdditionalInfo additional_info = {};
};

#endif
