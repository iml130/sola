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

#include "cpps/logical/algorithms/disposition/auction_participant_state.h"

#include <catch2/catch_test_macros.hpp>
#include <string>

#include "cpps/logical/order_management/stn_order_management.h"
#include "material_flow/model/task.h"

using namespace daisi;
using namespace daisi::cpps;
using namespace daisi::material_flow;
using namespace daisi::cpps::logical;

std::vector<Task> getThreeTasks() {
  TransportOrderStep pickup1("tos11", {}, Location("0x0", "type", util::Position(10, 10)));
  TransportOrderStep delivery1("tos12", {}, Location("0x0", "type", util::Position(10, 20)));
  TransportOrder to1({pickup1}, delivery1);
  Task task1("task1", {to1}, {});

  TransportOrderStep pickup2("tos21", {}, Location("0x0", "type", util::Position(20, 10)));
  TransportOrderStep delivery2("tos22", {}, Location("0x0", "type", util::Position(10, 20)));
  TransportOrder to2({pickup2}, delivery2);
  Task task2("task2", {to2}, {});

  TransportOrderStep pickup3("tos31", {}, Location("0x0", "type", util::Position(10, 20)));
  TransportOrderStep delivery3("tos32", {}, Location("0x0", "type", util::Position(5, 5)));
  TransportOrder to3({pickup3}, delivery3);
  Task task3("task3", {to3}, {});

  amr::AmrStaticAbility ability1(amr::LoadCarrier(amr::LoadCarrier::Types::kPackage), 20);
  amr::AmrStaticAbility ability2(amr::LoadCarrier(amr::LoadCarrier::Types::kEuroBox), 20);

  task1.setAbilityRequirement(ability1);
  task2.setAbilityRequirement(ability2);
  task3.setAbilityRequirement(ability1);

  return {task1, task2, task3};
}

MetricsComposition getMetrics1() {
  Metrics metrics(10, 10, 10, 10, 10);
  MetricsComposition composition(metrics);
  return composition;
}

MetricsComposition getMetrics2() {
  Metrics metrics(20, 20, 20, 20, 20);
  MetricsComposition composition(metrics);
  return composition;
}

MetricsComposition getMetrics3() {
  Metrics metrics(30, 30, 30, 30, 30);
  MetricsComposition composition(metrics);
  return composition;
}

TEST_CASE("Empty Constructor", "[constructor]") {
  std::vector<Task> tasks = {};
  AuctionParticipantState state(tasks);

  REQUIRE(!state.hasEntries());
  REQUIRE(state.task_state_mapping.empty());
  REQUIRE(state.previously_submitted.empty());

  REQUIRE_THROWS(state.pickBest());
  state.prune();
  REQUIRE(!state.hasEntries());
}

TEST_CASE("Small Constructor", "[constructor]") {
  auto tasks = getThreeTasks();

  AuctionParticipantState state(tasks);
  REQUIRE(state.hasEntries());
  REQUIRE(state.task_state_mapping.size() == 3);
  REQUIRE(state.previously_submitted.empty());

  for (auto it = state.task_state_mapping.begin(); it != state.task_state_mapping.end(); it++) {
    REQUIRE(!it->second.isValid());
  }

  REQUIRE_THROWS(state.pickBest());

  state.prune();
  REQUIRE(!state.hasEntries());
}

TEST_CASE("Small Examples", "[pickBest]") {
  auto tasks = getThreeTasks();

  AuctionParticipantState state(tasks);

  auto basic_insertion_point = std::make_shared<AuctionBasedOrderManagement::InsertionPoint>();

  auto m1 = getMetrics1();
  auto m2 = getMetrics2();
  auto m3 = getMetrics3();

  REQUIRE(m1 > m2);
  REQUIRE(m2 > m3);
  REQUIRE(m1 > m3);

  auto it = state.task_state_mapping.begin();

  std::string middle = it->second.getTask().getName();
  it->second.setInformation(m2, basic_insertion_point);

  it++;
  std::string worst = it->second.getTask().getName();
  it->second.setInformation(m3, basic_insertion_point);

  it++;
  std::string best = it->second.getTask().getName();
  it->second.setInformation(m1, basic_insertion_point);

  AuctionParticipantTaskState best_task_state = state.pickBest();
  REQUIRE(best_task_state.getTask().getName() == best);
  REQUIRE(best_task_state.getMetricsComposition() == m1);
  REQUIRE(best_task_state.isValid());
  REQUIRE(best_task_state.getInsertionPoint());

  it->second.removeInformation();
  REQUIRE_THROWS(state.pickBest());
  state.prune();
  REQUIRE(state.hasEntries());

  AuctionParticipantTaskState middle_task_state = state.pickBest();
  REQUIRE(middle_task_state.getTask().getName() == middle);
  REQUIRE(middle_task_state.getMetricsComposition() == m2);
  REQUIRE(middle_task_state.isValid());
  REQUIRE(middle_task_state.getInsertionPoint());

  state.task_state_mapping.begin()->second.removeInformation();
  REQUIRE_THROWS(state.pickBest());
  state.prune();
  REQUIRE(state.hasEntries());
  REQUIRE(state.task_state_mapping.size() == 1);

  AuctionParticipantTaskState worst_task_state = state.pickBest();
  REQUIRE(worst_task_state.getTask().getName() == worst);
  REQUIRE(worst_task_state.getMetricsComposition() == m3);
  REQUIRE(worst_task_state.isValid());
  REQUIRE(worst_task_state.getInsertionPoint());

  state.task_state_mapping.begin()->second.removeInformation();
  REQUIRE_THROWS(state.task_state_mapping.begin()->second.getMetricsComposition());
  REQUIRE(state.task_state_mapping.begin()->second.getInsertionPoint() == nullptr);
  REQUIRE(!state.task_state_mapping.begin()->second.isValid());

  REQUIRE_THROWS(state.pickBest());
  state.prune();
  REQUIRE(!state.hasEntries());

  REQUIRE(state.previously_submitted.empty());
  REQUIRE_THROWS(state.pickBest());
}
