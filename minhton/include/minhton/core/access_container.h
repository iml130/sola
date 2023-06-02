// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#ifndef MINHTON_CORE_ACCESS_CONTAINER_H_
#define MINHTON_CORE_ACCESS_CONTAINER_H_

#include <functional>
#include <memory>

#include "minhton/core/node_info.h"
#include "minhton/core/routing_information.h"
#include "minhton/message/inform_about_neighbors.h"
#include "minhton/message/message.h"
#include "minhton/message/se_types.h"
#include "minhton/message/types_all.h"
#include "minhton/utils/fsm.h"
#include "minhton/utils/procedure_info.h"
#include "minhton/utils/timeout_lengths_container.h"

namespace minhton {

struct AccessContainer {
  std::shared_ptr<RoutingInformation> routing_info;
  std::shared_ptr<ProcedureInfo> procedure_info;
  Logger logger;

  std::function<void(const MessageVariant &)> send;
  std::function<void(const MessageVariant &)> recv;
  std::function<uint32_t(const MessageVariant &)> send_multicast;

  std::function<void(TimeoutType)> set_timeout;
  std::function<void(TimeoutType)> cancel_timeout;
  std::function<FSMState()> get_fsm_state;
  std::function<void(FiniteStateMachine)> set_new_fsm;
  std::function<uint16_t(TimeoutType)> get_timeout_length;

  std::function<void(const minhton::MessageInformAboutNeighbors &)> continue_accept_child_procedure;
  std::function<void(minhton::NodeInfo, std::shared_ptr<MessageSEVariant> query)>
      perform_search_exact;

  std::function<void(uint32_t number, std::function<void()> cb)> wait_for_acks;

  std::function<void(const minhton::NodeInfo, std::vector<minhton::NodeInfo>, bool)> replace_myself;

  std::function<uint64_t()> get_timestamp;

  bool node_locked = false;
};

}  // namespace minhton

#endif
