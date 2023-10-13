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

#ifndef DAISI_CPPS_COMMON_SCENARIOFILE_CPPS_SCENARIOFILE_H_
#define DAISI_CPPS_COMMON_SCENARIOFILE_CPPS_SCENARIOFILE_H_

#include <numeric>
#include <string>

#include "amr_description_scenario.h"
#include "cpps/amr/amr_topology.h"
#include "cpps/logical/algorithms/algorithm_config.h"
#include "manager/general_scenariofile.h"
#include "material_flow_scenario.h"
#include "spawn_info_scenario.h"

namespace daisi::cpps {

struct TopologyScenario {
  double width = 0.0;
  double height = 0.0;
  double depth = 0.0;

  void parse(YAML::Node node) {
    SERIALIZE_VAR(width);
    SERIALIZE_VAR(height);
    SERIALIZE_VAR(depth);
  }

  Topology getTopology() const { return Topology(util::Dimensions(width, height, depth)); }
};

struct AlgorithmScenario {
  std::string assignment_strategy;

  const std::unordered_map<std::string, logical::AlgorithmType>
      assignment_strategy_to_initiator_algorithm_type = {
          {"iterated_auction", logical::AlgorithmType::kIteratedAuctionAssignmentInitiator},
          {"round_robin", logical::AlgorithmType::kRoundRobinInitiator},
  };

  const std::unordered_map<std::string, logical::AlgorithmType>
      assignment_strategy_to_participant_algorithm_type = {
          {"iterated_auction", logical::AlgorithmType::kIteratedAuctionAssignmentParticipant},
          {"round_robin", logical::AlgorithmType::kRoundRobinParticipant},
  };

  void parse(YAML::Node node) { SERIALIZE_VAR(assignment_strategy); }

  logical::AlgorithmConfig getInitiatorAlgorithmConfig() const {
    logical::AlgorithmConfig algorithm_config;

    algorithm_config.algorithm_types.push_back(
        assignment_strategy_to_initiator_algorithm_type.at(assignment_strategy));

    return algorithm_config;
  }

  logical::AlgorithmConfig getParticipantAlgorithmConfig() const {
    logical::AlgorithmConfig algorithm_config;

    algorithm_config.algorithm_types.push_back(
        assignment_strategy_to_participant_algorithm_type.at(assignment_strategy));

    return algorithm_config;
  }
};

struct CppsScenariofile : public GeneralScenariofile {
  explicit CppsScenariofile(const std::string &path_to_file) : GeneralScenariofile(path_to_file) {
    SERIALIZE_VAR(initial_number_of_amrs);
    SERIALIZE_VAR(number_of_material_flow_agents);

    SERIALIZE_VAR(algorithm);
    SERIALIZE_VAR(topology);

    SERIALIZE_VAR(autonomous_mobile_robots);
    SERIALIZE_VAR(material_flows);
    SERIALIZE_VAR(scenario_sequence);

    verifyScenarioSequenceOfMaterialFlows();
    verifyScenarioSequenceOfAmrs();
    calcNumbersOfRelativeAmrDistribution();
  }

  uint16_t initial_number_of_amrs = 0;
  uint16_t number_of_material_flow_agents = 0;

  AlgorithmScenario algorithm;
  TopologyScenario topology;

  std::vector<AmrDescriptionScenario> autonomous_mobile_robots;
  std::vector<MaterialFlowDescriptionScenario> material_flows;
  std::vector<SpawnInfoScenario> scenario_sequence;

  std::unordered_map<std::string, AmrDescription> getAmrDescriptions() const;
  std::unordered_map<std::string, MaterialFlowDescriptionScenario> getMaterialFlowDescriptions()
      const;

private:
  void verifyScenarioSequenceOfMaterialFlows() const;
  void verifyScenarioSequenceOfAmrs() const;
  void calcNumbersOfRelativeAmrDistribution();
};
}  // namespace daisi::cpps
#endif
