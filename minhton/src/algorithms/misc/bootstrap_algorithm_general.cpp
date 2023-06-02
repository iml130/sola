// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#include "minhton/algorithms/misc/bootstrap_algorithm_general.h"

#include "minhton/exception/algorithm_exception.h"
#include "minhton/logging/logging.h"
#include "minhton/message/types_all.h"

namespace minhton {

void BootstrapAlgorithmGeneral::process(const MessageVariant &msg) {
  std::visit(
      Overload{
          [this](const MessageBootstrapDiscover &message) { processBootstrapDiscover(message); },
          [this](const MessageBootstrapResponse &message) { processBootstrapResponse(message); },
          [](auto &message) {
            throw AlgorithmException("Wrong Algorithm Interface process called");
          }},
      msg);
}

void BootstrapAlgorithmGeneral::initiateJoin([[maybe_unused]] const NetworkInfo &network_info) {
  MinhtonMessageHeader header(getSelfNodeInfo(), NodeInfo());
  MessageBootstrapDiscover msg_bootstrap_discover(header);

  access_->set_timeout(TimeoutType::kBootstrapResponseTimeout);

  access_->send_multicast(msg_bootstrap_discover);
}

void BootstrapAlgorithmGeneral::processBootstrapDiscover(const MessageBootstrapDiscover &msg) {
  // Only respond if node is initialized
  if (!getSelfNodeInfo().isInitialized()) return;

  // Only respond if we are in the connected FSM state
  if (access_->get_fsm_state() != FSMState::kConnected) return;

  // Only handling requests from devices which are not ourselves
  if (msg.getSender() == getSelfNodeInfo()) return;

  LOG_INFO("Receive a join request from some other node");

  // TODO add strategy
  // e.g. only answer if we are a prio node

  MinhtonMessageHeader header(getSelfNodeInfo(), msg.getSender(), msg.getHeader().getEventId());
  MessageBootstrapResponse response(header, getSelfNodeInfo());
  send(response);
}

void BootstrapAlgorithmGeneral::processBootstrapResponse(const MessageBootstrapResponse &msg) {
  if (access_->get_fsm_state() == FSMState::kWaitForBootstrapResponse) {
    auto bootstrap_sender = msg.getSender();
    bootstrap_response_senders_.push_back(bootstrap_sender);
  }
}

void BootstrapAlgorithmGeneral::processBootstrapResponseTimeout() {
  if (!access_->procedure_info->hasKey(ProcedureKey::kJoinProcedure)) {
    // TODO add strategy to choose best of the response sender nodes
    auto bootstrap_responder = bootstrap_response_senders_[0];
    bootstrap_response_senders_.clear();

    access_->set_timeout(TimeoutType::kJoinAcceptResponseTimeout);

    MinhtonMessageHeader header(getSelfNodeInfo(), bootstrap_responder);
    MessageJoin msg(header, getSelfNodeInfo());
    send(msg);
  }
}

bool BootstrapAlgorithmGeneral::isBootstrapResponseValid() const {
  return !bootstrap_response_senders_.empty();
}

}  // namespace minhton
