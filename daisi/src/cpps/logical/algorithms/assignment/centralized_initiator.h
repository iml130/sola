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

#ifndef DAISI_CPPS_LOGICAL_ALGORITHMS_ASSIGNMENT_CENTRALIZED_INITIATOR_H_
#define DAISI_CPPS_LOGICAL_ALGORITHMS_ASSIGNMENT_CENTRALIZED_INITIATOR_H_

#include "assignment_initiator.h"
#include "cpps/common/cpps_communicator.h"

namespace daisi::cpps::logical {

/// @brief Algorithm that centrally assigns tasks of incoming material flows to the
/// corresponding centralized participants. Should be implemented by any concrete centralized task
/// assignment algorithm.
class CentralizedInitiator : public AssignmentInitiator {
public:
  CentralizedInitiator(daisi::cpps::common::CppsCommunicatorPtr communicator,
                       std::shared_ptr<CppsLoggerNs3> logger);
  ~CentralizedInitiator() override = default;

  // TODO: check if possible to restict implementation by child classes without warnings
  //   REQUIRE_IMPLEMENTATION(AssignmentResponse);
  //   REQUIRE_IMPLEMENTATION(StatusUpdate);

  /// @brief Receive a new material flow to assign its tasks to the known participants. Push it
  /// into material_flows_.
  /// @param scheduler
  void addMaterialFlow(std::shared_ptr<material_flow::MFDLScheduler> scheduler) override;

protected:
  /// @brief Helper that stores all relevant information about the task assignment participants. If
  /// more specific information are necessary, use a struct that derives from ParticipantInfo.
  struct ParticipantInfo {
    /// @brief Connection string used to contact the participant.
    std::string connection_string;

    /// @brief The participant's ability. Used to check if a task's requirements are met by the AMR.
    amr::AmrStaticAbility ability;

    /// @brief Check wether all entries are initialized correctly
    bool isValid() const { return (!connection_string.empty() && ability.isValid()); }
  };

  /// @brief Perform task assignment for all unassigned tasks of a material flow.
  /// @param index The index position of the material flow in material_flows_.
  /// @param previously_allocated Indicates wether the tasks of the material flow have been assigned
  /// to participants before. In that case, only some of them may need to be reassigned.
  virtual void distributeMFTasks(uint32_t index, bool previously_allocated) = 0;

  /// @brief Add a new participant and store it locally.
  /// @param info The necessary information about the new participant.
  virtual void storeParticipant(ParticipantInfo &info) = 0;

  /// @brief Log the new state of a task.
  /// @param task The changed task.
  /// @param order_state The new order state.
  void logMaterialFlowOrderStatesOfTask(const material_flow::Task &task,
                                        const OrderStates &order_state);

  /// @brief All material flows that have been received to assign their tasks.
  std::vector<material_flow::MFDLScheduler> material_flows_;

  /// @brief Responses of task assignments that have been accepted.
  std::vector<AssignmentResponse> assignment_acceptions_;

  /// @brief Storing all delays in one place. The unit is seconds.
  struct {
    /// @brief Delay between assigning a task and expecting a response.
    util::Duration wait_to_receive_assignment_response = 0.3;
    /// @brief Delay between requesting and receiving a status update.
    util::Duration wait_to_receive_status_update = 0.3;
  } delays_;

private:
  /// @brief Read out all relevant information from amr_find_result_ to create ParticipantInfos for
  /// each participant. Only used once in the preparation phase.
  void readAmrRequestFuture();

  /// @brief Gain information about the participating AMRs. Only read out in the preparation phase.
  std::future<minhton::FindResult> amr_find_result_;

  /// @brief Helper to mark wether amr_find_result_ is ready to be read. Only used in the
  /// preparation phase.
  bool preparation_finished_ = false;
};
}  // namespace daisi::cpps::logical

#endif
