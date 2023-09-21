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

#ifndef DAISI_CPPS_LOGICAL_TASK_MANAGEMENT_STN_TASK_MANAGEMENT_COMPONENTS_H_
#define DAISI_CPPS_LOGICAL_TASK_MANAGEMENT_STN_TASK_MANAGEMENT_COMPONENTS_H_

#include <optional>
#include <string>

#include "material_flow/model/task.h"
#include "metrics_composition.h"
#include "utils/structure_helpers.h"

namespace daisi::cpps::logical {

struct StnOrderManagementVertex {
  StnOrderManagementVertex(daisi::material_flow::Order order, bool is_start);

  static StnOrderManagementVertex createOrigin();

  const daisi::material_flow::Order &getOrder() const;
  bool isStart() const;
  bool isOrigin() const;

  friend bool operator==(const StnOrderManagementVertex &v1, const StnOrderManagementVertex &v2);

  void setLastPosition(const daisi::util::Position &position);

  const daisi::util::Position &getLastPosition() const;

  Metrics initial_insert_metrics;
  Metrics current_metrics;

private:
  daisi::material_flow::Order order_;
  bool is_start_;  // otherwise is finish
  bool is_origin_ = false;
  daisi::util::Position position_;
};

struct StnOrderManagementEdge {
  explicit StnOrderManagementEdge(const bool all_positive) : all_positive_(all_positive){};

  void addWeight(double weight) {
    if (weight >= 0 && all_positive_) {
      weights_.push_back(weight);
    } else if (weight <= 0 && !all_positive_) {
      weights_.push_back(weight);
    } else {
      throw std::invalid_argument("Weight does not fit to the edge type");
    }
  }

  double getWeight() const {
    if (all_positive_) {
      return *std::max_element(weights_.begin(), weights_.end());
    }
    return *std::min_element(weights_.begin(), weights_.end());
  }

  void removeLastWeight() { weights_.pop_back(); }

  int getNumberOfWeights() const { return weights_.size(); }

  void updateWeight(const int index, const double &weight) {
    if (index >= weights_.size()) {
      throw std::invalid_argument("Index out of range");
    }

    if (weight >= 0 && all_positive_) {
      weights_[index] = weight;
    } else if (weight < 0 && !all_positive_) {
      weights_[index] = weight;
    } else {
      throw std::invalid_argument("Weight does not fit to the edge type");
    }
  }

private:
  std::vector<double> weights_;
  bool all_positive_;
};

}  // namespace daisi::cpps::logical

namespace std {

template <> struct hash<daisi::cpps::logical::StnOrderManagementVertex> {
  std::size_t operator()(const daisi::cpps::logical::StnOrderManagementVertex &v) const {
    std::string repr;

    if (auto move_order_pval = std::get_if<daisi::material_flow::MoveOrder>(&v.getOrder())) {
      repr = move_order_pval->getUuid();
    } else if (auto action_order_pval =
                   std::get_if<daisi::material_flow::ActionOrder>(&v.getOrder())) {
      repr = action_order_pval->getUuid();
    } else if (auto transport_order_pval =
                   std::get_if<daisi::material_flow::TransportOrder>(&v.getOrder())) {
      repr = transport_order_pval->getUuid();
    } else {
      throw std::runtime_error("Order type not supported");
    }

    repr += v.isStart() ? "start" : "finish";
    return hash<string>()(repr);
  }
};

}  // namespace std

#endif
