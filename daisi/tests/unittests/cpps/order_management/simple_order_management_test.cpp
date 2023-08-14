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

#include "cpps/logical/order_management/simple_order_management.h"

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
  AmrLoadHandlingUnit load_handling_unit{7, 8,
                                         AmrStaticAbility(LoadCarrier(LoadCarrier::kEuroBox), 50)};

  return AmrDescription{42, kinematics, properties, physical_properties, load_handling_unit};
}

Topology buildBasicTopology() { return Topology{{20, 20, 0}}; }

daisi::util::Position p0(0, 0);
daisi::util::Position p1(10, 0);
daisi::util::Position p2(10, 10);
daisi::util::Position p3(0, 10);
daisi::util::Position p4(5, 10);
daisi::util::Position p5(20, 0);
daisi::util::Position p6(20, 10);

TEST_CASE("SimpleOrderManagement Empty Tasks", "[adding and removing vertices]") {
  // arrange
  auto current_pose = daisi::util::Pose(daisi::util::Position(10, 10));
  SimpleOrderManagement management(buildBasicAmrDescription(), buildBasicTopology(), current_pose);

  // assert preconditions
  REQUIRE(!management.hasTasks());
  REQUIRE_THROWS(management.getCurrentTask());
  REQUIRE(!management.setNextTask());

  // empty task
  Task task_empty("task1", "127.0.0.1:5000", {}, {});
  REQUIRE_THROWS(management.canAddTask(task_empty));
  REQUIRE_THROWS(management.addTask(task_empty));

  // empty task with follow ups
  Task task_empty_with_follow_ups("task2", "127.0.0.1:5000", {}, {"task1", "task3"});
  REQUIRE_THROWS(management.canAddTask(task_empty_with_follow_ups));
  REQUIRE_THROWS(management.addTask(task_empty_with_follow_ups));
}

TEST_CASE("SimpleOrderManagement One Simple Transport Order", "[adding and removing vertices]") {
  // arrange
  auto current_pose = daisi::util::Pose(p0);
  SimpleOrderManagement management(buildBasicAmrDescription(), buildBasicTopology(), current_pose);

  // assert preconditions
  REQUIRE(!management.hasTasks());
  REQUIRE_THROWS(management.getCurrentTask());
  REQUIRE(!management.setNextTask());

  // one transport order
  TransportOrderStep pickup1("tos1", {}, Location("0x0", "type", p1));
  TransportOrderStep delivery1("tos2", {}, Location("0x1", "type", p2));
  TransportOrder simple_to("simple_to", {pickup1}, delivery1);
  Task simple_task("simple_task", "127.0.0.1:5000", {simple_to}, {});

  // act
  auto opt_result = management.canAddTask(simple_task);
  REQUIRE(opt_result);
  auto metrics = management.getFinalMetrics();
  REQUIRE(metrics.getTime() == 0);
  REQUIRE(metrics.loaded_travel_time == 0);
  REQUIRE(metrics.empty_travel_time == 0);
  REQUIRE(metrics.action_time == 0);
  REQUIRE(metrics.loaded_travel_distance == 0);
  REQUIRE(metrics.empty_travel_distance == 0);
  REQUIRE(metrics.getDistance() == 0);
  REQUIRE(metrics.getMakespan() == 0);

  // assert management states not changed
  REQUIRE(!management.hasTasks());
  REQUIRE_THROWS(management.getCurrentTask());
  REQUIRE(!management.setNextTask());

  // act
  auto add_results = management.addTask(simple_task);
  REQUIRE(add_results);
  metrics = management.getFinalMetrics();
  // assert metrics results
  REQUIRE(metrics.loaded_travel_time == 11);
  REQUIRE(metrics.empty_travel_time == 11);
  REQUIRE(metrics.action_time == 15);
  REQUIRE(metrics.loaded_travel_distance == 10);
  REQUIRE(metrics.empty_travel_distance == 10);
  REQUIRE(metrics.getTime() == 37);
  REQUIRE(metrics.getDistance() == 20);
  REQUIRE(metrics.getMakespan() == 37);

  // assert
  REQUIRE(management.setNextTask());
  REQUIRE(management.hasTasks());
  auto task = management.getCurrentTask();
  REQUIRE(task.getName() == "simple_task");

  REQUIRE(!management.setNextTask());
  REQUIRE(!management.hasTasks());
}

TEST_CASE("SimpleOrderManagement Two Simple Transport Orders", "[adding and removing vertices]") {
  // arrange
  auto current_pose = daisi::util::Pose(p0);
  SimpleOrderManagement management(buildBasicAmrDescription(), buildBasicTopology(), current_pose);

  // transport order 1
  TransportOrderStep pickup_1("tos1_1", {}, Location("0x0", "type", p1));
  TransportOrderStep delivery_1("tos2_1", {}, Location("0x1", "type", p2));
  TransportOrder simple_to_1("simple_to_1", {pickup_1}, delivery_1);
  Task simple_task_1("simple_task_1", "127.0.0.1:5000", {simple_to_1}, {});

  // transport order 2
  TransportOrderStep pickup_2("tos1_2", {}, Location("0x2", "type", p4));
  TransportOrderStep delivery_2("tos2_2", {}, Location("0x3", "type", p3));
  TransportOrder simple_to_2("simple_to_2", {pickup_2}, delivery_2);
  Task simple_task_2("simple_task_2", "127.0.0.1:5000", {simple_to_2}, {});

  // can add tests
  auto can_add_1 = management.canAddTask(simple_task_1);
  REQUIRE(can_add_1);

  auto can_add_2 = management.canAddTask(simple_task_2);
  REQUIRE(can_add_2);
  auto metrics = management.getFinalMetrics();
  REQUIRE(metrics.getTime() == 0);
  REQUIRE(metrics.loaded_travel_time == 0);
  REQUIRE(metrics.empty_travel_time == 0);
  REQUIRE(metrics.action_time == 0);
  REQUIRE(metrics.loaded_travel_distance == 0);
  REQUIRE(metrics.empty_travel_distance == 0);
  REQUIRE(metrics.getDistance() == 0);
  REQUIRE(metrics.getMakespan() == 0);

  // adding task 1
  REQUIRE(!management.hasTasks());
  auto add_1_result = management.addTask(simple_task_1);
  REQUIRE(add_1_result);

  auto add_1_metrics = management.getFinalMetrics();

  // can add task 2 after 1
  auto can_add_2_2 = management.canAddTask(simple_task_2);
  REQUIRE(can_add_2_2);

  // adding task 2
  auto add_2_result = management.addTask(simple_task_2);
  REQUIRE(add_2_result);

  auto add_2_metrics = management.getFinalMetrics();
  REQUIRE(add_2_metrics.loaded_travel_time == 6);
  REQUIRE(add_2_metrics.empty_travel_time == 6);
  REQUIRE(add_2_metrics.action_time == 15);
  REQUIRE(add_2_metrics.loaded_travel_distance == 5);
  REQUIRE(add_2_metrics.empty_travel_distance == 5);
  REQUIRE(add_2_metrics.getTime() == 27);
  REQUIRE(add_2_metrics.getDistance() == 10);
  REQUIRE(add_2_metrics.getMakespan() == 64);

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

TEST_CASE("SimpleOrderManagement Two Simple Transport Orders multiple times",
          "[adding and removing vertices]") {
  // arrange
  auto current_pose = daisi::util::Pose(p0);
  SimpleOrderManagement management(buildBasicAmrDescription(), buildBasicTopology(), current_pose);

  // transport order 1
  TransportOrderStep pickup_1("tos1_1", {}, Location("0x0", "type", p1));
  TransportOrderStep delivery_1("tos2_1", {}, Location("0x1", "type", p2));
  TransportOrder simple_to_1("simple_to_1", {pickup_1}, delivery_1);
  Task simple_task_1("simple_task_1", "127.0.0.1:5000", {simple_to_1}, {});

  // transport order 2
  TransportOrderStep pickup_2("tos1_2", {}, Location("0x2", "type", p4));
  TransportOrderStep delivery_2("tos2_2", {}, Location("0x3", "type", p3));
  TransportOrder simple_to_2("simple_to_2", {pickup_2}, delivery_2);
  Task simple_task_2("simple_task_2", "127.0.0.1:5000", {simple_to_2}, {});

  // adding and removing task 2
  REQUIRE(!management.hasTasks());
  auto add_2_1_result = management.addTask(simple_task_2);
  REQUIRE(add_2_1_result);
  auto add_2_1_metrics = management.getFinalMetrics();
  REQUIRE(management.setNextTask());
  auto second_task = management.getCurrentTask();
  REQUIRE(second_task.getName() == "simple_task_2");
  REQUIRE(management.hasTasks());
  REQUIRE(!management.setNextTask());
  REQUIRE(!management.hasTasks());

  // adding task 1
  auto add_1_result = management.addTask(simple_task_1);
  REQUIRE(add_1_result);

  auto add_1_metrics = management.getFinalMetrics();
  auto makespan_task_1 = add_1_metrics.getMakespan();

  // makespan of task 1 must depend on the makespan of task 2
  REQUIRE(makespan_task_1 >= add_2_1_metrics.getMakespan() + add_1_metrics.getTime());

  // can add task 2 after 1
  auto can_add_2_2 = management.canAddTask(simple_task_2);
  REQUIRE(can_add_2_2);

  // adding task 2
  auto add_2_2_result = management.addTask(simple_task_2);
  REQUIRE(add_2_2_result);

  auto add_2_2_metrics = management.getFinalMetrics();
  REQUIRE(add_2_2_metrics.loaded_travel_time == 6);
  REQUIRE(add_2_2_metrics.empty_travel_time == 6);
  REQUIRE(add_2_2_metrics.action_time == 15);
  REQUIRE(add_2_2_metrics.loaded_travel_distance == 5);
  REQUIRE(add_2_2_metrics.empty_travel_distance == 5);
  REQUIRE(add_2_2_metrics.getTime() == 27);
  REQUIRE(add_2_2_metrics.getDistance() == 10);
  REQUIRE(add_2_2_metrics.getMakespan() == makespan_task_1 + 27);

  // compare with metrics of task 2 from before inserting task 1
  REQUIRE(add_2_2_metrics.empty_travel_distance < add_2_1_metrics.empty_travel_distance);
  REQUIRE(add_2_2_metrics.empty_travel_time < add_2_1_metrics.empty_travel_time);
  REQUIRE(add_2_2_metrics.loaded_travel_distance == add_2_1_metrics.loaded_travel_distance);
  REQUIRE(add_2_2_metrics.loaded_travel_time == add_2_1_metrics.loaded_travel_time);
  REQUIRE(add_2_2_metrics.action_time == add_2_1_metrics.action_time);
  REQUIRE(add_2_2_metrics.getMakespan() > add_2_1_metrics.getMakespan());

  // checking management states after insertions
  REQUIRE(!management.hasTasks());
  REQUIRE(management.setNextTask());
  auto first_task = management.getCurrentTask();
  REQUIRE(first_task.getName() == "simple_task_1");
  REQUIRE(management.hasTasks());

  REQUIRE(management.setNextTask());
  auto second_task_2 = management.getCurrentTask();
  REQUIRE(second_task_2.getName() == "simple_task_2");
  REQUIRE(management.hasTasks());

  REQUIRE(!management.setNextTask());
  REQUIRE(!management.hasTasks());
}

TEST_CASE("SimpleOrderManagement Three Simple Transport Orders", "[adding and removing vertices]") {
  // arrange
  auto current_pose = daisi::util::Pose(p0);
  SimpleOrderManagement management(buildBasicAmrDescription(), buildBasicTopology(), current_pose);

  // transport order 1
  TransportOrderStep pickup_1("tos1_1", {}, Location("0x0", "type", p3));
  TransportOrderStep delivery_1("tos2_1", {}, Location("0x1", "type", p2));
  TransportOrder simple_to_1("simple_to_1", {pickup_1}, delivery_1);
  Task simple_task_1("simple_task_1", "127.0.0.1:5000", {simple_to_1}, {});

  // transport order 2
  TransportOrderStep pickup_2("tos1_2", {}, Location("0x2", "type", p6));
  TransportOrderStep delivery_2("tos2_2", {}, Location("0x3", "type", p5));
  TransportOrder simple_to_2("simple_to_2", {pickup_2}, delivery_2);
  Task simple_task_2("simple_task_2", "127.0.0.1:5000", {simple_to_2}, {});

  // transport order 3
  TransportOrderStep pickup_3("tos1_3", {}, Location("0x4", "type", p1));
  TransportOrderStep delivery_3("tos2_3", {}, Location("0x5", "type", p0));
  TransportOrder simple_to_3("simple_to_3", {pickup_3}, delivery_3);
  Task simple_task_3("simple_task_3", "127.0.0.1:5000", {simple_to_3}, {});

  auto add_1_opt = management.addTask(simple_task_1);
  auto add_1_metrics = management.getFinalMetrics();
  auto add_2_opt = management.addTask(simple_task_2);
  auto add_2_metrics = management.getFinalMetrics();

  REQUIRE(add_1_opt);
  REQUIRE(add_2_opt);

  REQUIRE(add_1_metrics.getDistance() == add_2_metrics.getDistance());
  REQUIRE(add_1_metrics.getTime() == add_2_metrics.getTime());
  REQUIRE(add_2_metrics.getMakespan() == add_1_metrics.getMakespan() + add_2_metrics.getTime());

  // add task 3, should come after task 2
  auto can_add_3_opt = management.canAddTask(simple_task_3);
  REQUIRE(can_add_3_opt);
  auto add_3_opt = management.addTask(simple_task_3);
  auto add_3_metrics = management.getFinalMetrics();

  REQUIRE(add_3_metrics.getMakespan() == add_2_metrics.getMakespan() + add_3_metrics.getTime());
}

TEST_CASE("SimpleOrderManagement Two Transport Orders in one Task",
          "[adding and removing vertices]") {
  // arrange
  auto current_pose = daisi::util::Pose(p0);
  SimpleOrderManagement management(buildBasicAmrDescription(), buildBasicTopology(), current_pose);

  // transport order 1
  TransportOrderStep pickup_1("tos1_1", {}, Location("0x0", "type", p0));
  TransportOrderStep delivery_1("tos2_1", {}, Location("0x1", "type", p1));
  TransportOrder simple_to_1("simple_to_1", {pickup_1}, delivery_1);

  TransportOrderStep pickup_2("tos1_2", {}, Location("0x2", "type", p2));
  TransportOrderStep delivery_2("tos2_2", {}, Location("0x3", "type", p4));
  TransportOrder simple_to_2("simple_to_2", {pickup_2}, delivery_2);

  Task task_1("task_1", "127.0.0.1:5000", {simple_to_1, simple_to_2}, {});

  auto add_1_opt = management.addTask(task_1);
  REQUIRE(add_1_opt);

  // current metrics of the second order
  auto add_1_metrics = management.getFinalMetrics();
  REQUIRE(add_1_metrics.empty_travel_distance == 10);
  REQUIRE(add_1_metrics.empty_travel_time == 11);
  REQUIRE(add_1_metrics.getMakespan() == 58);
}

TEST_CASE("SimpleOrderManagement One Transport, Move, and Action Order in one Task",
          "[adding and removing vertices]") {
  // arrange
  auto current_pose = daisi::util::Pose(p0);
  SimpleOrderManagement management(buildBasicAmrDescription(), buildBasicTopology(), current_pose);

  TransportOrderStep pickup_1("tos1_1", {}, Location("0x0", "type", p0));
  TransportOrderStep delivery_1("tos2_1", {}, Location("0x1", "type", p1));
  TransportOrder to_1("to_1", {pickup_1}, delivery_1);

  MoveOrderStep mos("mos", {}, Location("0x2", "type", p2));
  MoveOrder mo_1("mo", mos);

  ActionOrderStep aos("aos", {{"load", "load"}});
  ActionOrder ao_1("ao", aos);

  // try to add an illformed task, since a MoveOrder should not be the first order
  Task task_1("task_1", "127.0.0.1:5000", {mo_1, to_1, ao_1}, {});
  REQUIRE_THROWS(management.addTask(task_1));

  // current metrics should be unchanged
  auto metrics = management.getFinalMetrics();
  REQUIRE(metrics.getTime() == 0);
  REQUIRE(metrics.loaded_travel_time == 0);
  REQUIRE(metrics.empty_travel_time == 0);
  REQUIRE(metrics.action_time == 0);
  REQUIRE(metrics.loaded_travel_distance == 0);
  REQUIRE(metrics.empty_travel_distance == 0);
  REQUIRE(metrics.getDistance() == 0);
  REQUIRE(metrics.getMakespan() == 0);

  Task task_2("task_2", "127.0.0.1:5000", {to_1, mo_1, ao_1}, {});

  auto add_opt = management.addTask(task_2);
  REQUIRE(add_opt);
  auto add_metrics = management.getFinalMetrics();

  // current metrics of the action order
  REQUIRE(add_metrics.action_time == 7);
  REQUIRE(add_metrics.empty_travel_distance == 0);
  REQUIRE(add_metrics.empty_travel_time == 0);
  REQUIRE(add_metrics.loaded_travel_distance == 0);
  REQUIRE(add_metrics.loaded_travel_time == 0);

  // makespan + time for the previous orders
  REQUIRE(add_metrics.getMakespan() >= add_metrics.getTime() + 41);
}

TEST_CASE("Simple Order Management Three Simple Transport Orders with time delay",
          "[adding and removing vertices]") {
  // arrange
  auto current_pose = daisi::util::Pose(p0);
  SimpleOrderManagement management(buildBasicAmrDescription(), buildBasicTopology(), current_pose);

  // transport order 1
  TransportOrderStep pickup_1("tos1_1", {}, Location("0x0", "type", p3));
  TransportOrderStep delivery_1("tos2_1", {}, Location("0x1", "type", p2));
  TransportOrder simple_to_1("simple_to_1", {pickup_1}, delivery_1);
  Task simple_task_1("simple_task_1", "127.0.0.1:5000", {simple_to_1}, {});

  // transport order 2
  TransportOrderStep pickup_2("tos1_2", {}, Location("0x2", "type", p6));
  TransportOrderStep delivery_2("tos2_2", {}, Location("0x3", "type", p5));
  TransportOrder simple_to_2("simple_to_2", {pickup_2}, delivery_2);
  Task simple_task_2("simple_task_2", "127.0.0.1:5000", {simple_to_2}, {});

  // transport order 3
  TransportOrderStep pickup_3("tos1_3", {}, Location("0x4", "type", p1));
  TransportOrderStep delivery_3("tos2_3", {}, Location("0x5", "type", p0));
  TransportOrder simple_to_3("simple_to_3", {pickup_3}, delivery_3);
  Task simple_task_3("simple_task_3", "127.0.0.1:5000", {simple_to_3}, {});

  auto add_1_opt = management.addTask(simple_task_1);
  auto add_1_metrics = management.getFinalMetrics();

  management.setCurrentTime(10);
  auto add_2_opt = management.addTask(simple_task_2);
  auto add_2_metrics = management.getFinalMetrics();

  REQUIRE(add_1_opt);
  REQUIRE(add_2_opt);

  REQUIRE(add_1_metrics.getDistance() == add_2_metrics.getDistance());
  REQUIRE(add_1_metrics.getTime() == add_2_metrics.getTime());

  // makespan should not have been affected by the current time
  REQUIRE(add_2_metrics.getMakespan() == add_1_metrics.getMakespan() + add_2_metrics.getTime());

  // current time should not be decreased
  REQUIRE_THROWS(management.setCurrentTime(0));

  management.setCurrentTime(100);
  // add task 3, should come after task 2
  auto can_add_3_opt = management.canAddTask(simple_task_3);
  REQUIRE(can_add_3_opt);
  auto add_3_opt = management.addTask(simple_task_3);
  auto add_3_metrics = management.getFinalMetrics();

  // makespan should have been affected by the current time
  REQUIRE(add_3_metrics.getMakespan() == add_3_metrics.getTime() + 100);
  REQUIRE(add_3_metrics.getMakespan() > add_2_metrics.getMakespan() + add_3_metrics.getTime());
}
