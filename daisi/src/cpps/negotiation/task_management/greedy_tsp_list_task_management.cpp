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

#include "cpps/negotiation/task_management/greedy_tsp_list_task_management.h"

#include <cassert>
#include <numeric>

namespace daisi::cpps {

GreedyTSPListTaskManagement::GreedyTSPListTaskManagement(
    const std::shared_ptr<Kinematics> &kinematics, const std::shared_ptr<amr::Ability> &ability,
    const std::shared_ptr<ns3::Vector> &last_position,
    const std::shared_ptr<daisi::cpps::CppsLoggerNs3> &logger,
    const std::shared_ptr<UtilityEvaluator> &utility_evaluator)
    : BasicListTaskManagement(kinematics, ability, last_position, logger, utility_evaluator) {}

void GreedyTSPListTaskManagement::updateQueue(
    std::list<std::pair<Task, UtilityDimensions>>::iterator &it) {
  // updating the new order and the first after
  BasicListTaskManagement::updateQueue(it);

  // "it" is at one item after new inserted item
  if (it == queue_.end()) {
    return;
  }

  // updating the all start times after
  UtilityDimensions &previous_dims = std::get<1>(*std::prev(it));
  for (; it != queue_.end(); it++) {
    // updating the start times
    // travel to pickup and execution duration should already be given

    std::get<1>(*it).start_time = previous_dims.start_time + previous_dims.execution_duration +
                                  std::get<1>(*it).travel_to_pickup_duration;

    previous_dims = std::get<1>(*it);
  }
}

UtilityDimensions GreedyTSPListTaskManagement::getUtilityDimensions(const Task &order) {
  return std::get<0>(GreedyTSPListTaskManagement::getUtilityDimensionsAndInsertInfo(order));
}

std::pair<UtilityDimensions, std::shared_ptr<InsertInfo>>
GreedyTSPListTaskManagement::getUtilityDimensionsAndInsertInfo(const Task &order) {
  auto marginal_costs =
      getMarginalCosts(order);  // TODO marginal costs should not be set to 0 if its only one option
  int min_index =
      std::min_element(marginal_costs.begin(), marginal_costs.end()) - marginal_costs.begin();
  auto min_marginal_cost = marginal_costs[min_index];

  bool current_inserted =
      !current_order_.getUUID().empty() && current_order_.getOrderState() != OrderStates::kFinished;
  if (current_inserted) {
    min_index--;
  }

  UtilityDimensions dims(order);
  std::tie(dims.execution_duration, dims.execution_distance) =
      kinematics_->getStartStopTimeAndDistance(order.getPickupLocation(),
                                               order.getDeliveryLocation(), true);

  if (min_index > 0) {  // previous TO is not current order

    auto previous_to = queue_.begin();
    std::advance(previous_to, min_index);
    std::tie(dims.travel_to_pickup_duration, dims.travel_to_pickup_distance) =
        kinematics_->getStartStopTimeAndDistance(std::get<0>(*previous_to).getDeliveryLocation(),
                                                 order.getPickupLocation());
    dims.start_time =
        std::get<1>(*previous_to).start_time + std::get<1>(*previous_to).execution_duration;

  } else if (!current_inserted) {
    std::tie(dims.travel_to_pickup_duration, dims.travel_to_pickup_distance) =
        kinematics_->getStartStopTimeAndDistance(*last_position_, order.getPickupLocation());
    dims.start_time = dims.travel_to_pickup_duration;

  } else if (current_inserted) {
    std::tie(dims.travel_to_pickup_duration, dims.travel_to_pickup_distance) =
        kinematics_->getStartStopTimeAndDistance(current_order_.getDeliveryLocation(),
                                                 order.getPickupLocation());
    dims.start_time = expected_current_order_finish_time_ + dims.travel_to_pickup_duration;

  } else {
    throw std::logic_error("something is wrong");
  }

  auto list_insert_info = std::make_shared<ListInsertInfo>();
  list_insert_info->list_position = min_index;

  dims.setCosts(min_marginal_cost);  // TODO check
  dims.setQueueSize(getNumberOfOrdersInQueue());
  utility_evaluator_->evaluate(dims);

  return {dims, list_insert_info};
}

std::vector<double> GreedyTSPListTaskManagement::getMarginalCosts(const Task &order) {
  if (queue_.empty()) {
    return {0.0};
  }

  std::vector<std::pair<Task, UtilityDimensions>> temp_queue;
  bool current_inserted =
      !current_order_.getUUID().empty() && current_order_.getOrderState() != OrderStates::kFinished;
  if (current_inserted) {
    temp_queue.push_back({current_order_, current_utility_dimensions_});
  }
  temp_queue.insert(temp_queue.end(), queue_.begin(), queue_.end());

  // temp queue =
  // [0] current_order
  // [1] queue_[0]
  // [2] queue_[1]
  // ...
  // [n] queue_[n-1]

  // order insertion options in temp queue are in between
  // [0] and [1]
  // [1] and [2]
  // ...
  // [n-1] and [n]
  // after [n]

  // travel to new order pickup
  // execute new order
  // travel to pickup next from new order delivery
  std::vector<std::pair<double, double>> insertion_travel_to_pickup_dur_dist;
  std::vector<std::pair<double, double>> insertion_travel_after_delivery_dur_dist;

  for (auto to1 = temp_queue.begin(); to1 != temp_queue.end(); to1++) {
    insertion_travel_to_pickup_dur_dist.push_back(kinematics_->getStartStopTimeAndDistance(
        std::get<0>(*to1).getDeliveryLocation(), order.getPickupLocation()));

    auto to2 = std::next(to1);
    if (to2 != temp_queue.end()) {
      insertion_travel_after_delivery_dur_dist.push_back(kinematics_->getStartStopTimeAndDistance(
          order.getDeliveryLocation(), std::get<0>(*to1).getPickupLocation()));
    } else {
      insertion_travel_after_delivery_dur_dist.push_back(std::make_pair(0.0, 0.0));
    }
  }

  std::vector<double> marginal_costs;

  if (current_inserted) {
    std::get<1>(temp_queue[0]).travel_to_pickup_distance = 0.0;
    std::get<1>(temp_queue[0]).travel_to_pickup_duration = 0.0;
    std::get<1>(temp_queue[0]).execution_distance = 0.0;
    std::get<1>(temp_queue[0]).execution_duration = 0.0;
  }

  double order_execution_duration;
  double order_execution_distance;
  std::tie(order_execution_duration, order_execution_distance) =
      kinematics_->getStartStopTimeAndDistance(order.getPickupLocation(),
                                               order.getDeliveryLocation(), true);

  double costs_without_new_order = std::accumulate(
      std::next(temp_queue.begin()), temp_queue.end(), 0.0,
      [](double sum, const auto &pair) { return sum + std::get<1>(pair).getCosts(); });

  int i = 0;
  for (auto to_before_insert = temp_queue.begin(); to_before_insert != temp_queue.end();
       to_before_insert++) {
    double costs_before = 0.0;
    for (auto before = temp_queue.begin(); std::distance(before, to_before_insert) >= 0; before++) {
      costs_before += std::get<1>(*before).getCosts();
    }

    double current_time = to_before_insert->second.start_time +
                          to_before_insert->second.travel_to_pickup_duration +
                          to_before_insert->second.execution_duration;

    UtilityDimensions temp_new_order_dims(order);
    temp_new_order_dims.execution_distance = order_execution_distance;
    temp_new_order_dims.execution_duration = order_execution_duration;
    temp_new_order_dims.travel_to_pickup_duration =
        std::get<0>(insertion_travel_to_pickup_dur_dist[i]);
    temp_new_order_dims.travel_to_pickup_distance =
        std::get<1>(insertion_travel_to_pickup_dur_dist[i]);
    temp_new_order_dims.start_time = current_time;
    temp_new_order_dims.setQueueSize(queue_.size());
    utility_evaluator_->evaluate(temp_new_order_dims);

    current_time += temp_new_order_dims.travel_to_pickup_duration;
    current_time += temp_new_order_dims.execution_duration;

    double full_costs;
    auto to_after_insert = std::next(to_before_insert);
    if (to_after_insert != temp_queue.end()) {
      UtilityDimensions temp_to_after_dims = std::get<1>(*to_after_insert);
      temp_to_after_dims.travel_to_pickup_duration =
          std::get<0>(insertion_travel_after_delivery_dur_dist[i]);
      temp_to_after_dims.travel_to_pickup_distance =
          std::get<1>(insertion_travel_after_delivery_dur_dist[i]);
      temp_to_after_dims.start_time = current_time;
      utility_evaluator_->reevaluate(temp_to_after_dims);

      current_time += temp_to_after_dims.travel_to_pickup_duration;
      current_time += temp_to_after_dims.execution_duration;

      double costs_after = 0.0;
      for (auto after = std::next(to_after_insert); after != temp_queue.end(); after++) {
        after->second.start_time = current_time;

        current_time = after->second.travel_to_pickup_duration;
        current_time = after->second.execution_duration;

        costs_after += std::get<1>(*after).getCosts();
      }

      full_costs = costs_before + temp_to_after_dims.getCosts() + temp_new_order_dims.getCosts() +
                   costs_after;
    } else {
      full_costs = costs_before + temp_new_order_dims.getCosts();
    }

    assert(full_costs > costs_without_new_order);

    marginal_costs.push_back(full_costs - costs_without_new_order);

    i++;
  }

  return marginal_costs;
}

}  // namespace daisi::cpps
