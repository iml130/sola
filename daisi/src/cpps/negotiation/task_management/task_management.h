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

#ifndef DAISI_NEGOTIATION_TASK_MANAGEMENT_NS3_H_
#define DAISI_NEGOTIATION_TASK_MANAGEMENT_NS3_H_

#include <memory.h>

#include <functional>
#include <memory>
#include <utility>
#include <vector>

#include "cpps/amr/model/amr_static_ability.h"
#include "cpps/common/cpps_logger_ns3.h"
#include "cpps/model/kinematics.h"
#include "cpps/model/task.h"
#include "cpps/negotiation/mrta_configuration.h"
#include "cpps/negotiation/task_management/insert_info.h"
#include "cpps/negotiation/utility/utility_evaluator.h"
#include "ns3/vector.h"

namespace daisi::cpps {

class TaskManagement {
public:
  TaskManagement(const std::shared_ptr<Kinematics> &kinematics,
                 const std::shared_ptr<amr::AmrStaticAbility> &ability,
                 const std::shared_ptr<ns3::Vector> &last_position,
                 const std::shared_ptr<daisi::cpps::CppsLoggerNs3> &logger,
                 const std::shared_ptr<UtilityEvaluator> &utility_evaluator);
  virtual ~TaskManagement() = default;

  const Task &getCurrentOrder() const;
  void setNextOrderToExecute();
  virtual void pickNextOrderToExecute() = 0;
  void updateCurrentOrderPosition();
  void setCurrentOrderStatus(const OrderStates &state);
  virtual bool hasOrdersInQueue() const = 0;
  virtual bool canExecuteNextOrder() const = 0;
  virtual int getNumberOfOrdersInQueue() const = 0;

  bool couldExecuteOrder(const Task &order);
  virtual bool addOrder(Task order, const std::shared_ptr<InsertInfo> &info) = 0;

  virtual UtilityDimensions getUtilityDimensions(const Task &order) = 0;
  virtual std::pair<UtilityDimensions, std::shared_ptr<InsertInfo>>
  getUtilityDimensionsAndInsertInfo(const Task &order) = 0;

  void setAgvUUID(const std::string &uuid);

  amr::AmrStaticAbility getAbility() const;

  virtual bool hasOrder(const std::string &order_uuid) = 0;

protected:
  virtual void logCurrentOrderExecution();

  std::pair<ns3::Vector, double> getEndPositionAndTime() const;

  double expected_current_order_finish_time_ = -1.0;

  const std::shared_ptr<Kinematics> &kinematics_;
  const std::shared_ptr<amr::AmrStaticAbility> &ability_;
  const std::shared_ptr<ns3::Vector> &last_position_;

  const std::shared_ptr<daisi::cpps::CppsLoggerNs3> &logger_;

  std::shared_ptr<UtilityEvaluator> utility_evaluator_;

  Task current_order_;
  UtilityDimensions current_utility_dimensions_;

  std::string agv_uuid_;
};

}  // namespace daisi::cpps

#endif
