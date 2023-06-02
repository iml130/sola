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

#include "cpps/negotiation/task_management/stn_task_management.h"

#include <numeric>

namespace daisi::cpps {

STNTaskManagement::STNTaskManagement(const std::shared_ptr<Kinematics> &kinematics,
                                     const std::shared_ptr<mrta::model::Ability> &ability,
                                     const std::shared_ptr<ns3::Vector> &last_position,
                                     const std::shared_ptr<daisi::cpps::CppsLoggerNs3> &logger,
                                     const std::shared_ptr<UtilityEvaluator> &utility_evaluator)
    : TaskManagement(kinematics, ability, last_position, logger, utility_evaluator),
      stn_(std::make_shared<SimpleTemporalNetwork>(utility_evaluator)) {}

void STNTaskManagement::pickNextOrderToExecute() {
  std::string next_order_uuid = stn_->getNextOrderUUID();
  std::tie(current_order_, current_utility_dimensions_) = stn_->removeOrder(next_order_uuid);
}

bool STNTaskManagement::hasOrdersInQueue() const { return stn_->getNumberOfIncludedOrders() > 0; }

bool STNTaskManagement::canExecuteNextOrder() const {
  if (!hasOrdersInQueue()) {
    return false;
  }

  Task next_order = stn_->getNextOrder();
  double travel_time = std::get<0>(kinematics_->getStartStopTimeAndDistance(
      *last_position_, next_order.getPickupLocation(), false));

  double sim_time = (double)ns3::Simulator::Now().GetMilliSeconds();
  return next_order.time_window.getEarliestStart() - travel_time <= sim_time;
}

int STNTaskManagement::getNumberOfOrdersInQueue() const {
  return stn_->getNumberOfIncludedOrders();
}

bool STNTaskManagement::addOrder(Task order, const std::shared_ptr<InsertInfo> &info) {
  auto const &[position, time] = getEndPositionAndTime();
  stn_->setOriginInformation(position, time);

  order.setSpecificAbilityRequirement(*ability_);
  order.setOrderState(OrderStates::kQueued);
  stn_->setKinematics(*kinematics_);
  AccumulatedUtilityDimensions acc_udims_before = stn_->getCurrentAccumulatedUtilityDimensions();

  std::shared_ptr<SimpleTemporalNetwork> backup_stn =
      std::make_shared<SimpleTemporalNetwork>(*stn_);

  auto insert_info = std::static_pointer_cast<STNInsertInfo>(info);

  bool success = false;
  try {
    success = stn_->insertOrderByInsertInfo(order, insert_info);
  } catch (const std::logic_error &e) {
    return false;
  }

  if (!success) {
    stn_ = backup_stn;
    return false;
  }

  AccumulatedUtilityDimensions acc_udims_after = stn_->getCurrentAccumulatedUtilityDimensions();
  AccumulatedUtilityDimensions delta =
      AccumulatedUtilityDimensions::getDelta(acc_udims_before, acc_udims_after);
  stn_->setNewestUtilityDimensionDelta(delta);

  logger_->logTransportOrderUpdate(order, agv_uuid_);
  return true;
}

UtilityDimensions STNTaskManagement::getUtilityDimensions(const Task &order) {
  return std::get<0>(STNTaskManagement::getUtilityDimensionsAndInsertInfo(order));
}

std::pair<UtilityDimensions, std::shared_ptr<InsertInfo>>
STNTaskManagement::getUtilityDimensionsAndInsertInfo(const Task &order) {
  stn_->setKinematics(*kinematics_);
  std::shared_ptr<SimpleTemporalNetwork> copy_stn = std::make_shared<SimpleTemporalNetwork>(*stn_);

  AccumulatedUtilityDimensions acc_udims_before =
      copy_stn->getCurrentAccumulatedUtilityDimensions();

  Task copy_order = order;
  copy_order.setSpecificAbilityRequirement(*ability_);

  double scheduling_buffer = 2000;
  auto const &[position, time] = getEndPositionAndTime();
  copy_stn->setOriginInformation(position, time + scheduling_buffer);

  std::pair<UtilityDimensions, std::shared_ptr<STNInsertInfo>> result =
      copy_stn->insertOrder(copy_order);

  if (copy_stn->isConsistent()) {
    AccumulatedUtilityDimensions acc_udims_after =
        copy_stn->getCurrentAccumulatedUtilityDimensions();
    AccumulatedUtilityDimensions delta =
        AccumulatedUtilityDimensions::getDelta(acc_udims_before, acc_udims_after);

    std::get<0>(result).delta_execution_duration = delta.getAccumulatedExecutionDuration();
    std::get<0>(result).delta_execution_distance = delta.getAccumulatedExecutionDistance();
    std::get<0>(result).delta_travel_to_pickup_duration =
        delta.getAccumulatedTravelToPickupDuration();
    std::get<0>(result).delta_travel_to_pickup_distance =
        delta.getAccumulatedTravelToPickupDistance();
    std::get<0>(result).delta_makespan = delta.getMakespan();

    utility_evaluator_->reevaluate(std::get<0>(result));
  }

  return result;
}

bool STNTaskManagement::hasOrder(const std::string &order_uuid) {
  if (stn_->hasOrder(order_uuid)) {
    return true;
  }

  if (current_order_.getUUID() == order_uuid) {
    return true;
  }

  return false;
}

}  // namespace daisi::cpps
