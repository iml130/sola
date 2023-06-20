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

#ifndef DAISI_NEGOTIATION_GREEDY_TSP_LIST_TASK_MANAGEMENT_NS3_H_
#define DAISI_NEGOTIATION_GREEDY_TSP_LIST_TASK_MANAGEMENT_NS3_H_

#include "cpps/negotiation/task_management/basic_list_task_management.h"

namespace daisi::cpps {

class GreedyTSPListTaskManagement : public BasicListTaskManagement {
public:
  GreedyTSPListTaskManagement(const std::shared_ptr<Kinematics> &kinematics,
                              const std::shared_ptr<amr::AmrStaticAbility> &ability,
                              const std::shared_ptr<ns3::Vector> &last_position,
                              const std::shared_ptr<daisi::cpps::CppsLoggerNs3> &logger,
                              const std::shared_ptr<UtilityEvaluator> &utility_evaluator);

  virtual void updateQueue(std::list<std::pair<Task, UtilityDimensions>>::iterator &it) override;
  virtual UtilityDimensions getUtilityDimensions(const Task &order) override;
  virtual std::pair<UtilityDimensions, std::shared_ptr<InsertInfo>>
  getUtilityDimensionsAndInsertInfo(const Task &order) override;

  virtual bool hasOrder(const std::string &order_uuid) override { return false; }

private:
  std::vector<double> getMarginalCosts(const Task &order);
};

}  // namespace daisi::cpps

#endif
