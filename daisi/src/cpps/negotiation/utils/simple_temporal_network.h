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

#ifndef DAISI_NEGOTIATION_UTILS_SIMPLE_TEMPORAL_NETWORK_NS3_H_
#define DAISI_NEGOTIATION_UTILS_SIMPLE_TEMPORAL_NETWORK_NS3_H_

#include <cassert>
#include <functional>
#include <memory>
#include <optional>

#include "cpps/model/kinematics.h"
#include "cpps/model/task.h"
#include "cpps/negotiation/task_management/insert_info.h"
#include "cpps/negotiation/utility/accumulated_utility_dimensions.h"
#include "cpps/negotiation/utility/utility_evaluator.h"
#include "cpps/negotiation/utils/directed_graph.h"
#include "cpps/negotiation/utils/simple_temporal_network_components.h"
#include "ns3/vector.h"

namespace daisi::cpps {

class SimpleTemporalNetwork : public DirectedGraph<STN_Vertex, STN_Edge> {
public:
  SimpleTemporalNetwork();
  ~SimpleTemporalNetwork() = default;
  explicit SimpleTemporalNetwork(const std::shared_ptr<UtilityEvaluator> &utility_evaluator);

  std::pair<UtilityDimensions, std::shared_ptr<STNInsertInfo>> insertOrder(Task order);
  std::pair<Task, UtilityDimensions> removeOrder(const std::string &order_uuid);

  bool insertOrderByInsertInfo(Task order, const std::shared_ptr<STNInsertInfo> &insert_info);

  void insertOrderWithTimeWindows(const Task &order);
  void insertPrecedenceConstraintsOnPath(const std::vector<Task> &path);

  void generateDGraph();
  bool isConsistent();

  void setOriginInformation(const ns3::Vector &position, const double &time);

  void updateUtilityDimensions(const std::shared_ptr<STNInsertInfo> &insert_info);

  UtilityDimensions calcUtilityDimensions(const Task &order);

  std::string getNextOrderUUID() const;
  Task getNextOrder() const;
  int getNumberOfIncludedOrders() const;

  std::vector<std::tuple<std::string, double>> generateEarliestStartSolution();
  std::vector<std::tuple<std::string, double>> generateEarliestFinishSolution();
  std::vector<std::tuple<std::string, double>> generateLatestFinishSolution();

  void setKinematics(const Kinematics &kinematics);

  AccumulatedUtilityDimensions getCurrentAccumulatedUtilityDimensions() const;
  void setNewestUtilityDimensionDelta(const AccumulatedUtilityDimensions &delta);

  double getMakespan();

  bool hasOrder(const std::string &order_uuid);

private:
  void initOrigin();

  void insertTimeWindowConstraints(const STN_Vertex &start, const STN_Vertex &finish, Task order,
                                   std::optional<double> required_finish_time = std::nullopt);
  std::pair<UtilityDimensions, std::shared_ptr<STNInsertInfo>> insertTravelConstraints(
      const STN_Vertex &start, const STN_Vertex &finish, Task &order);

  bool insertTravelConstraintsBetween(const STN_Vertex &finish_before,
                                      const STN_Vertex &start_after, const STN_Vertex &start,
                                      const STN_Vertex &finish, Task &order);
  void removeTravelConstraintsBetween(const STN_Vertex &finish_before,
                                      const STN_Vertex &start_after, const STN_Vertex &start,
                                      const STN_Vertex &finish);

  void insertPrecedenceConstraints(const STN_Vertex &start, Task order);

  void removeOrderUUIDFromOrdering(const std::string &uuid);

  std::vector<STN_Vertex> getStartVertices() const;
  std::vector<STN_Vertex> getFinishVertices() const;

  void updateOriginEdges(const double &offset_difference);

  std::vector<std::pair<std::string, double>> calcOrderingAndStartTimes();

  const std::string origin_id_ = "origin";
  STN_Vertex origin_;

  const std::function<double(const ns3::Vector &x1, const ns3::Vector &x2, const bool execution)>
      get_travel_duration_;
  const std::function<double(const ns3::Vector &x1, const ns3::Vector &x2)> get_travel_distance_;

  std::optional<Kinematics> kinematics_;
  const std::shared_ptr<UtilityEvaluator> utility_evaluator_;

  // at last position is the newest order
  std::vector<std::pair<Task, UtilityDimensions>> included_orders_;

  // uuids and start_time
  std::vector<std::pair<std::string, double>> current_ordering_;
  void generateCurrentUtilityDimensions();

  std::vector<std::vector<double>> d_graph_;
};

}  // namespace daisi::cpps

#endif
