// Copyright 2023 The SOLA authors
//
// This file is part of DAISI.
//
// DAISI is free software: you can redistribute it and/or modify it under the terms of the GNU
// General Public License as published by the Free Software Foundation; version 2.
//
// DAISI is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even
// the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
// Public License for more details.
//
// You should have received a copy of the GNU General Public License along with DAISI. If not, see
// <https://www.gnu.org/licenses/>.
//
// SPDX-License-Identifier: GPL-2.0-only

#ifndef DAISI_MINHTON_NS3_MINHTON_SCENARIOFILE_H_
#define DAISI_MINHTON_NS3_MINHTON_SCENARIOFILE_H_

#include <string>
#include <variant>

#include "manager/general_scenariofile.h"
#include "minhton/utils/algorithm_types_container.h"
#include "minhton/utils/timeout_lengths_container.h"
#include "peer_discovery_attributes.h"
#include "peer_discovery_requests.h"
#include "scenario_steps.h"

namespace daisi::minhton_ns3 {

struct Timeouts {
  // From minhton::TimeoutLengthsContainer
  // Intentionally not reusing the struct to avoid
  // forgetting about adding new members to this parser method
  uint64_t bootstrap_response;
  uint64_t join_response;
  uint64_t join_accept_ack_response;
  uint64_t replacement_offer_response;
  uint64_t replacement_ack_response;
  uint64_t dsn_aggregation;
  uint64_t inquiry_aggregation;

  void parse(YAML::Node node);

  /// @brief Convert this struct to a TimeoutLengthsContainer
  minhton::TimeoutLengthsContainer convert() const {
    minhton::TimeoutLengthsContainer cont;
    cont.bootstrap_response = bootstrap_response;
    cont.join_response = join_response;
    cont.join_accept_ack_response = join_accept_ack_response;
    cont.replacement_offer_response = replacement_offer_response;
    cont.replacement_ack_response = replacement_ack_response;
    cont.dsn_aggregation = dsn_aggregation;
    cont.inquiry_aggregation = inquiry_aggregation;
    return cont;
  }
};

struct Algorithms {
  std::string join;
  std::string leave;
  std::string search_exact;
  std::string response;
  std::string bootstrap;

  void parse(YAML::Node node);

  /// @brief Convert this struct to a AlgorithmTypesContainer
  minhton::AlgorithmTypesContainer convert() const {
    minhton::AlgorithmTypesContainer cont;

    std::unordered_map<std::string, minhton::JoinAlgorithms> string_to_join_algorithms_type{
        {"minhton", minhton::JoinAlgorithms::kJoinMinhton},
    };

    std::unordered_map<std::string, minhton::LeaveAlgorithms> string_to_leave_algorithms_type{
        {"minhton", minhton::LeaveAlgorithms::kLeaveMinhton},
    };

    std::unordered_map<std::string, minhton::SearchExactAlgorithms>
        string_to_search_exact_algorithms_type{
            {"minhton", minhton::SearchExactAlgorithms::kSearchExactMinhton},
        };

    std::unordered_map<std::string, minhton::ResponseAlgorithms> string_to_response_algorithms_type{
        {"general", minhton::ResponseAlgorithms::kResponseGeneral},
    };

    std::unordered_map<std::string, minhton::BootstrapAlgorithms>
        string_to_bootstrap_algorithms_type{
            {"general", minhton::BootstrapAlgorithms::kBootstrapGeneral},
        };

    cont.bootstrap = string_to_bootstrap_algorithms_type.at(bootstrap);
    cont.join = string_to_join_algorithms_type.at(join);
    cont.leave = string_to_leave_algorithms_type.at(leave);
    cont.search_exact = string_to_search_exact_algorithms_type.at(search_exact);
    cont.response = string_to_response_algorithms_type.at(response);

    return cont;
  }
};

struct PeerDiscoveryEnvironment {
  std::vector<PeerDiscoveryAttribute> attributes;
  Requests requests;

  void parse(YAML::Node node);
};

struct MinhtonScenarioSequenceStep {
  std::variant<JoinOne, JoinMany, LeaveOne, LeaveMany, SearchMany, SearchAll, FailOne, FailMany,
               MixedExecution, ValidateLeave, FindQuery, Time, StaticBuild, RequestCountdown>
      step;

  void parse(YAML::Node node);
};

struct MinhtonScenariofile : public GeneralScenariofile {
  explicit MinhtonScenariofile(const std::string &path_to_file);

  uint16_t fanout;
  Timeouts timeouts;
  Algorithms algorithms;
  std::optional<PeerDiscoveryEnvironment> peer_discovery_environment;
  std::vector<MinhtonScenarioSequenceStep> scenario_sequence;
};
}  // namespace daisi::minhton_ns3
#endif
