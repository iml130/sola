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

#ifndef DAISI_CPPS_LOGICAL_TASK_MANAGEMENT_SIMPLE_TASK_MANAGEMENT_H_
#define DAISI_CPPS_LOGICAL_TASK_MANAGEMENT_SIMPLE_TASK_MANAGEMENT_H_

#include "cpps/amr/amr_mobility_helper.h"
#include "cpps/amr/physical/material_flow_functionality_mapping.h"
#include "task_management.h"

namespace daisi::cpps::logical {
class SimpleTaskManagement : public TaskManagement {
public:
  SimpleTaskManagement(const AmrDescription &amr_description, const Topology &topology,
                       const daisi::util::Pose &pose);

  ~SimpleTaskManagement() override = default;

  /// @brief return the metrics of the final order contained in the last task that has been added to
  /// the management
  Metrics getFinalMetrics() const;

  /// @brief return the end position after executing the final task in the queue
  daisi::util::Position getExpectedEndPosition() const;

  /// @brief check wether the task management has a current task assigned
  bool hasTasks() const override;

  /// @brief get the current task
  daisi::material_flow::Task getCurrentTask() const override;

  /// @brief replace the current task with the first task stored in the management's queue
  /// @return true if the new assignment of current task was successful, false if there were no more
  /// tasks in the management's queue
  bool setNextTask() override;

  /// @brief check wether a new task can be added to the management's queue
  bool canAddTask(const daisi::material_flow::Task &task) override;

  /// @brief insert a task into the management's queue. Update the current metrics and end location.
  /// @return true if the insertion was successful, false otherwise
  bool addTask(const daisi::material_flow::Task &task) override;

  void setCurrentTime(const daisi::util::Duration &now);

private:
  /// @brief update the current metrics by assuming that a new task has been inserted into the
  /// management's queue.
  /// Uses the metrics from the last order of the previous task.
  void updateFinalMetrics();

  /// @brief calculate the metrics for the given order of the given task and insert them into the
  /// given metrics.
  /// @param order the order the metrics should be calculated for
  /// @param metrics the container to insert the metrics
  /// @param task the task for the passed order
  /// @param start_time the start time of the order
  void insertOrderPropertiesIntoMetrics(const daisi::material_flow::Order &order, Metrics &metrics,
                                        const daisi::material_flow::Task &task,
                                        const daisi::util::Duration &start_time) const;

  /// @brief the currently active task
  std::optional<daisi::material_flow::Task> active_task_;

  /// @brief the management's queued tasks (without the current task)
  std::vector<daisi::material_flow::Task> queue_;

  /// @brief the metrics of the final order that has been added
  Metrics final_metrics_;

  /// @brief the final position after executing all currently known orders
  std::optional<daisi::util::Position> expected_end_position_;

  /// @brief the current time
  daisi::util::Duration time_now_ = 0;
};

}  // namespace daisi::cpps::logical

#endif
