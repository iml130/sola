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

#ifndef DAISI_NEGOTIATION_ITERATED_AUCTION_INITIATOR_NS3_H_
#define DAISI_NEGOTIATION_ITERATED_AUCTION_INITIATOR_NS3_H_

#include <memory>

#include "cpps/message/ssi_call_for_proposal.h"
#include "cpps/message/ssi_iteration_notification.h"
#include "cpps/message/ssi_submission.h"
#include "cpps/message/ssi_winner_notification.h"
#include "cpps/message/ssi_winner_response.h"
#include "cpps/model/ability.h"
#include "cpps/negotiation/initiator/task_allocation_initiator.h"
#include "cpps/negotiation/utility/utility_evaluator.h"
#include "cpps/negotiation/utils/overload.h"
#include "storage/storage.h"

namespace daisi::cpps {

class IteratedAuctionInitiator : public TaskAllocationInitiator {
public:
  IteratedAuctionInitiator(const std::shared_ptr<sola_ns3::SOLAWrapperNs3> &sola,
                           const std::shared_ptr<daisi::cpps::CppsLoggerNs3> &logger,
                           const std::shared_ptr<MaterialFlowModel> &material_flow_model);

  virtual ~IteratedAuctionInitiator() = default;

  void receiveMessage(const Message &msg) override;

  virtual void init() final;

protected:
  struct ReceivedBids {
    std::string task_uuid;
    std::string participant_connection;
    mrta::model::Ability participant_ability;
    UtilityDimensions udims;

    friend bool operator==(const ReceivedBids &r1, const ReceivedBids &r2) {
      return (r1.task_uuid == r2.task_uuid) &&
             (r1.participant_connection == r2.participant_connection) &&
             (r1.udims.getUtility() == r2.udims.getUtility());
    }
  };

  struct Winner {
    std::string task_uuid;
    std::string winner_connection;
  };

  struct {
    // Delays for scheduling
    // given in milliseconds

    uint64_t interaction_preparation = 5000;

    uint64_t waiting_to_receive_bids = 700;
    uint64_t waiting_to_receive_winner_responses = 300;

  } delays_;

  // depending on the subclass
  // subscribing to topics etc...
  virtual void prepareInteraction() = 0;

  bool preparation_finished_;

  // sending call for proposals
  virtual void taskAnnoucement() = 0;

  // selecting winners based on B_{valid}
  virtual std::vector<Winner> selectWinners() = 0;

  // sending winner notifications
  virtual void notifyWinners(const std::vector<Winner> &winners);  // protected - may get overridden

  // sending iteration notifications
  virtual void iterationNotification(
      const std::vector<std::tuple<std::string, Task>> &iteration_info) = 0;

  // sending iteration notification without auctioned tasks
  virtual void renotifyAboutOpenTasks() = 0;

  // B_{valid} in IP algorithm
  std::vector<ReceivedBids> bids_;

  // W_{accept} in IP algorithm
  std::vector<Winner> winner_acceptions_;

  // T_{auct} in pIA algorithm
  std::vector<Task> auctionable_tasks_;

  // F in pIA; latest finish time of tasks that have been scheduled
  std::unordered_map<std::string, double> latest_finish_times_;

  uint8_t no_bids_counter_ = 0;

  // --- Helper methods ---

  static void removeBidsForTask(std::vector<ReceivedBids> &bids, const std::string &task_uuid);
  static void removeBidsForWinner(std::vector<ReceivedBids> &bids, const std::string &task_uuid,
                                  const std::string &winner_connection);
  static void removeBidsWhichMeetAbilityRequirement(
      std::vector<ReceivedBids> &bids, const mrta::model::Ability &ability_requirement);
  static void removeBidsWhichAreLesserThanAbilityRequirement(
      std::vector<ReceivedBids> &bids, const mrta::model::Ability &ability_requirement);

private:
  // receiving and inserting bids into B_{valid}
  void processBidSubmission(const SSISubmission &msg);

  // receiving and inserting accepted winner-responses into W_{accept}
  void processWinnerResponse(const SSIWinnerResponse &msg);

  // starting outer IP algorithm loop
  void startIteration();

  // reparing for next IP algorithm loop
  void finishIteration();

  // start of the (while T_{auct} != emptyset) loop in the IP algorithm
  void bidProcessing();

  // processing winner responses, updating B_{valid} and T_{auct}
  void winnerResponseProcessing();

  // --- pIA algorithm ---

  void initLayers();
  void updateLayers();
  void calcTaskPriorities();
  void selectAuctionableTasks();

  // T_F in pIA algorithm
  std::vector<Task> free_layer_tasks_;

  // T_L in pIA algorithm
  std::vector<Task> second_layer_tasks_;

  // T_H in pIA algorithm
  std::vector<Task> hidden_layer_tasks_;

  // T_S in pIA algorithm
  std::vector<Task> scheduled_layer_tasks_;

  // copy of T_{auct} to update graph afterwards
  std::vector<Task> initial_auctionable_tasks_;

  // c in pIA algorithm to select
  double current_critical_value_ = 0.0;

  uint16_t number_of_tasks_ = 0;

  // PC in pIA; earliest valid start time for tasks whose preconditions have been scheduled
  // tasks initially in T_F can be started at any time -> 0/now appropriate value
  std::unordered_map<Task, double> earliest_valid_start_time_;

  // priority for each task
  std::unordered_map<Task, double> task_prioritization_;

  uint8_t no_winner_acceptions_counter_;
};
}  // namespace daisi::cpps

#endif
