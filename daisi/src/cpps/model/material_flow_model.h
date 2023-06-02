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

#ifndef DAISI_MODEL_MATERIAL_FLOW_MODEL_NS3_H_
#define DAISI_MODEL_MATERIAL_FLOW_MODEL_NS3_H_

#include "cpps/negotiation/utils/precedence_graph.h"
#include "manager/scenariofileparser.h"

namespace daisi::cpps {

struct MaterialFlowModel : public PrecedenceGraph {
public:
  MaterialFlowModel();

  void setOrderState(const std::string &order_uuid, const OrderStates &new_state);

  void loadFromScenarioTable(const std::shared_ptr<ScenariofileParser::Table> &model_description,
                             const std::vector<ns3::Vector> &locations,
                             const double &simulation_time);

private:
  bool loaded_;

  std::vector<ns3::Vector> random_locations_;
};

}  // namespace daisi::cpps

#endif
