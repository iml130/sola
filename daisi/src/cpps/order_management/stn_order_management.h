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

#ifndef DAISI_CPPS_ORDER_MANAGEMENT_STN_ORDER_MANAGEMENT_H_
#define DAISI_CPPS_ORDER_MANAGEMENT_STN_ORDER_MANAGEMENT_H_

#include <memory>

#include "cpps/amr/amr_mobility_helper.h"
#include "cpps/amr/physical/material_flow_functionality_mapping.h"
#include "datastructure/simple_temporal_network.h"
#include "order_management.h"
#include "stn_order_management_components.h"

namespace daisi::cpps::order_management {

class StnOrderManagement
    : public OrderManagement,
      private daisi::datastructure::SimpleTemporalNetwork<StnOrderManagementVertex,
                                                          StnOrderManagementEdge> {
public:
  struct StnInsertionPoint : public InsertionPoint {
    StnOrderManagementVertex previous_finish;
    std::optional<StnOrderManagementVertex> next_start;
    int new_index;
  };

  StnOrderManagement(const AmrDescription &amr_description, const Topology &topology,
                     const daisi::util::Pose &pose);

  ~StnOrderManagement() = default;

  bool hasTasks() const override;
  daisi::material_flow::Task getCurrentTask() const override;
  bool setNextTask() override;

  std::optional<std::pair<MetricsComposition, std::shared_ptr<InsertionPoint>>> canAddTask(
      const daisi::material_flow::Task &task) const override;
  std::optional<std::pair<MetricsComposition, std::shared_ptr<InsertionPoint>>> addTask(
      const daisi::material_flow::Task &task,
      std::shared_ptr<InsertionPoint> insertion_point = nullptr) override;

  void setCurrentTime(const daisi::util::Duration &now);

  using VertexIterator = std::vector<StnOrderManagementVertex>::iterator;

  struct TaskInsertInfo {
    daisi::material_flow::Task task;

    // for each order their location at the end of their execution
    std::vector<daisi::material_flow::Location> end_locations;

    MetricsComposition metrics_composition;
  };

protected:
  /// the current task was previously at the first position of the current ordering
  /// however to not get confused with insertion, the current task is kept separate from the
  /// ordering vector
  std::optional<daisi::material_flow::Task> current_task_;

  std::optional<daisi::material_flow::Location> current_task_end_location_;

  daisi::util::Duration current_task_expected_finish_time_;

  std::vector<TaskInsertInfo> current_ordering_;

  Metrics current_total_metrics_;

  std::vector<TaskInsertInfo>::iterator newest_task_insert_info_;

  daisi::util::Duration time_now_;

  bool solve() override;

  void addPrecedenceConstraintBetweenTask(const StnOrderManagementVertex &start_vertex,
                                          const std::string &precedence_task_name);

  void addDurationConstraints(const StnOrderManagementVertex &start_vertex,
                              const StnOrderManagementVertex &finish_vertex,
                              const daisi::material_flow::Order &order,
                              const TaskInsertInfo &task_insert_info);

  void updateDurationConstraints(const int &task_index_to_update);

  void addOrderingConstraintBetweenTasks(StnInsertionPoint insertion_point,
                                         TaskInsertInfo &task_insert_info);

  std::optional<std::pair<MetricsComposition, std::shared_ptr<StnInsertionPoint>>> addBestOrdering(
      StnOrderManagement::TaskInsertInfo &task_insert_info);

  std::vector<StnInsertionPoint> calcInsertionPoints();

  void updateCurrentOrdering();

  daisi::util::Position getLastPositionBefore(int task_index);

  daisi::util::Duration calcOrderDurationForInsert(const daisi::material_flow::Order &order,
                                                   const TaskInsertInfo &task_insert_info) const;

  void insertOrderPropertiesIntoMetrics(const daisi::material_flow::Order &order, Metrics &metrics,
                                        const TaskInsertInfo &task_insert_info,
                                        const int &task_ordering_index,
                                        const daisi::util::Duration &start_time);

  static std::optional<daisi::material_flow::Location> getEndLocationOfOrder(
      const daisi::material_flow::Order &order);

  // simple helper
  VertexIterator getVertexIteratorOfOrder(const daisi::material_flow::Order &order, bool start);
  int getVertexIndexOfOrder(const daisi::material_flow::Order &order, bool start);
  const StnOrderManagementVertex &getVertexOfOrder(const daisi::material_flow::Order &order,
                                                   bool start);

private:
  void updateOriginConstraints(const daisi::util::Duration &time_difference);
};

}  // namespace daisi::cpps::order_management

#endif
