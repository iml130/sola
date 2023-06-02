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

#include "cpps/negotiation/initiator/iterated_auction_initiator.h"

#include <limits>

#include "cpps/model/ability.h"
#include "cpps/negotiation/utils/kmeans.h"
#include "cpps/negotiation/utils/simple_temporal_network.h"
#include "ns3/vector.h"

namespace daisi::cpps {

IteratedAuctionInitiator::IteratedAuctionInitiator(
    const std::shared_ptr<sola_ns3::SOLAWrapperNs3> &sola,
    const std::shared_ptr<daisi::cpps::CppsLoggerNs3> &logger,
    const std::shared_ptr<MaterialFlowModel> &material_flow_model)
    : TaskAllocationInitiator(sola, logger, material_flow_model),
      preparation_finished_(false),
      no_winner_acceptions_counter_(0) {}

void IteratedAuctionInitiator::receiveMessage(const Message &msg) {
  auto overload = Overload{[&](const SSISubmission &msg) { processBidSubmission(msg); },
                           [&](const SSIWinnerResponse &msg) { processWinnerResponse(msg); },
                           [&](auto /*unused*/) {}};

  std::visit(overload, msg);
}

void IteratedAuctionInitiator::init() {
  material_flow_model_->generateSimpleAdjacencyList();
  material_flow_model_->generateSimpleInvertedAdjacencyList();
  number_of_tasks_ = material_flow_model_->getNumberOfvertices();

  initLayers();
  calcTaskPriorities();

  // initialize PC : earliest valid start time
  double sim_time = (double)ns3::Simulator::Now().GetMilliSeconds();
  for (auto &task : free_layer_tasks_) {
    earliest_valid_start_time_[task] = sim_time;
    task.precedence_constraints.setEarliestValidStartTime(sim_time);
  }

  // depening on the interaction protocol, subscribe to topics, gather information etc.
  prepareInteraction();

  // schedule to ensure that preparation has finished
  ns3::Simulator::Schedule(ns3::MilliSeconds(delays_.interaction_preparation),
                           &IteratedAuctionInitiator::startIteration, this);
}

void IteratedAuctionInitiator::startIteration() {
  if (scheduled_layer_tasks_.size() == number_of_tasks_) {
    // all tasks are scheduled
    return;
  }

  assert(preparation_finished_);

  selectAuctionableTasks();

  assert(!initial_auctionable_tasks_.empty() && !auctionable_tasks_.empty() &&
         (initial_auctionable_tasks_.size() == auctionable_tasks_.size()));

  // --- Start of Algorithm ---

  taskAnnoucement();

  bids_.clear();
  winner_acceptions_.clear();

  // starting (while T_{auct} != emptyset) loop
  // waiting to receive bids
  ns3::Simulator::Schedule(ns3::MilliSeconds(delays_.waiting_to_receive_bids),
                           &IteratedAuctionInitiator::bidProcessing, this);
}

void IteratedAuctionInitiator::bidProcessing() {
  // receiving bids in the meantime
  auto winners = selectWinners();
  if (!winners.empty()) {
    notifyWinners(winners);

    // waiting to receive winner responses
    ns3::Simulator::Schedule(ns3::MilliSeconds(delays_.waiting_to_receive_winner_responses),
                             &IteratedAuctionInitiator::winnerResponseProcessing, this);
  } else {
    renotifyAboutOpenTasks();

    ns3::Simulator::Schedule(ns3::MilliSeconds(delays_.waiting_to_receive_bids),
                             &IteratedAuctionInitiator::bidProcessing, this);
  }
}

void IteratedAuctionInitiator::notifyWinners(
    const std::vector<IteratedAuctionInitiator::Winner> &winners) {
  winner_acceptions_.clear();

  std::string initiator_connection = sola_->getConectionString();
  for (auto const &winner : winners) {
    double latest_finish_time = latest_finish_times_[winner.task_uuid];
    SSIWinnerNotification notification(winner.task_uuid, latest_finish_time, initiator_connection);
    sola_->sendData(serialize(notification), sola::Endpoint(winner.winner_connection));
  }
}

void IteratedAuctionInitiator::winnerResponseProcessing() {
  if (winner_acceptions_.empty()) {
    if (++no_winner_acceptions_counter_ >= 100) {
      throw std::runtime_error("No winner acceptions received 100 times in a row");
    }
  } else {
    no_winner_acceptions_counter_ = 0;
  }

  std::vector<std::tuple<std::string, Task>> iteration_info;

  for (auto const &accepted_winner : winner_acceptions_) {
    auto it_task =
        std::find_if(auctionable_tasks_.begin(), auctionable_tasks_.end(),
                     [&](const Task &t) { return t.getUUID() == accepted_winner.task_uuid; });
    assert(it_task != auctionable_tasks_.end());

    removeBidsForTask(bids_, it_task->getUUID());

    // removeBidsWhichMeetAbilityRequirement(bids_, it_task->getAbilityRequirement());

    iteration_info.push_back({accepted_winner.winner_connection, *it_task});

    auctionable_tasks_.erase(it_task);
  }

  winner_acceptions_.clear();

  iterationNotification(iteration_info);

  if (auctionable_tasks_.empty()) {
    finishIteration();

  } else {
    // continue (while T_{auct} != emptyset) loop

    // waiting to receive bids
    ns3::Simulator::Schedule(ns3::MilliSeconds(delays_.waiting_to_receive_bids),
                             &IteratedAuctionInitiator::bidProcessing, this);
  }
}

void IteratedAuctionInitiator::finishIteration() {
  updateLayers();

  bids_.clear();
  winner_acceptions_.clear();

  startIteration();
}

void IteratedAuctionInitiator::removeBidsForTask(std::vector<ReceivedBids> &bids,
                                                 const std::string &task_uuid) {
  auto remove_condition_lambda = [&task_uuid](const IteratedAuctionInitiator::ReceivedBids &bid) {
    return bid.task_uuid == task_uuid;
  };

  bids.erase(std::remove_if(bids.begin(), bids.end(), remove_condition_lambda), bids.end());
}

void IteratedAuctionInitiator::removeBidsForWinner(std::vector<ReceivedBids> &bids,
                                                   const std::string &task_uuid,
                                                   const std::string &winner_connection) {
  auto remove_condition_lambda = [&](const IteratedAuctionInitiator::ReceivedBids &bid) {
    return bid.task_uuid == task_uuid && bid.participant_connection == winner_connection;
  };

  bids.erase(std::remove_if(bids.begin(), bids.end(), remove_condition_lambda), bids.end());
}

void IteratedAuctionInitiator::removeBidsWhichMeetAbilityRequirement(
    std::vector<ReceivedBids> &bids, const mrta::model::Ability &ability_requirement) {
  auto remove_condition_lambda =
      [&ability_requirement](const IteratedAuctionInitiator::ReceivedBids &bid) {
        return comparableAbility(ability_requirement, bid.participant_ability) &&
               lessOrEqualAbility(ability_requirement, bid.participant_ability);
      };

  bids.erase(std::remove_if(bids.begin(), bids.end(), remove_condition_lambda), bids.end());
}

void IteratedAuctionInitiator::removeBidsWhichAreLesserThanAbilityRequirement(
    std::vector<ReceivedBids> &bids, const mrta::model::Ability &ability_requirement) {
  auto remove_condition_lambda =
      [&ability_requirement](const IteratedAuctionInitiator::ReceivedBids &bid) {
        return comparableAbility(ability_requirement, bid.participant_ability) &&
               lessOrEqualAbility(bid.participant_ability, ability_requirement);
      };

  bids.erase(std::remove_if(bids.begin(), bids.end(), remove_condition_lambda), bids.end());
}

void IteratedAuctionInitiator::processBidSubmission(const SSISubmission &msg) {
  // receive bids and insert them into B

  bids_.push_back(IteratedAuctionInitiator::ReceivedBids{
      msg.getTaskUuid(), msg.getParticipantConnection(), msg.getParticipantAbility(),
      msg.getUtilityDimensions().value()});
}

void IteratedAuctionInitiator::processWinnerResponse(const SSIWinnerResponse &msg) {
  // receive responses and insert accepted ones into W_{accept}

  if (msg.doesAccept()) {
    winner_acceptions_.push_back(
        IteratedAuctionInitiator::Winner{msg.getTaskUuid(), msg.getParticipantConnection()});
  } else {
    removeBidsForWinner(bids_, msg.getTaskUuid(), msg.getParticipantConnection());
  }
}

void IteratedAuctionInitiator::initLayers() {
  // free layer -> no incoming edges
  auto incoming = material_flow_model_->getSimpleInvertedAdjacencyList();
  for (auto const &[task, incoming_list] : incoming) {
    if (incoming_list.empty()) {
      free_layer_tasks_.push_back(material_flow_model_->getUpToDateOrder(task));
    }
  }

  // second layer -> neighbors of free layer
  const std::unordered_map<Task, std::unordered_map<Task, std::vector<GEdge>>> &adjacency_list =
      material_flow_model_->getAdjacencyLists();
  for (auto const &free_order : free_layer_tasks_) {
    for (auto const &[neighbor, _] : adjacency_list.at(free_order)) {
      second_layer_tasks_.push_back(material_flow_model_->getUpToDateOrder(neighbor));
    }
  }

  // removing potential duplicates
  std::sort(second_layer_tasks_.begin(), second_layer_tasks_.end());
  second_layer_tasks_.erase(std::unique(second_layer_tasks_.begin(), second_layer_tasks_.end()),
                            second_layer_tasks_.end());

  // hidden layer via set difference
  std::vector<Task> all_orders = material_flow_model_->getVertices();
  std::sort(all_orders.begin(), all_orders.end());

  std::vector<Task> first_layers = free_layer_tasks_;
  first_layers.insert(first_layers.begin(), second_layer_tasks_.begin(), second_layer_tasks_.end());
  std::sort(first_layers.begin(), first_layers.end());

  std::set_difference(all_orders.begin(), all_orders.end(), first_layers.begin(),
                      first_layers.end(), std::back_inserter(hidden_layer_tasks_));
}

void IteratedAuctionInitiator::updateLayers() {
  // insert T_auct into T_S
  scheduled_layer_tasks_.insert(scheduled_layer_tasks_.end(), initial_auctionable_tasks_.begin(),
                                initial_auctionable_tasks_.end());

  // sort needed vectors
  std::sort(scheduled_layer_tasks_.begin(), scheduled_layer_tasks_.end());
  std::sort(initial_auctionable_tasks_.begin(), initial_auctionable_tasks_.end());
  std::sort(free_layer_tasks_.begin(), free_layer_tasks_.end());

  // remove T_auct from T_F
  std::vector<Task> free_without_auct;
  std::set_difference(free_layer_tasks_.begin(), free_layer_tasks_.end(),
                      initial_auctionable_tasks_.begin(), initial_auctionable_tasks_.end(),
                      std::back_inserter(free_without_auct));
  free_layer_tasks_ = free_without_auct;

  // Translating pIA Pseudocode:

  // for all t in T_auct do: UpdatePrecGraph
  for (auto const &t : initial_auctionable_tasks_) {
    // for all t' in (T_L intersection children(t)) do:
    std::vector<Task> children_of_t = material_flow_model_->getChildren(t);
    std::vector<Task> second_layer_children_of_t;
    std::sort(second_layer_tasks_.begin(), second_layer_tasks_.end());
    std::sort(children_of_t.begin(), children_of_t.end());
    std::set_intersection(second_layer_tasks_.begin(), second_layer_tasks_.end(),
                          children_of_t.begin(), children_of_t.end(),
                          std::back_inserter(second_layer_children_of_t));

    for (auto const &t_dash : second_layer_children_of_t) {
      // if parents(t') subset T_S then
      std::vector<Task> parents_of_t_dash = material_flow_model_->getParents(t_dash);
      bool parents_of_t_dash_subset_of_scheduled =
          std::includes(scheduled_layer_tasks_.begin(), scheduled_layer_tasks_.end(),
                        parents_of_t_dash.begin(), parents_of_t_dash.end());

      if (parents_of_t_dash_subset_of_scheduled) {
        // Move t': T_L -> T_F
        second_layer_tasks_.erase(
            std::remove(second_layer_tasks_.begin(), second_layer_tasks_.end(), t_dash),
            second_layer_tasks_.end());
        free_layer_tasks_.push_back(t_dash);

        // PC[t'] = max_{t'' in parents(t')} F[t'']
        std::vector<double> parent_finish_times;
        for (auto const &parent : material_flow_model_->getParents(t_dash)) {
          parent_finish_times.push_back(latest_finish_times_[parent.getUUID()]);
        }
        earliest_valid_start_time_[t_dash] =
            *std::max_element(parent_finish_times.begin(), parent_finish_times.end());

        // for all t'' in (T_H intersection children(t')) d0:
        std::vector<Task> children_of_t_dash = material_flow_model_->getChildren(t_dash);
        std::vector<Task> hidden_layer_children_of_d_dash;
        std::sort(hidden_layer_tasks_.begin(), hidden_layer_tasks_.end());
        std::sort(children_of_t_dash.begin(), children_of_t_dash.end());
        std::set_intersection(hidden_layer_tasks_.begin(), hidden_layer_tasks_.end(),
                              children_of_t_dash.begin(), children_of_t_dash.end(),
                              std::back_inserter(hidden_layer_children_of_d_dash));

        for (auto const &t_dash_dash : hidden_layer_children_of_d_dash) {
          // if parents(t'') subset (T_S union T_F)
          std::vector<Task> parents_of_t_dash_dash = material_flow_model_->getParents(t_dash_dash);
          std::vector<Task> scheduled_union_free = scheduled_layer_tasks_;
          scheduled_union_free.insert(scheduled_union_free.end(), free_layer_tasks_.begin(),
                                      free_layer_tasks_.end());
          std::sort(scheduled_union_free.begin(), scheduled_union_free.end());

          bool parents_of_t_dash_dash_subset_of_scheduled =
              std::includes(scheduled_union_free.begin(), scheduled_union_free.end(),
                            parents_of_t_dash_dash.begin(), parents_of_t_dash_dash.end());
          if (parents_of_t_dash_dash_subset_of_scheduled) {
            // Move t'': T_H -> T_L
            hidden_layer_tasks_.erase(
                std::remove(hidden_layer_tasks_.begin(), hidden_layer_tasks_.end(), t_dash_dash),
                hidden_layer_tasks_.end());
            second_layer_tasks_.push_back(t_dash_dash);
          }
        }
      }
    }
  }
}

void IteratedAuctionInitiator::calcTaskPriorities() {
  // dynamic programming on a graph

  const std::unordered_map<Task, std::unordered_map<Task, std::vector<GEdge>>> &adjacency_list =
      material_flow_model_->getAdjacencyLists();

  std::unordered_map<int, std::vector<Task>> layers = material_flow_model_->calcPrecedenceLayers();
  int i = 0;

  std::unordered_map<Task, double> l_t;
  std::unordered_map<Task, double> u_t;

  while (layers.find(i) != layers.end()) {
    std::vector<Task> current_layer = layers[i++];

    for (auto const &task : current_layer) {
      std::vector<double> children_l = {0.0};
      std::vector<double> children_u = {0.0};

      for (auto const &[neighbor, _] : adjacency_list.at(task)) {
        children_l.push_back(l_t[neighbor]);

        double distance_order_to_neighbor =
            CalculateDistance(task.getDeliveryLocation(), neighbor.getPickupLocation());
        children_u.push_back(u_t[neighbor] + distance_order_to_neighbor);
      }

      double max_l = *std::max_element(children_l.begin(), children_l.end());
      double max_u = *std::max_element(children_u.begin(), children_u.end());

      l_t[task] = max_l + task.getFastestDuration();
      u_t[task] = max_u + task.getSlowestDuration();
    }
  }

  double prioritization_alpha = 0.5;

  // weighting factor between l_t and u_t; alpha in [0, 1]
  assert(prioritization_alpha >= 0);
  assert(prioritization_alpha <= 1);

  for (auto const &task : material_flow_model_->getVertices()) {
    task_prioritization_[task] =
        (1 - prioritization_alpha) * l_t[task] + prioritization_alpha * u_t[task];
  }
}

void IteratedAuctionInitiator::selectAuctionableTasks() {
  if (second_layer_tasks_.empty()) {
    current_critical_value_ = 0;
  } else {
    std::vector<double> second_layer_priorities;
    std::transform(second_layer_tasks_.begin(), second_layer_tasks_.end(),
                   std::back_inserter(second_layer_priorities),
                   [&](const Task &t) { return task_prioritization_[t]; });
    current_critical_value_ =
        *std::max_element(second_layer_priorities.begin(), second_layer_priorities.end());
  }

  auctionable_tasks_.clear();
  initial_auctionable_tasks_.clear();

  assert(!free_layer_tasks_.empty());

  std::copy_if(free_layer_tasks_.begin(), free_layer_tasks_.end(),
               std::back_inserter(auctionable_tasks_),
               [&](const Task &t) { return task_prioritization_[t] >= current_critical_value_; });
  // copy for updating of layers at end of iteration step
  initial_auctionable_tasks_ = auctionable_tasks_;
}

}  // namespace daisi::cpps
