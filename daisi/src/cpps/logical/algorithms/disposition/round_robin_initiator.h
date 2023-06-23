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

#ifndef DAISI_CPPS_LOGICAL_ALGORITHMS_DISPOSITION_ROUND_ROBIN_INITIATOR_H_
#define DAISI_CPPS_LOGICAL_ALGORITHMS_DISPOSITION_ROUND_ROBIN_INITIATOR_H_

#include <map>
#include <queue>
#include <random>
#include <unordered_map>

#include "centralized_initiator.h"

/// @brief Modified Round Robin Algorithm that centrally assigns tasks of incoming material flows to
/// the corresponding centralized participants.
namespace daisi::cpps::logical {
class RoundRobinInitiator : public CentralizedInitiator {
public:
  RoundRobinInitiator(std::shared_ptr<sola_ns3::SOLAWrapperNs3> sola,
                      std::shared_ptr<CppsLoggerNs3> logger);
  ~RoundRobinInitiator() override = default;

  /// @brief React on a participant's response to a task assignment.
  REGISTER_IMPLEMENTATION(AssignmentResponse);

  /// @brief Receive a participant's status update. For this basic algorithm, it is simply ignored.
  REGISTER_IMPLEMENTATION(StatusUpdate);

  /// @brief Log tasks and orders of a material flow.
  void logMaterialFlowContent(const std::string &material_flow_uuid) override;

private:
  /// @brief Helper class to store all relevant infos to run the round robin assignment.
  struct ParticipantInfoRoundRobin : public ParticipantInfo {
    /// @brief the number of tasks that have already been assigned to the participant.
    uint32_t assignment_counter = 0;

    bool operator<(const ParticipantInfoRoundRobin &other) const {
      return assignment_counter < other.assignment_counter;
    }
    bool operator>(const ParticipantInfoRoundRobin &other) const {
      return assignment_counter > other.assignment_counter;
    }
  };

  /// @brief Add a new participant and store it locally.
  /// @param info The necessary information about the new participant.
  void storeParticipant(ParticipantInfo &info) override;

  /// @brief Initialize round robin task assignment for all unassigned tasks of a material flow.
  /// @param index The index position of the material flow in material_flows_.
  /// @param previously_allocated Indicates wether the tasks of the material flow have been assigned
  /// to participants before. In that case, only some of them may need to be reassigned.
  void distributeMFTasks(uint32_t index, bool previously_allocated) override;

  /// @brief Perform task assignment for a given task.
  /// @param task The task to be assigned.
  void assignTask(const material_flow::Task &task);

  /// @brief Choose one of the known participants for a given task by executing a modified round
  /// robin strategy.
  /// @param task The task to find a participant for.
  /// @return A pointer onto the choosen participant.
  std::shared_ptr<ParticipantInfoRoundRobin> chooseParticipantForTask(
      const material_flow::Task &task);

  /// @brief Iterate through all accepted task assignment. Eventually start reassigning if not all
  /// task where accepted.
  /// @param index The index position of the material flow in material_flows_.
  void processAssignmentAcceptions(uint32_t index);

  /// @brief Map that holds uuids of material flows as key and stores all tasks of that mf, that are
  /// not assigned yet.
  std::map<std::string, std::vector<material_flow::Task>> unallocated_tasks_per_mf_;

  /// @brief The participants of the round robin task assignment, seperated by ability. For a round
  /// robin behaviour, they are stored in a priority queue ordered by their assignment_counter.
  std::unordered_map<amr::AmrStaticAbility,
                     std::priority_queue<ParticipantInfoRoundRobin,
                                         std::vector<ParticipantInfoRoundRobin>, std::greater<>>,
                     amr::AmrStaticAbilityHasher>
      participants_per_ability_;
};
}  // namespace daisi::cpps::logical

#endif
