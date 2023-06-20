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

#ifndef DAISI_NEGOTIATION_STN_TASK_MANAGEMENT_NS3_H_
#define DAISI_NEGOTIATION_STN_TASK_MANAGEMENT_NS3_H_

#include <cmath>
#include <cstdint>
#include <vector>

#include "cpps/negotiation/task_management/task_management.h"
#include "cpps/negotiation/utils/simple_temporal_network.h"

namespace daisi::cpps {

class STNTaskManagement : public TaskManagement {
public:
  STNTaskManagement(const std::shared_ptr<Kinematics> &kinematics,
                    const std::shared_ptr<amr::AmrStaticAbility> &ability,
                    const std::shared_ptr<ns3::Vector> &last_position,
                    const std::shared_ptr<daisi::cpps::CppsLoggerNs3> &logger,
                    const std::shared_ptr<UtilityEvaluator> &utility_evaluator);

  virtual void pickNextOrderToExecute() override;
  virtual bool hasOrdersInQueue() const override;
  virtual bool canExecuteNextOrder() const override;
  virtual int getNumberOfOrdersInQueue() const override;
  virtual bool addOrder(Task order, const std::shared_ptr<InsertInfo> &info) override;
  virtual UtilityDimensions getUtilityDimensions(const Task &order) override;
  virtual std::pair<UtilityDimensions, std::shared_ptr<InsertInfo>>
  getUtilityDimensionsAndInsertInfo(const Task &order) override;

  virtual bool hasOrder(const std::string &order_uuid) override;

protected:
  std::shared_ptr<SimpleTemporalNetwork> stn_;
};

}  // namespace daisi::cpps

#endif
