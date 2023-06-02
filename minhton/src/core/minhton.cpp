// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#include "minhton/core/minhton.h"

#include "minhton/utils/config_reader.h"

namespace minhton {

Minhton::Minhton(NeighborCallbackFct fct, const ConfigNode &config) : node_(config, fct) {}

void Minhton::insert(const std::vector<Entry> &data) { node_.insert(data); }

void Minhton::update(const std::vector<Entry> &data) { node_.update(data); }

void Minhton::remove(const std::vector<std::string> &keys) { node_.remove(keys); }

std::future<FindResult> Minhton::find(const FindQuery &query) { return node_.find(query); }

void Minhton::stop() { node_.stop(); }

State Minhton::getState() {
  auto fsm_state = node_.getFsmState();
  switch (fsm_state) {
    case kIdle:
      return State::kIdle;
    case kWaitForBootstrapResponse:
    case kWaitForJoinAccept:
      return State::kStarted;
    case kConnected:
    case kConnectedAcceptingChild:
    case kConnectedReplacing:
    case kWaitForReplacementOffer:
    case kConnectedWaitingParentResponse:
    case kSignOffFromInlevelNeighbors:
    case kConnectedWaitingParentResponseDirectLeaveWoReplacement:
    case kSignOffFromInlevelNeighborsDirectLeaveWoReplacement:
      return State::kConnected;
    case kJoinFailed:
    case kErrorState:
      return State::kError;
    default:
      throw std::runtime_error("unknown fsm state");
  }
}

}  // namespace minhton
