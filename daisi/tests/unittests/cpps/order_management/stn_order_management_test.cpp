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

#include "cpps/logical/order_management/stn_order_management.h"

#include <catch2/catch_test_macros.hpp>
#include <string>

using namespace daisi::material_flow;
using namespace daisi::cpps;
using namespace daisi::cpps::logical;
using namespace daisi::cpps::amr;

AmrDescription buildBasicAmrDescription() {
  AmrKinematics kinematics{1, 0, 1, 1};
  AmrProperties properties{};
  AmrPhysicalProperties physical_properties{50, {0.5, 0.5, 0.5}};
  AmrLoadHandlingUnit load_hanling_unit{
      7, 8, amr::AmrStaticAbility(LoadCarrier(LoadCarrier::kEuroBox), 50)};

  return AmrDescription{42, kinematics, properties, physical_properties, load_hanling_unit};
}

Topology buildBasicTopology() { return Topology{{20, 20, 0}}; }

daisi::util::Position p0(0, 0);
daisi::util::Position p1(10, 0);
daisi::util::Position p2(10, 10);
daisi::util::Position p3(0, 10);
daisi::util::Position p4(5, 10);
daisi::util::Position p5(20, 0);
daisi::util::Position p6(20, 10);

TEST_CASE("One Simple Transport Order", "[adding and removing vertices]") {
  // arrange
  auto current_pose = daisi::util::Pose(p0);
  StnOrderManagement management(buildBasicAmrDescription(), buildBasicTopology(), current_pose);

  // assert preconditions
  REQUIRE(!management.hasTasks());
  REQUIRE_THROWS(management.getCurrentTask());
  REQUIRE(!management.setNextTask());

  // one transport order without constraints
  TransportOrderStep pickup1("tos1", {}, Location("0x0", "type", p1));
  TransportOrderStep delivery1("tos2", {}, Location("0x1", "type", p2));
  TransportOrder simple_to("simple_to", {pickup1}, delivery1);
  Task simple_task("simple_task", {simple_to}, {});

  // act

  REQUIRE(management.canAddTask(simple_task));
  auto res1 = management.getLatestCalculatedInsertionInfo();
  auto metrics_comp = std::get<0>(res1);
  auto insertion_point = std::get<1>(res1);
  auto diff_metrics = metrics_comp.getDiffInsertionMetrics();

  // assert metrics results
  REQUIRE(diff_metrics.loaded_travel_time == 11);
  REQUIRE(diff_metrics.empty_travel_time == 11);
  REQUIRE(diff_metrics.action_time == 15);
  REQUIRE(diff_metrics.loaded_travel_distance == 10);
  REQUIRE(diff_metrics.empty_travel_distance == 10);

  REQUIRE(diff_metrics.getTime() == 37);
  REQUIRE(diff_metrics.getDistance() == 20);
  REQUIRE(diff_metrics.getMakespan() == 37);

  // assert intertion point

  std::shared_ptr<StnOrderManagement::StnInsertionPoint> stn_insertion_point =
      std::static_pointer_cast<StnOrderManagement::StnInsertionPoint>(insertion_point);

  REQUIRE(stn_insertion_point->new_index == 0);
  REQUIRE(stn_insertion_point->previous_finish.isOrigin());
  REQUIRE(stn_insertion_point->next_start == std::nullopt);

  // assert management states not changed
  REQUIRE(!management.hasTasks());
  REQUIRE_THROWS(management.getCurrentTask());
  REQUIRE(!management.setNextTask());

  // act
  REQUIRE(management.addTask(simple_task, insertion_point));
  auto add_metrics_comp = std::get<0>(management.getLatestCalculatedInsertionInfo());

  // assert
  REQUIRE(management.setNextTask());
  REQUIRE(management.hasTasks());
  auto task = management.getCurrentTask();
  REQUIRE(task.getName() == "simple_task");

  REQUIRE(diff_metrics == add_metrics_comp.getDiffInsertionMetrics());
  REQUIRE(diff_metrics == add_metrics_comp.getInsertionMetrics());

  REQUIRE(!management.setNextTask());
  REQUIRE(!management.hasTasks());
}

TEST_CASE("Two Simple Transport Orders statically", "[adding and removing vertices]") {
  // arrange
  auto current_pose = daisi::util::Pose(p0);
  StnOrderManagement management(buildBasicAmrDescription(), buildBasicTopology(), current_pose);

  // transport order 1 without constraints
  TransportOrderStep pickup_1("tos1_1", {}, Location("0x0", "type", p1));
  TransportOrderStep delivery_1("tos2_1", {}, Location("0x1", "type", p2));
  TransportOrder simple_to_1("simple_to_1", {pickup_1}, delivery_1);
  Task simple_task_1("simple_task_1", {simple_to_1}, {});

  // transport order 2 without constraints
  TransportOrderStep pickup_2("tos1_2", {}, Location("0x2", "type", p4));
  TransportOrderStep delivery_2("tos2_2", {}, Location("0x3", "type", p3));
  TransportOrder simple_to_2("simple_to_2", {pickup_2}, delivery_2);
  Task simple_task_2("simple_task_2", {simple_to_2}, {});

  // can add tests
  REQUIRE(management.canAddTask(simple_task_1));
  auto can_add_1_metrics_comp = std::get<0>(management.getLatestCalculatedInsertionInfo());

  REQUIRE(management.canAddTask(simple_task_2));
  auto can_add_2_metrics_comp = std::get<0>(management.getLatestCalculatedInsertionInfo());

  REQUIRE(can_add_1_metrics_comp.getCurrentMetrics().empty_travel_distance > 0);
  REQUIRE(can_add_1_metrics_comp.getCurrentMetrics().empty_travel_time > 0);
  REQUIRE(can_add_1_metrics_comp.getCurrentMetrics().loaded_travel_distance > 0);
  REQUIRE(can_add_1_metrics_comp.getCurrentMetrics().loaded_travel_time > 0);
  REQUIRE(can_add_1_metrics_comp.getCurrentMetrics().action_time > 0);
  REQUIRE(can_add_1_metrics_comp.getCurrentMetrics().getMakespan() > 0.01);

  REQUIRE(can_add_2_metrics_comp.getCurrentMetrics().empty_travel_distance > 0);
  REQUIRE(can_add_2_metrics_comp.getCurrentMetrics().empty_travel_time > 0);
  REQUIRE(can_add_2_metrics_comp.getCurrentMetrics().loaded_travel_distance > 0);
  REQUIRE(can_add_2_metrics_comp.getCurrentMetrics().loaded_travel_time > 0);
  REQUIRE(can_add_2_metrics_comp.getCurrentMetrics().action_time > 0);
  REQUIRE(can_add_2_metrics_comp.getCurrentMetrics().getMakespan() > 0.01);

  REQUIRE(can_add_1_metrics_comp.getCurrentMetrics().empty_travel_distance <
          can_add_2_metrics_comp.getCurrentMetrics().empty_travel_distance);
  REQUIRE(can_add_1_metrics_comp.getCurrentMetrics().empty_travel_time <
          can_add_2_metrics_comp.getCurrentMetrics().empty_travel_time);
  REQUIRE(can_add_1_metrics_comp.getCurrentMetrics().loaded_travel_distance >
          can_add_2_metrics_comp.getCurrentMetrics().loaded_travel_distance);
  REQUIRE(can_add_1_metrics_comp.getCurrentMetrics().loaded_travel_time >
          can_add_2_metrics_comp.getCurrentMetrics().loaded_travel_time);
  REQUIRE(can_add_1_metrics_comp.getCurrentMetrics().action_time ==
          can_add_2_metrics_comp.getCurrentMetrics().action_time);

  REQUIRE(can_add_1_metrics_comp.getCurrentMetrics().getMakespan() >
          can_add_2_metrics_comp.getCurrentMetrics().getMakespan());

  // adding task 1
  REQUIRE(!management.hasTasks());
  REQUIRE(management.addTask(simple_task_1));

  // can add task 2 after 1
  REQUIRE(management.canAddTask(simple_task_2));
  std::shared_ptr<StnOrderManagement::StnInsertionPoint> can_add_2_2_stn_insertion_point =
      std::static_pointer_cast<StnOrderManagement::StnInsertionPoint>(
          std::get<1>(management.getLatestCalculatedInsertionInfo()));
  REQUIRE(can_add_2_2_stn_insertion_point->new_index == 1);

  // adding task 2
  REQUIRE(management.addTask(simple_task_2));
  std::shared_ptr<StnOrderManagement::StnInsertionPoint> add_2_stn_insertion_point =
      std::static_pointer_cast<StnOrderManagement::StnInsertionPoint>(
          std::get<1>(management.getLatestCalculatedInsertionInfo()));
  REQUIRE(add_2_stn_insertion_point->new_index == 1);
  REQUIRE(!add_2_stn_insertion_point->next_start.has_value());

  // checking management states after insertions
  REQUIRE(!management.hasTasks());
  REQUIRE(management.setNextTask());
  auto first_task = management.getCurrentTask();
  REQUIRE(first_task.getName() == "simple_task_1");
  REQUIRE(management.hasTasks());

  REQUIRE(management.setNextTask());
  auto second_task = management.getCurrentTask();
  REQUIRE(second_task.getName() == "simple_task_2");
  REQUIRE(management.hasTasks());

  REQUIRE(!management.setNextTask());
  REQUIRE(!management.hasTasks());
}

TEST_CASE("Two Simple Transport Orders dynamically", "[adding and removing vertices]") {
  // arrange
  auto current_pose = daisi::util::Pose(p0);
  StnOrderManagement management(buildBasicAmrDescription(), buildBasicTopology(), current_pose);

  // transport order 1 without constraints
  TransportOrderStep pickup_1("tos1_1", {}, Location("0x0", "type", p1));
  TransportOrderStep delivery_1("tos2_1", {}, Location("0x1", "type", p2));
  TransportOrder simple_to_1("simple_to_1", {pickup_1}, delivery_1);
  Task simple_task_1("simple_task_1", {simple_to_1}, {});

  // transport order 2 without constraints
  TransportOrderStep pickup_2("tos1_2", {}, Location("0x2", "type", p4));
  TransportOrderStep delivery_2("tos2_2", {}, Location("0x3", "type", p3));
  TransportOrder simple_to_2("simple_to_2", {pickup_2}, delivery_2);
  Task simple_task_2("simple_task_2", {simple_to_2}, {});

  // add task 1 and setting it dynamically to the next task
  REQUIRE(management.addTask(simple_task_1));
  REQUIRE(management.setNextTask());
  REQUIRE(management.hasTasks());
  REQUIRE(management.getCurrentTask().getName() == "simple_task_1");
  auto add_1_info = management.getLatestCalculatedInsertionInfo();

  // adding task 2, however current ordering is now empty
  REQUIRE(management.canAddTask(simple_task_2));
  auto can_add_2_1_info = management.getLatestCalculatedInsertionInfo();

  // insertion point should be at the first point of the current ordering
  std::shared_ptr<StnOrderManagement::StnInsertionPoint> can_add_2_1_stn_insertion_point =
      std::static_pointer_cast<StnOrderManagement::StnInsertionPoint>(
          std::get<1>(can_add_2_1_info));
  REQUIRE(can_add_2_1_stn_insertion_point->new_index == 0);
  REQUIRE(can_add_2_1_stn_insertion_point->previous_finish.isOrigin());
  REQUIRE(!can_add_2_1_stn_insertion_point->next_start.has_value());

  // expected finish time of task 1 must be respected in the metrics
  auto can_add_2_1_metrics_comp = std::get<0>(can_add_2_1_info);
  auto add_1_metrics_comp = std::get<0>(add_1_info);
  REQUIRE(can_add_2_1_metrics_comp.getCurrentMetrics().getDistance() <
          add_1_metrics_comp.getCurrentMetrics().getDistance());
  REQUIRE(can_add_2_1_metrics_comp.getCurrentMetrics().getMakespan() >=
          add_1_metrics_comp.getCurrentMetrics().getMakespan() +
              can_add_2_1_metrics_comp.getCurrentMetrics().getTime());

  // increasing time such that task 1 should still be executed -> no change to can_add_2_1 expected
  management.setCurrentTime(10);
  REQUIRE(management.canAddTask(simple_task_2));
  std::shared_ptr<StnOrderManagement::StnInsertionPoint> can_add_2_2_stn_insertion_point =
      std::static_pointer_cast<StnOrderManagement::StnInsertionPoint>(
          std::get<1>(management.getLatestCalculatedInsertionInfo()));
  REQUIRE(can_add_2_2_stn_insertion_point->new_index == 0);

  auto can_add_2_2_metrics_comp = std::get<0>(management.getLatestCalculatedInsertionInfo());
  REQUIRE(can_add_2_1_metrics_comp.getCurrentMetrics().getMakespan() ==
          can_add_2_2_metrics_comp.getCurrentMetrics().getMakespan());

  // increasing time such that execution of task 1 should be over
  // therefore makespan must be greater
  management.setCurrentTime(50);
  REQUIRE(management.canAddTask(simple_task_2));
  auto can_add_2_3_metrics_comp = std::get<0>(management.getLatestCalculatedInsertionInfo());

  REQUIRE(can_add_2_3_metrics_comp.getCurrentMetrics().getMakespan() >
          can_add_2_2_metrics_comp.getCurrentMetrics().getMakespan());
  REQUIRE(can_add_2_3_metrics_comp.getCurrentMetrics().getMakespan() ==
          50 + can_add_2_2_metrics_comp.getCurrentMetrics().getTime());

  // finally adding it
  REQUIRE(management.addTask(simple_task_2));
}

TEST_CASE("Three Simple Transport Orders statically", "[adding and removing vertices]") {
  // arrange
  auto current_pose = daisi::util::Pose(p0);
  StnOrderManagement management(buildBasicAmrDescription(), buildBasicTopology(), current_pose);

  // transport order 1 without constraints
  TransportOrderStep pickup_1("tos1_1", {}, Location("0x0", "type", p3));
  TransportOrderStep delivery_1("tos2_1", {}, Location("0x1", "type", p2));
  TransportOrder simple_to_1("simple_to_1", {pickup_1}, delivery_1);
  Task simple_task_1("simple_task_1", {simple_to_1}, {});

  // transport order 2 without constraints
  TransportOrderStep pickup_2("tos1_2", {}, Location("0x2", "type", p6));
  TransportOrderStep delivery_2("tos2_2", {}, Location("0x3", "type", p5));
  TransportOrder simple_to_2("simple_to_2", {pickup_2}, delivery_2);
  Task simple_task_2("simple_task_2", {simple_to_2}, {});

  // transport order 3 without constraints
  TransportOrderStep pickup_3("tos1_3", {}, Location("0x4", "type", p1));
  TransportOrderStep delivery_3("tos2_3", {}, Location("0x5", "type", p0));
  TransportOrder simple_to_3("simple_to_3", {pickup_3}, delivery_3);
  Task simple_task_3("simple_task_3", {simple_to_3}, {});

  REQUIRE(management.addTask(simple_task_1));
  auto add_1 = management.getLatestCalculatedInsertionInfo();
  REQUIRE(management.addTask(simple_task_2));
  auto add_2 = management.getLatestCalculatedInsertionInfo();

  auto add_1_metrics_comp = add_1.first;
  auto add_2_metrics_comp = add_2.first;
  REQUIRE(add_1_metrics_comp.getCurrentMetrics().getDistance() ==
          add_2_metrics_comp.getCurrentMetrics().getDistance());
  REQUIRE(add_1_metrics_comp.getCurrentMetrics().getTime() ==
          add_2_metrics_comp.getCurrentMetrics().getTime());
  REQUIRE(add_2_metrics_comp.getCurrentMetrics().getMakespan() ==
          add_1_metrics_comp.getCurrentMetrics().getMakespan() +
              add_1_metrics_comp.getCurrentMetrics().getTime());

  std::shared_ptr<StnOrderManagement::StnInsertionPoint> add_1_stn_insertion_point =
      std::static_pointer_cast<StnOrderManagement::StnInsertionPoint>(std::get<1>(add_1));

  std::shared_ptr<StnOrderManagement::StnInsertionPoint> add_2_stn_insertion_point =
      std::static_pointer_cast<StnOrderManagement::StnInsertionPoint>(std::get<1>(add_2));
  REQUIRE(add_1_stn_insertion_point->new_index == 0);
  REQUIRE(add_2_stn_insertion_point->new_index == 1);

  // trying to add task 3, should come after task 2
  REQUIRE(management.canAddTask(simple_task_3));
  std::shared_ptr<StnOrderManagement::StnInsertionPoint> can_add_3_stn_insertion_point =
      std::static_pointer_cast<StnOrderManagement::StnInsertionPoint>(
          std::get<1>(management.getLatestCalculatedInsertionInfo()));
  REQUIRE((can_add_3_stn_insertion_point->new_index == 0 ||
           can_add_3_stn_insertion_point->new_index == 2));
}

TEST_CASE("Two Transport Orders in one Task", "[adding and removing vertices]") {
  // arrange
  auto current_pose = daisi::util::Pose(p0);
  StnOrderManagement management(buildBasicAmrDescription(), buildBasicTopology(), current_pose);

  // transport order 1 without constraints
  TransportOrderStep pickup_1("tos1_1", {}, Location("0x0", "type", p0));
  TransportOrderStep delivery_1("tos2_1", {}, Location("0x1", "type", p1));
  TransportOrder simple_to_1("simple_to_1", {pickup_1}, delivery_1);

  TransportOrderStep pickup_2("tos1_2", {}, Location("0x2", "type", p2));
  TransportOrderStep delivery_2("tos2_2", {}, Location("0x3", "type", p4));
  TransportOrder simple_to_2("simple_to_2", {pickup_2}, delivery_2);

  Task task_1("task_1", {simple_to_1, simple_to_2}, {});

  REQUIRE(management.addTask(task_1));
  auto add_1_metrics_comp = management.getLatestCalculatedInsertionInfo().first;
  REQUIRE(add_1_metrics_comp.getCurrentMetrics().empty_travel_distance == 10);
  REQUIRE(add_1_metrics_comp.getCurrentMetrics().empty_travel_distance == 10);
  REQUIRE(add_1_metrics_comp.getCurrentMetrics().getMakespan() == 58);
}

TEST_CASE("One Transport, Move, and Action Order in one Task", "[adding and removing vertices]") {
  // arrange
  auto current_pose = daisi::util::Pose(p0);
  StnOrderManagement management(buildBasicAmrDescription(), buildBasicTopology(), current_pose);

  TransportOrderStep pickup_1("tos1_1", {}, Location("0x0", "type", p0));
  TransportOrderStep delivery_1("tos2_1", {}, Location("0x1", "type", p1));
  TransportOrder to_1("to_1", {pickup_1}, delivery_1);

  MoveOrderStep mos("mos", {}, Location("0x2", "type", p2));
  MoveOrder mo_1("mo", mos);

  ActionOrderStep aos("aos", {{"load", "load"}});
  ActionOrder ao_1("ao", aos);

  Task task_1("task_1", {to_1, mo_1, ao_1}, {});

  REQUIRE(management.addTask(task_1));
  auto add_1_metrics_comp = management.getLatestCalculatedInsertionInfo().first;

  REQUIRE(add_1_metrics_comp.getCurrentMetrics().action_time == 15 + 7);
  REQUIRE(add_1_metrics_comp.getCurrentMetrics().empty_travel_distance == 10);
  REQUIRE(add_1_metrics_comp.getCurrentMetrics().loaded_travel_distance == 10);
}
