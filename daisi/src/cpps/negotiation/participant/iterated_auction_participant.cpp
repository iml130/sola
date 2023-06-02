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

#include "cpps/negotiation/participant/iterated_auction_participant.h"

namespace daisi::cpps {

IteratedAuctionParticipant::IteratedAuctionParticipant(
    const std::shared_ptr<sola_ns3::SOLAWrapperNs3> &sola,
    const std::unique_ptr<TaskManagement> &task_management)
    : TaskAllocationParticipant(sola, task_management) {}

void IteratedAuctionParticipant::receiveMessage(const Message &msg) {
  auto overload =
      Overload{[&](const SSICallForProposal &msg) { processCallForProposal(msg); },
               [&](const SSIWinnerNotification &msg) { processWinnerNotification(msg); },
               [&](const SSIIterationNotification &msg) { processIterationNotification(msg); },
               [&](auto /*unused*/) {
                 throw std::invalid_argument("SSI Participant cannot handle this Message Type");
               }};

  std::visit(overload, msg);
}

void IteratedAuctionParticipant::processCallForProposal(const SSICallForProposal &msg) {
  std::string initiator_connection = msg.getInitiatorConnection();
  auction_infos_[initiator_connection] = AuctionInfo{msg.getTasks()};

  performBidSubmission(initiator_connection);
}

void IteratedAuctionParticipant::processWinnerNotification(const SSIWinnerNotification &msg) {
  std::string initiator_connection = msg.getInitiatorConnection();
  const std::string auctioned_task_uuid = msg.getTaskUuid();
  std::string participant_connection = sola_->getConectionString();

  auto it_auction_infos = auction_infos_.find(initiator_connection);
  if (it_auction_infos == auction_infos_.end()) {
    throw std::logic_error("We are the winner, but do not have information about auction");
  }

  auto it_task = std::find_if(
      it_auction_infos->second.auctioning_orders.begin(),
      it_auction_infos->second.auctioning_orders.end(),
      [auctioned_task_uuid](const Task &o) { return o.getUUID() == auctioned_task_uuid; });
  if (it_task == it_auction_infos->second.auctioning_orders.end()) {
    throw std::logic_error("Winning participant does not have information about auctioned order");
  }

  UtilityDimensions utility_dimensions = UtilityDimensions::createInvalid();
  std::shared_ptr<InsertInfo> insert_info;

  if (!it_auction_infos->second.previous_calculation_info.empty()) {
    // we have information stored
    auto it_prev_info =
        it_auction_infos->second.previous_calculation_info.find(auctioned_task_uuid);
    std::tie(utility_dimensions, insert_info) = it_prev_info->second;

  } else {
    SSIWinnerResponse response(auctioned_task_uuid, false, participant_connection);
    sola_->sendData(serialize(response), sola::Endpoint(initiator_connection));
    return;
  }

  bool success = false;
  if (task_management_->couldExecuteOrder(*it_task) &&
      utility_dimensions.getUtility() > std::numeric_limits<double>::lowest()) {
    it_task->setConnection(initiator_connection);
    success = task_management_->addOrder(*it_task, insert_info);
  }

  SSIWinnerResponse response(auctioned_task_uuid, success, participant_connection);
  sola_->sendData(serialize(response), sola::Endpoint(initiator_connection));

  if (success) {
    it_auction_infos->second.auctioning_orders.erase(it_task);
    clearStoredInformationAfterInsertion();
  } else {
    it_auction_infos->second.auctioning_orders.erase(it_task);
  }
}

void IteratedAuctionParticipant::processIterationNotification(const SSIIterationNotification &msg) {
  std::string initiator_connection = msg.getInitiatorConnection();
  std::string participant_connection = sola_->getConectionString();

  auto it_auction_infos = auction_infos_.find(initiator_connection);
  if (it_auction_infos != auction_infos_.end()) {
    std::vector<Task> &auctioning_orders = it_auction_infos->second.auctioning_orders;

    for (auto const &auctioned_task_uuid : msg.getAuctionedTasksUuids()) {
      auctioning_orders.erase(std::remove_if(auctioning_orders.begin(), auctioning_orders.end(),
                                             [auctioned_task_uuid](const Task &o) {
                                               return o.getUUID() == auctioned_task_uuid;
                                             }),
                              auctioning_orders.end());

      it_auction_infos->second.previous_calculation_info.erase(auctioned_task_uuid);
    }

    if (auctioning_orders.empty()) {
      // assert(it_auction_infos->second.previous_calculation_info.empty());
      auction_infos_.erase(it_auction_infos);
    }

    performBidSubmission(initiator_connection);
  }
}

void IteratedAuctionParticipant::performBidSubmission(const std::string &initiator_connection) {
  for (auto &[key, entry] : auction_infos_) {
    assert(entry.auctioning_orders.size() >= entry.previous_calculation_info.size());
  }

  auto auction_info_it = auction_infos_.find(initiator_connection);
  if (auction_info_it == auction_infos_.end()) {
    return;
  }

  if (auction_info_it->second.auctioning_orders.empty()) {
    // auction finished -> remove from map
    auction_infos_.erase(auction_info_it);
    return;
  }

  double best_utility = std::numeric_limits<double>::lowest();
  std::string order_uuid_with_best_utility;

  bool capable;
  bool acceptable;
  for (auto const &order : auction_info_it->second.auctioning_orders) {
    auto prev_calc_info = auction_info_it->second.previous_calculation_info.find(order.getUUID());
    if (prev_calc_info != auction_info_it->second.previous_calculation_info.end()) {
      double utility = std::get<0>(prev_calc_info->second).getUtility();
      if (utility > best_utility && std::get<1>(prev_calc_info->second) != nullptr) {
        best_utility = utility;
        order_uuid_with_best_utility = order.getUUID();
      }

    } else {
      capable = task_management_->couldExecuteOrder(order);
      acceptable = false;
      if (capable) {
        auto const &[utility_dims, insert_info] =
            task_management_->getUtilityDimensionsAndInsertInfo(order);
        double utility = utility_dims.getUtility();
        acceptable = utility > std::numeric_limits<double>::lowest();
        if (acceptable) {
          // capable and acceptable
          auction_info_it->second.previous_calculation_info[order.getUUID()] = {utility_dims,
                                                                                insert_info};
          if (utility > best_utility) {
            best_utility = utility;
            order_uuid_with_best_utility = order.getUUID();
          }
        }
      }

      if (!capable || !acceptable) {
        // not capable or not acceptable
        auction_info_it->second.previous_calculation_info[order.getUUID()] = {
            UtilityDimensions::createInvalid(), nullptr};
      }
    }
  }

  // send result
  if (!order_uuid_with_best_utility.empty()) {
    // but only send if we have previously submitted something different
    // otherwise the initiator still has our bid
    if (resubmit_bids_ ||
        (order_uuid_with_best_utility != auction_info_it->second.previously_submitted)) {
      std::string participant_connection = sola_->getConectionString();
      auto best_udim = std::get<0>(
          auction_info_it->second.previous_calculation_info[order_uuid_with_best_utility]);
      SSISubmission submission(order_uuid_with_best_utility, initiator_connection,
                               participant_connection, task_management_->getAbility(), best_udim);

      sendBidSubmission(submission);

      auction_info_it->second.previously_submitted = order_uuid_with_best_utility;
    }
  }
}

void IteratedAuctionParticipant::clearStoredInformationAfterInsertion() {
  for (auto &[key, auction_info] : auction_infos_) {
    auction_info.previous_calculation_info.clear();
  }
}

void IteratedAuctionParticipant::sendBidSubmission(const SSISubmission &submission) {
  sola_->sendData(serialize(submission), sola::Endpoint(submission.getInitiatorConnection()));
}

}  // namespace daisi::cpps
