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

#include "cpps/negotiation/task_management/task_management.h"

#include "ns3/simulator.h"

namespace daisi::cpps {

TaskManagement::TaskManagement(const std::shared_ptr<Kinematics> &kinematics,
                               const std::shared_ptr<mrta::model::Ability> &ability,
                               const std::shared_ptr<ns3::Vector> &last_position,
                               const std::shared_ptr<daisi::cpps::CppsLoggerNs3> &logger,
                               const std::shared_ptr<UtilityEvaluator> &utility_evaluator)
    : kinematics_(kinematics),
      ability_(ability),
      last_position_(last_position),
      logger_(logger),
      utility_evaluator_(utility_evaluator) {}

void TaskManagement::setNextOrderToExecute() {
  pickNextOrderToExecute();

  logCurrentOrderExecution();
  double sim_time = (double)ns3::Simulator::Now().GetMilliSeconds();
  expected_current_order_finish_time_ = sim_time +
                                        current_utility_dimensions_.travel_to_pickup_duration +
                                        current_utility_dimensions_.execution_duration;
}

bool TaskManagement::couldExecuteOrder(const Task &order) {
  bool ability_fits = lessOrEqualAbility(order.getAbilityRequirement(), *ability_);

  if (ability_fits) {
    double execution_duration = std::get<0>(kinematics_->getStartStopTimeAndDistance(
        order.getPickupLocation(), order.getDeliveryLocation(), true));

    double sim_time = (double)ns3::Simulator::Now().GetMilliSeconds();
    double time_window;
    if (sim_time > order.time_window.getEarliestStart()) {
      time_window = order.time_window.getLatestFinish() - sim_time;
    } else {
      time_window = order.time_window.getLatestFinish() - order.time_window.getEarliestStart();
    }
    assert(time_window >= 0);

    return time_window >= execution_duration;
  }

  return false;
}

const Task &TaskManagement::getCurrentOrder() const { return current_order_; }

void TaskManagement::updateCurrentOrderPosition() {
  current_order_.setCurrentPosition(*last_position_);
}

void TaskManagement::setCurrentOrderStatus(const OrderStates &state) {
  current_order_.setOrderState(state);
  logger_->logTransportOrderUpdate(current_order_, agv_uuid_);
}

void TaskManagement::setAgvUUID(const std::string &uuid) { agv_uuid_ = uuid; }

void TaskManagement::logCurrentOrderExecution() {
  daisi::cpps::ExecutedOrderUtilityLoggingInfo logging_info;

  logging_info.order = current_order_.getUUID();
  logging_info.agv = agv_uuid_;
  logging_info.expected_start_time = current_utility_dimensions_.start_time;

  logging_info.execution_duration = current_utility_dimensions_.execution_duration;
  logging_info.execution_distance = current_utility_dimensions_.execution_distance;
  logging_info.travel_to_pickup_duration = current_utility_dimensions_.travel_to_pickup_duration;
  logging_info.travel_to_pickup_distance = current_utility_dimensions_.travel_to_pickup_distance;
  logging_info.makespan = current_utility_dimensions_.makespan;
  logging_info.delay = current_utility_dimensions_.getDelay();

  logging_info.delta_execution_duration = current_utility_dimensions_.delta_execution_duration;
  logging_info.delta_execution_distance = current_utility_dimensions_.delta_execution_distance;
  logging_info.delta_travel_to_pickup_duration =
      current_utility_dimensions_.delta_travel_to_pickup_duration;
  logging_info.delta_travel_to_pickup_distance =
      current_utility_dimensions_.delta_travel_to_pickup_distance;
  logging_info.delta_makespan = current_utility_dimensions_.delta_makespan;

  logging_info.queue_size = current_utility_dimensions_.getQueueSize();
  logging_info.quality = current_utility_dimensions_.getQuality();
  logging_info.costs = current_utility_dimensions_.getCosts();
  logging_info.utility = current_utility_dimensions_.getUtility();

  logger_->logExecutedOrderCost(logging_info);
}

std::pair<ns3::Vector, double> TaskManagement::getEndPositionAndTime() const {
  if (current_order_.getUUID().empty() &&
      current_order_.getOrderState() != OrderStates::kFinished) {
    return {*last_position_, (double)ns3::Simulator::Now().GetMilliSeconds()};
  }

  return {current_order_.getDeliveryLocation(), expected_current_order_finish_time_};
}

mrta::model::Ability TaskManagement::getAbility() const { return *(ability_.get()); }

}  // namespace daisi::cpps
