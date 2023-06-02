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

#include "minhton_scenariofile_helper.h"

namespace daisi::minhton_ns3::helper {

minhton::AlgorithmTypesContainer toAlgorithmContainer(
    const std::unordered_map<std::string, std::string> &algorithm_map) {
  minhton::AlgorithmTypesContainer container;

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

  std::unordered_map<std::string, minhton::BootstrapAlgorithms> string_to_bootstrap_algorithms_type{
      {"general", minhton::BootstrapAlgorithms::kBootstrapGeneral},
  };

  const std::string bootstrap = algorithm_map.at("bootstrap");
  const std::string join = algorithm_map.at("join");
  const std::string leave = algorithm_map.at("leave");
  const std::string search_exact = algorithm_map.at("search_exact");
  const std::string response = algorithm_map.at("response");

  container.bootstrap = string_to_bootstrap_algorithms_type.at(bootstrap);
  container.join = string_to_join_algorithms_type.at(join);
  container.leave = string_to_leave_algorithms_type.at(leave);
  container.search_exact = string_to_search_exact_algorithms_type.at(search_exact);
  container.response = string_to_response_algorithms_type.at(response);

  return container;
}

minhton::TimeoutLengthsContainer toTimeoutLengthsContainer(
    const std::unordered_map<std::string, uint64_t> &timeout_map) {
  minhton::TimeoutLengthsContainer container;

  container.bootstrap_response = timeout_map.at("bootstrap_response");
  container.join_response = timeout_map.at("join_response");
  container.join_accept_ack_response = timeout_map.at("join_accept_ack_response");
  container.replacement_offer_response = timeout_map.at("replacement_offer_response");
  container.replacement_ack_response = timeout_map.at("replacement_ack_response");
  container.dsn_aggregation = timeout_map.at("dsn_aggregation");
  container.inquiry_aggregation = timeout_map.at("inquiry_aggregation");

  return container;
}

}  // namespace daisi::minhton_ns3::helper
