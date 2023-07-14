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

#ifndef DAISI_CPPS_LOGICAL_ORDER_MANAGEMENT_STN_ORDER_MANAGEMENT_H_
#define DAISI_CPPS_LOGICAL_ORDER_MANAGEMENT_STN_ORDER_MANAGEMENT_H_

#include <memory>

#include "auction_based_order_management.h"
#include "cpps/amr/amr_mobility_helper.h"
#include "cpps/amr/physical/material_flow_functionality_mapping.h"
#include "datastructure/simple_temporal_network.tpp"
#include "stn_order_management_components.h"

namespace daisi::cpps::logical {

class StnOrderManagement
    : public AuctionBasedOrderManagement,
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

  ~StnOrderManagement() override = default;

  /// @brief check wether the order management has a current task assigned
  bool hasTasks() const override;

  /// @brief get the current task
  daisi::material_flow::Task getCurrentTask() const override;

  /// @brief set the current task to the first task that is currently queued
  /// @return true if the assignment was successful
  bool setNextTask() override;

  /// @brief check wether a new task can be assigned to the management
  /// @param task the task to be assigned
  /// @return true if the assignment can be made
  bool canAddTask(const daisi::material_flow::Task &task) override;

  /// @brief assign a new task to the management
  /// @param task the task to be assigned
  /// @param insertion_point the position in the managements queue where the task should be inserted
  /// @return true if the insertion was successful
  bool addTask(const daisi::material_flow::Task &task,
               std::shared_ptr<InsertionPoint> insertion_point = nullptr) override;
  /// @brief get infos about the latest task insertion (also includes the check wether a task can be
  /// inserted)
  /// @return the MetricsComposition and InsertionPoint of the latest task insertion
  std::pair<MetricsComposition, std::shared_ptr<InsertionPoint>> getLatestCalculatedInsertionInfo()
      const override;

  /// @brief set the management's current time
  /// @param now
  void setCurrentTime(const daisi::util::Duration &now);

  using VertexIterator = std::vector<StnOrderManagementVertex>::iterator;

  /// @brief contains a task and the end locations and metrics compositions for the single orders
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

  daisi::util::Duration current_task_expected_finish_time_ = 0.0;

  std::vector<TaskInsertInfo> current_ordering_;

  Metrics current_total_metrics_;

  std::vector<TaskInsertInfo>::iterator newest_task_insert_info_;

  daisi::util::Duration time_now_ = 0;

  std::optional<std::pair<MetricsComposition, std::shared_ptr<InsertionPoint>>>
      latest_calculated_insertion_info_;

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

  /// @brief update the metrics compositions of all currently queued tasks as well as the total
  /// metrics. sort the current ordering by start time. method is called after inserting a new task.
  void updateCurrentOrdering();

  daisi::util::Position getLastPositionBefore(int task_index);

  daisi::util::Duration calcOrderDurationForInsert(const daisi::material_flow::Order &order,
                                                   const TaskInsertInfo &task_insert_info) const;
  /// @brief calculate the metrics for the given order of the given task and insert them into the
  /// given metrics.
  /// @param order the order the metrics should be calculated for
  /// @param metrics the container to insert the metrics
  /// @param task_insert_info the TaskInsertInfo for the passed order
  /// @param task_ordering_index the current ordering index for the task
  /// @param start_time the start time of the order
  void insertOrderPropertiesIntoMetrics(const daisi::material_flow::Order &order, Metrics &metrics,
                                        const TaskInsertInfo &task_insert_info,
                                        const int &task_ordering_index,
                                        const daisi::util::Duration &start_time);

  // simple helper
  VertexIterator getVertexIteratorOfOrder(const daisi::material_flow::Order &order, bool start);
  int getVertexIndexOfOrder(const daisi::material_flow::Order &order, bool start);
  const StnOrderManagementVertex &getVertexOfOrder(const daisi::material_flow::Order &order,
                                                   bool start);

private:
  void updateOriginConstraints(const daisi::util::Duration &time_difference);
};

}  // namespace daisi::cpps::logical

#endif
