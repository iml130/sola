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

#include "round_robin_initiator.h"

#include "cpps/amr/model/amr_fleet.h"
#include "ns3/simulator.h"
#include "utils/random_engine.h"

namespace daisi::cpps::logical {
RoundRobinInitiator::RoundRobinInitiator(daisi::cpps::common::CppsCommunicatorPtr communicator,
                                         std::shared_ptr<CppsLoggerNs3> logger)
    : CentralizedInitiator(communicator, logger){};

bool RoundRobinInitiator::process(const AssignmentResponse &assignment_response) {
  if (assignment_response.doesAccept()) {
    assignment_acceptions_.push_back(assignment_response);
    // TODO find correct task for corresponding uuid ad log it
  } else {
    throw std::logic_error("The AMR should always accept the task assignment!");
  }
  return true;
}

// Since this is a basic algorithm, the AMR's status is not relevant here and can be ignored.
bool RoundRobinInitiator::process(const StatusUpdate &) { return true; }

void RoundRobinInitiator::storeParticipant(ParticipantInfo &info) {
  // transform base class ParticipantInfo into concrete ParticipantInfoRoundRobin and store it
  ParticipantInfoRoundRobin info_rr;
  info_rr.ability = info.ability;
  info_rr.connection_string = info.connection_string;
  participants_per_ability_[info_rr.ability].push(info_rr);
}

void RoundRobinInitiator::logMaterialFlowContent(const std::string &material_flow_uuid) {
  // only roughly implemented since the MFDLScheduler is not available yet
  [[maybe_unused]] auto material_flow =
      find_if(material_flows_.begin(), material_flows_.end(),
              [&material_flow_uuid](const auto & /*mf_scheduler*/) {
                // placeholder
                return true;
              });
  // TODO: Log tasks and orders of the MFDLScheduler with the passed uuid
}

void RoundRobinInitiator::distributeMFTasks(uint32_t index, bool previously_allocated) {
  // TODO: filter the wanted information out from the MFDLScheduler
  const std::string connection_string = communicator_->network.getConnectionString();

  // hardcoded tasks for testing
  material_flow::TransportOrderStep pickup1(
      "tos11", {}, material_flow::Location("0x0", "type", util::Position(10, 10)));
  material_flow::TransportOrderStep delivery1(
      "tos12", {}, material_flow::Location("0x0", "type", util::Position(10, 20)));
  material_flow::TransportOrder to1({pickup1}, delivery1);
  material_flow::Task task1("task1", connection_string, {to1}, {});

  material_flow::TransportOrderStep pickup2(
      "tos21", {}, material_flow::Location("0x0", "type", util::Position(20, 10)));
  material_flow::TransportOrderStep delivery2(
      "tos22", {}, material_flow::Location("0x0", "type", util::Position(10, 20)));
  material_flow::TransportOrder to2({pickup2}, delivery2);
  material_flow::Task task2("task2", connection_string, {to2}, {});

  material_flow::TransportOrderStep pickup3(
      "tos31", {}, material_flow::Location("0x0", "type", util::Position(10, 20)));
  material_flow::TransportOrderStep delivery3(
      "tos32", {}, material_flow::Location("0x0", "type", util::Position(5, 5)));
  material_flow::TransportOrder to3({pickup3}, delivery3);
  material_flow::Task task3("task3", connection_string, {to3}, {});

  material_flow::TransportOrderStep pickup4(
      "tos41", {}, material_flow::Location("0x0", "type", util::Position(0, 10)));
  material_flow::TransportOrderStep delivery4(
      "tos42", {}, material_flow::Location("0x0", "type", util::Position(15, 15)));
  material_flow::TransportOrder to4({pickup4}, delivery4);
  material_flow::Task task4("task4", connection_string, {to4}, {});

  material_flow::TransportOrder to5({pickup2}, delivery4);
  material_flow::Task task5("task5", connection_string, {to5}, {});

  material_flow::TransportOrder to6({pickup1}, delivery1);
  material_flow::Task task6("task6", connection_string, {to6}, {});

  material_flow::TransportOrder to7({pickup3}, delivery2);
  material_flow::Task task7("task7", connection_string, {to7}, {});

  amr::AmrStaticAbility ability1(amr::LoadCarrier(amr::LoadCarrier::Types::kPackage), 20);
  amr::AmrStaticAbility ability2(amr::LoadCarrier(amr::LoadCarrier::Types::kEuroBox), 20);
  amr::AmrStaticAbility ability3(amr::LoadCarrier(amr::LoadCarrier::Types::kPackage), 40);

  task1.setAbilityRequirement(ability1);
  task2.setAbilityRequirement(ability2);
  task3.setAbilityRequirement(ability3);
  task4.setAbilityRequirement(ability3);
  task5.setAbilityRequirement(ability1);
  task6.setAbilityRequirement(ability3);
  task7.setAbilityRequirement(ability1);

  std::string mf_id = "test_id";
  std::vector<material_flow::Task> tasks = {task1, task2, task3, task4, task5, task6, task7};

  // check wether all tasks have to be distributed, or only parts of it
  if (previously_allocated) {
    tasks = unallocated_tasks_per_mf_[mf_id];
  } else {
    unallocated_tasks_per_mf_[mf_id] = tasks;
  }
  for (const auto &task : tasks) {
    assignTask(task);
  }
  ns3::Simulator::Schedule(ns3::Seconds(delays_.wait_to_receive_assignment_response),
                           &RoundRobinInitiator::processAssignmentAcceptions, this, index);
}

void RoundRobinInitiator::assignTask(const material_flow::Task &task) {
  std::shared_ptr<RoundRobinInitiator::ParticipantInfoRoundRobin> chosen_participant =
      chooseParticipantForTask(task);

  // inform about task assignment
  AssignmentNotification assignment_notification(task,
                                                 communicator_->network.getConnectionString());
  communicator_->network.send(
      {chosen_participant->connection_string, serialize(assignment_notification)});

  // update participant and priority queue
  chosen_participant->assignment_counter++;
  participants_per_ability_[chosen_participant->ability].pop();
  participants_per_ability_[chosen_participant->ability].push(*chosen_participant);
}

std::shared_ptr<RoundRobinInitiator::ParticipantInfoRoundRobin>
RoundRobinInitiator::chooseParticipantForTask(const material_flow::Task &task) {
  // discover participants which can execute the task
  std::vector<daisi::cpps::amr::AmrStaticAbility> fitting_abilities =
      AmrFleet::get().getFittingExistingAbilities(task.getAbilityRequirement());
  std::vector<ParticipantInfoRoundRobin> possible_candidates;
  if (fitting_abilities.empty()) {
    throw std::runtime_error("The task requirements cannot be met by any participant.");
  }

  for (const auto &ability : fitting_abilities) {
    // observe the participant with the highest priority per ability
    ParticipantInfoRoundRobin first_participant = participants_per_ability_[ability].top();

    // check wether the priority is high enough to become a possible candidate for the task
    if (possible_candidates.empty() ||
        first_participant.assignment_counter <= possible_candidates[0].assignment_counter) {
      if (!possible_candidates.empty() &&
          first_participant.assignment_counter < possible_candidates[0].assignment_counter) {
        // higher prio than all other participants so far
        possible_candidates.clear();
      }
      possible_candidates.push_back(first_participant);
    }
  }
  if (possible_candidates.empty()) {
    throw std::runtime_error("No AMR matches the task requirements");
  }

  // choose AMR randomly
  std::uniform_int_distribution<uint64_t> dist(0, possible_candidates.size() - 1);
  const uint64_t random_index = dist(daisi::global_random_engine);
  auto chosen_participant =
      std::make_shared<ParticipantInfoRoundRobin>(possible_candidates.at(random_index));

  return chosen_participant;
}

void RoundRobinInitiator::processAssignmentAcceptions(uint32_t index) {
  // TODO: find the correct mf_id with the passed index when MFDLScheduler available

  // id for testing
  std::string mf_id = "test_id";

  auto distributed_tasks = unallocated_tasks_per_mf_[mf_id];
  // remove tasks where we received an acception for
  for (const auto &message : assignment_acceptions_) {
    auto task_it = std::find_if(distributed_tasks.begin(), distributed_tasks.end(),
                                [&message](const material_flow::Task &task) -> bool {
                                  return task.getUuid() == message.getTaskUuid();
                                });
    assert(task_it != distributed_tasks.end());
    distributed_tasks.erase(task_it);
  }
  if (!distributed_tasks.empty()) {
    // reassign failed tasks
    distributeMFTasks(index, true);
  }
}

}  // namespace daisi::cpps::logical
