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

#include "cpps/model/material_flow_model.h"

#include "cpps/model/ability.h"
#include "cpps/model/task.h"
#include "ns3/vector.h"
#include "utils/random_engine.h"

namespace daisi::cpps {

MaterialFlowModel::MaterialFlowModel() : loaded_(false) {}

void MaterialFlowModel::loadFromScenarioTable(
    const std::shared_ptr<ScenariofileParser::Table> &model_description,
    const std::vector<ns3::Vector> &locations, const double &simulation_time) {
  std::vector<ns3::Vector> random_locations = locations;
  std::shuffle(std::begin(random_locations), std::end(random_locations),
               daisi::global_random_engine);

  std::unordered_map<std::string, Task> prec_map;
  for (const auto &task_inner : model_description->content) {
    std::string task_name = task_inner.first;
    auto task_desc =
        model_description->getRequired<std::shared_ptr<ScenariofileParser::Table>>(task_name);

    int pickup_index = task_desc->getRequired<uint64_t>("pickup");
    int delivery_index = task_desc->getRequired<uint64_t>("delivery");

    mrta::model::LoadCarrier load_carrier(task_desc->getRequired<std::string>("load_carrier"));
    float payload_weight = task_desc->getRequired<float>("payload_weight");
    mrta::model::Ability ability{payload_weight, load_carrier};

    Task task(random_locations[pickup_index], random_locations[delivery_index], ability);

    float earliest_start = task_desc->getRequired<float>("earliest_start");
    float latest_finish = task_desc->getRequired<float>("latest_finish");
    task.time_window = TimeWindow(earliest_start, latest_finish);
    task.time_window.setSpawnTime(simulation_time);

    task.setName(task_name);
    prec_map[task_inner.first] = task;
  }

  for (const auto &task_inner : model_description->content) {
    std::string task_name = task_inner.first;
    auto task_desc =
        model_description->getRequired<std::shared_ptr<ScenariofileParser::Table>>(task_name);

    auto prec_constr = task_desc->getOptional<std::shared_ptr<ScenariofileParser::Table>>(
        "precedence_constraints");
    if (prec_constr) {
      Task &task = prec_map[task_name];
      for (auto &prec_inner : prec_constr->get()->content) {
        std::string prec_task_name = *std::get_if<std::string>(&prec_inner.second);
        task.precedence_constraints.addConstraintUUID(prec_map[prec_task_name].getUUID());
      }
    }
  }

  for (auto &[key, task] : prec_map) {
    addVertex(task);
  }

  for (const auto &task_inner : model_description->content) {
    std::string task_name = task_inner.first;

    auto task_desc =
        model_description->getRequired<std::shared_ptr<ScenariofileParser::Table>>(task_name);
    auto prec_constr = task_desc->getOptional<std::shared_ptr<ScenariofileParser::Table>>(
        "precedence_constraints");
    if (prec_constr) {
      const Task &task = prec_map[task_name];
      for (auto &prec_inner : prec_constr->get()->content) {
        std::string prec_task_name = *std::get_if<std::string>(&prec_inner.second);

        const Task &prec_task = prec_map[prec_task_name];
        addEdge(prec_task, task);
      }
    }
  }

  loaded_ = true;
}

void MaterialFlowModel::setOrderState(const std::string &order_uuid, const OrderStates &new_state) {
  auto it = std::find_if(vertices_.begin(), vertices_.end(),
                         [&order_uuid](const Task &o) { return o.getUUID() == order_uuid; });
  if (it == vertices_.end()) {
    throw std::runtime_error("Task to update not in MF Model");
  }

  it->setOrderState(new_state);
}

}  // namespace daisi::cpps
