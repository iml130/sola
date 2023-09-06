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

#include "minhton_manager_scheduler.h"

#include "minhton_manager.h"
#include "ns3/double.h"
#include "utils/sola_utils.h"

using namespace ns3;

namespace daisi::minhton_ns3 {

void MinhtonManager::scheduleEvents() { scheduler_->schedule(); }

MinhtonManager::Scheduler::Scheduler(MinhtonManager &manager)
    : manager_(manager),
      uniform_zero_one_distribution_(std::uniform_real_distribution<double>(0.0, 1.0)){};

void MinhtonManager::Scheduler::schedule() {
  uint64_t current_time = 0;

  parseNodeAttributes();
  parseRequests();

  if (attributes_off_) {
    std::cout << "Attributes OFF" << std::endl;
  } else {
    std::cout << "Attributes ON" << std::endl;
  }

  if (requests_off_) {
    std::cout << "Requests OFF" << std::endl;
  } else {
    std::cout << "Requests ON" << std::endl;
  }

  setupIndexQueues();
  Simulator::Schedule(MilliSeconds(10), &MinhtonManager::Scheduler::setupRequestingNodes, this);
  Simulator::Schedule(MilliSeconds(10), &MinhtonManager::Scheduler::setupRootBehavior, this);

  for (const MinhtonScenarioSequenceStep &step : manager_.scenariofile_.scenario_sequence) {
    std::visit([this, &current_time](auto &&step) { schedule(step, current_time); }, step.step);
  }
}

void MinhtonManager::Scheduler::schedule(Time step, uint64_t &current_time) {
  current_time += step.time;
}

void MinhtonManager::Scheduler::schedule(JoinMany step, uint64_t &current_time) {
  const uint64_t join_many_delay = manager_.scenariofile_.default_delay + step.delay.value_or(0);
  current_time += join_many_delay;

  if (step.mode == "random") {
    for (uint64_t i = 0; i < step.number; i++) {
      Simulator::Schedule(MilliSeconds(current_time + join_many_delay * i),
                          &MinhtonManager::Scheduler::executeOneRandomJoin, this);
    }
  } else if (step.mode == "root") {
    for (uint64_t i = 0; i < step.number; i++) {
      Simulator::Schedule(MilliSeconds(current_time + join_many_delay * i),
                          &MinhtonManager::Scheduler::executeOneJoinOnRoot, this);
    }
  } else if (step.mode == "discover") {
    for (uint64_t i = 0; i < step.number; i++) {
      Simulator::Schedule(MilliSeconds(current_time + join_many_delay * i),
                          &MinhtonManager::Scheduler::executeOneJoinDiscover, this);
    }
  } else {
    throw std::invalid_argument("Invalid Join-Many mode");
  }

  current_time += join_many_delay * (step.number - 1);
}

void MinhtonManager::Scheduler::schedule(JoinOne step, uint64_t &current_time) {
  const uint64_t join_one_delay = manager_.scenariofile_.default_delay + step.delay.value_or(0);
  current_time += join_one_delay;

  if (step.level && step.number) {
    Simulator::Schedule(MilliSeconds(current_time),
                        &MinhtonManager::Scheduler::executeOneJoinByPosition, this,
                        step.level.value(), step.number.value());
  } else if (step.index) {
    Simulator::Schedule(MilliSeconds(current_time),
                        &MinhtonManager::Scheduler::executeOneJoinByIndex, this,
                        step.index.value());
  } else {
    throw std::invalid_argument(
        "Invalid Join-One Arguments. Either join by position (level and number) or by index");
  }
}

void MinhtonManager::Scheduler::schedule(LeaveMany step, uint64_t &current_time) {
  const uint64_t leave_many_delay = manager_.scenariofile_.default_delay + step.delay.value_or(0);
  current_time += leave_many_delay;

  if (step.mode == "random") {
    for (uint64_t i = 0; i < step.number; i++) {
      Simulator::Schedule(MilliSeconds(current_time + leave_many_delay * i),
                          &MinhtonManager::Scheduler::executeOneRandomLeave, this);
    }
  } else if (step.mode == "root") {
    for (uint64_t i = 0; i < step.number; i++) {
      Simulator::Schedule(MilliSeconds(current_time + leave_many_delay * i),
                          &MinhtonManager::Scheduler::executeOneLeaveOnRoot, this);
    }
  } else {
    throw std::invalid_argument("Invalid Leave-Many mode");
  }

  current_time += leave_many_delay * (step.number - 1);
}

void MinhtonManager::Scheduler::schedule(LeaveOne step, uint64_t &current_time) {
  const uint64_t leave_one_delay = manager_.scenariofile_.default_delay + step.delay.value_or(0);
  current_time += leave_one_delay;

  if (step.level && step.number) {
    Simulator::Schedule(MilliSeconds(current_time),
                        &MinhtonManager::Scheduler::executeOneLeaveByPosition, this,
                        step.level.value(), step.number.value());
  } else if (step.index) {
    Simulator::Schedule(MilliSeconds(current_time),
                        &MinhtonManager::Scheduler::executeOneLeaveByIndex, this,
                        step.index.value());
  } else {
    throw std::invalid_argument(
        "Invalid Leave-One Arguments. Either join by position (level and number) or by index");
  }
}

void MinhtonManager::Scheduler::schedule(SearchAll step, uint64_t &current_time) {
  const uint64_t search_all_delay = manager_.scenariofile_.default_delay + step.delay.value_or(0);

  Simulator::Schedule(MilliSeconds(current_time + search_all_delay),
                      &MinhtonManager::Scheduler::scheduleSearchExactAll, this, search_all_delay);
  current_time += search_all_delay * (manager_.getNumberOfNodes() + 1);
}

void MinhtonManager::Scheduler::schedule(SearchMany step, uint64_t &current_time) {
  const uint64_t search_many_delay = manager_.scenariofile_.default_delay + step.delay.value_or(0);

  Simulator::Schedule(MilliSeconds(current_time),
                      &MinhtonManager::Scheduler::scheduleSearchExactMany, this, search_many_delay,
                      step.number);
  current_time += search_many_delay * (step.number + 1) - manager_.scenariofile_.default_delay;
}

void MinhtonManager::Scheduler::schedule(FailMany step, uint64_t &current_time) {
  const uint64_t fail_many_delay = manager_.scenariofile_.default_delay + step.delay.value_or(0);
  current_time += fail_many_delay;

  for (uint64_t i = 0; i < step.number; i++) {
    Simulator::Schedule(MilliSeconds(current_time + fail_many_delay * i),
                        &MinhtonManager::Scheduler::executeOneRandomFail, this);
  }

  current_time += fail_many_delay * (step.number - 1);
}

void MinhtonManager::Scheduler::schedule(FailOne step, uint64_t &current_time) {
  const uint64_t fail_one_delay = manager_.scenariofile_.default_delay + step.delay.value_or(0);
  current_time += fail_one_delay;

  if (step.level && step.number) {
    Simulator::Schedule(MilliSeconds(current_time),
                        &MinhtonManager::Scheduler::executeOneFailByPosition, this,
                        step.level.value(), step.number.value());
  } else if (step.index) {
    Simulator::Schedule(MilliSeconds(current_time),
                        &MinhtonManager::Scheduler::executeOneFailByIndex, this,
                        step.index.value());
  } else {
    throw std::invalid_argument(
        "Invalid Fail-One Arguments. Either fail by position (level and number) or by index");
  }
}

void MinhtonManager::Scheduler::schedule(MixedExecution step, uint64_t &current_time) {
  const uint64_t mixed_execution_delay =
      manager_.scenariofile_.default_delay + step.delay.value_or(0);

  this->scheduleMixedExecution(step.join_number, step.leave_number, step.search_number,
                               current_time, mixed_execution_delay);
  current_time +=
      mixed_execution_delay * (step.join_number + step.leave_number + step.search_number);
}

void MinhtonManager::Scheduler::schedule(ValidateLeave step, uint64_t &current_time) {
  const uint64_t validate_leave_delay =
      manager_.scenariofile_.default_delay + step.delay.value_or(0);

  current_time += validate_leave_delay;

  Simulator::Schedule(MilliSeconds(current_time), &MinhtonManager::Scheduler::scheduleValidateLeave,
                      this, validate_leave_delay);

  // upper bound, might be less though
  // times 2 for each leave and join
  current_time += validate_leave_delay * (manager_.getNumberOfNodes() - 1) * 2;
}

void MinhtonManager::Scheduler::schedule(FindQuery step, uint64_t &current_time) {
  minhton::FindQuery query(step.query, step.scope);

  minhton::NodeInfo requesting_node;
  requesting_node.setLogicalNodeInfo(minhton::LogicalNodeInfo(step.level, step.number));
  query.setRequestingNode(requesting_node);

  query.setValidityThreshold(step.validity_threshold);
  query.setSelection(minhton::FindQuery::FindQuerySelection::kSelectUnspecific);

  Simulator::Schedule(MilliSeconds(current_time), &MinhtonManager::Scheduler::executeFindQuery,
                      this, query);

  current_time += manager_.scenariofile_.default_delay;
}

void MinhtonManager::Scheduler::schedule(RequestCountdown step, uint64_t &current_time) {
  Simulator::Schedule(MilliSeconds(current_time),
                      &MinhtonManager::Scheduler::activateRequestCountdown, this, step.number);
}

void MinhtonManager::Scheduler::schedule(StaticBuild step, uint64_t &current_time) {
  Simulator::Schedule(MilliSeconds(current_time),
                      &MinhtonManager::Scheduler::executeStaticNetworkBuild, this, step.number);

  current_time += manager_.scenariofile_.default_delay;
}

void MinhtonManager::Scheduler::setRequestNodeSetup(RandomNode random) {
  requests_.random_nodes_percentage = random.percentage;
}

void MinhtonManager::Scheduler::setRequestNodeSetup(Absolute absolute) {
  requests_.absolute_nodes_number = absolute.number;
}

void MinhtonManager::Scheduler::setRequestFrequencySetup(Gaussian gaussian) {
  requests_.request_delay_distribution.dist =
      std::normal_distribution<double>(gaussian.mean, gaussian.sigma);
}

void MinhtonManager::Scheduler::setRequestFrequencySetup(StaticTime time) {
  requests_.request_delay_distribution.dist = time.time;  // ms
}

void MinhtonManager::Scheduler::setRequestDepthSetup(Uniform uniform) {
  requests_.query_depth_distribution.dist =
      std::uniform_int_distribution<uint8_t>(uniform.min, uniform.max);
}

void MinhtonManager::Scheduler::setRequestDepthSetup(Static static_val) {
  requests_.query_depth_distribution.dist = static_val.value;
}

void MinhtonManager::Scheduler::setRequestValiditySetup(ConstantTime threshold) {
  requests_.validity_threshold_distribution.dist = threshold.time;
}

void MinhtonManager::Scheduler::parseRequests() {
  if (!manager_.scenariofile_.peer_discovery_environment.has_value()) {
    requests_off_ = true;
    return;
  }

  const Requests &req = manager_.scenariofile_.peer_discovery_environment.value().requests;

  std::visit([this](auto &node) { setRequestNodeSetup(node); }, req.nodes.nodes);
  std::visit([this](auto &node) { setRequestFrequencySetup(node); }, req.frequency.frequency);
  std::visit([this](auto &node) { setRequestDepthSetup(node); }, req.depth.depth);
  std::visit([this](const auto &node) { setRequestValiditySetup(node); },
             req.validity_threshold.threshold);

  requests_.query_inquire_outdated = req.inquire_outdated;
  requests_.query_inquire_unknown = req.inquire_unknown;
}

void MinhtonManager::Scheduler::setAttributeContentBehavior(Choice choice,
                                                            NodeAttribute &node_attribute) {
  Ptr<EmpiricalRandomVariable> erv = CreateObject<EmpiricalRandomVariable>();
  erv->SetInterpolate(false);
  erv->CDF(0.0, 0.0);

  double current_prob = 0.0;
  int i = 1;
  for (const ChoiceValue &value : choice.values) {
    if (value.content_int.has_value()) {
      node_attribute.empirical_value_mapping[i] = value.content_int.value();
    } else if (value.content_float.has_value()) {
      node_attribute.empirical_value_mapping[i] = value.content_float.value();
    } else if (value.content_bool.has_value()) {
      node_attribute.empirical_value_mapping[i] = value.content_bool.value();
    } else if (value.content_string.has_value()) {
      node_attribute.empirical_value_mapping[i] = value.content_string.value();
    } else {
      throw std::runtime_error("invalid constant attribute behavior");
    }

    current_prob += value.prob;

    // the floats/doubles read from yaml always have some slight rounding errors
    // like 0.400000057564
    // therefore we will never reach exactly 1.0 with the yaml values and ns3 will throw errors
    // to bypass this, we have this little check
    if (0.98 <= current_prob && current_prob <= 1.05) {
      current_prob = 1.0;
    }

    erv->CDF(i, current_prob);

    i++;
  }

  node_attribute.content_is_numerical = false;
  node_attribute.content_distribution = erv;
}

void MinhtonManager::Scheduler::setAttributeContentBehavior(Constant constant,
                                                            NodeAttribute &node_attribute) {
  if (constant.value_int.has_value()) {
    node_attribute.empirical_value_mapping[1] = constant.value_int.value();
  } else if (constant.value_float.has_value()) {
    node_attribute.empirical_value_mapping[1] = constant.value_float.value();
  } else if (constant.value_bool.has_value()) {
    node_attribute.empirical_value_mapping[1] = constant.value_bool.value();
  } else if (constant.value_string.has_value()) {
    node_attribute.empirical_value_mapping[1] = constant.value_string.value();
  } else {
    throw std::runtime_error("invalid constant attribute behavior");
  }

  node_attribute.content_distribution = CreateObject<ConstantRandomVariable>();
  node_attribute.content_distribution->SetAttribute("Constant", ns3::DoubleValue(1));

  node_attribute.content_is_numerical = false;
}

void MinhtonManager::Scheduler::setAttributeContentBehavior(Gaussian gaussian,
                                                            NodeAttribute &node_attribute) {
  node_attribute.content_distribution = CreateObject<NormalRandomVariable>();
  node_attribute.content_distribution->SetAttribute("Mean", ns3::DoubleValue(gaussian.mean));
  node_attribute.content_distribution->SetAttribute(
      "Variance", ns3::DoubleValue(gaussian.sigma * gaussian.sigma));

  node_attribute.content_is_numerical = true;
}

void MinhtonManager::Scheduler::setAttributeContentBehavior(Uniform uniform,
                                                            NodeAttribute &node_attribute) {
  node_attribute.content_distribution = CreateObject<UniformRandomVariable>();
  node_attribute.content_distribution->SetAttribute("Min", ns3::DoubleValue(uniform.min));
  node_attribute.content_distribution->SetAttribute("Max", ns3::DoubleValue(uniform.max));

  node_attribute.content_is_numerical = true;
}

void MinhtonManager::Scheduler::setAttributeUpdateBehavior(ConstantTime constant,
                                                           NodeAttribute &node_attribute) {
  node_attribute.update_delay_distribution.dist = constant.time;
  node_attribute.value_type = minhton::NodeData::ValueType::kValueDynamic;
}

void MinhtonManager::Scheduler::setAttributeUpdateBehavior(Gaussian gaussian,
                                                           NodeAttribute &node_attribute) {
  node_attribute.update_delay_distribution.dist =
      std::normal_distribution<double>(gaussian.mean, gaussian.sigma);

  node_attribute.value_type = minhton::NodeData::ValueType::kValueDynamic;
}

void MinhtonManager::Scheduler::setAttributeUpdateBehavior(Uniform uniform,
                                                           NodeAttribute &node_attribute) {
  node_attribute.update_delay_distribution.dist =
      std::uniform_int_distribution<uint64_t>(uniform.min, uniform.max);

  node_attribute.value_type = minhton::NodeData::ValueType::kValueDynamic;
}

void MinhtonManager::Scheduler::setAttributeUpdateBehavior(StaticUpdate,
                                                           NodeAttribute &node_attribute) {
  node_attribute.value_type = minhton::NodeData::ValueType::kValueStatic;
}

void MinhtonManager::Scheduler::parseNodeAttributes() {
  if (!manager_.scenariofile_.peer_discovery_environment.has_value()) {
    attributes_off_ = true;
    return;
  }

  for (const PeerDiscoveryAttribute &attr :
       manager_.scenariofile_.peer_discovery_environment.value().attributes) {
    Scheduler::NodeAttribute node_attribute;
    node_attribute.name = attr.name;

    node_attribute.presence_percentage = attr.presence_behavior.percentage;

    std::visit(
        [this, &node_attribute](auto &node) { setAttributeContentBehavior(node, node_attribute); },
        attr.content_behavior.content_behavior);

    std::visit(
        [this, &node_attribute](auto &node) { setAttributeUpdateBehavior(node, node_attribute); },
        attr.update_behavior.update_behavior);

    this->node_content_attributes_.push_back(node_attribute);
    this->attribute_names_.push_back(node_attribute.name);
  }
}

}  // namespace daisi::minhton_ns3
