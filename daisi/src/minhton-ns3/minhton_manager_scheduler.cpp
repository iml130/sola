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
  uint64_t default_delay = manager_.parser_.getDefaultDelay();
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

  auto scenario_sequence = manager_.parser_.getScenarioSequence();

  setupIndexQueues();
  Simulator::Schedule(MilliSeconds(10), &MinhtonManager::Scheduler::setupRequestingNodes, this);
  Simulator::Schedule(MilliSeconds(10), &MinhtonManager::Scheduler::setupRootBehavior, this);

  for (std::shared_ptr<ScenariofileParser::Table> command : scenario_sequence) {
    auto map = command->content;

    auto it_time = map.find("time");
    if (it_time != map.end()) {
      uint64_t delay = *std::get_if<uint64_t>(&it_time->second);
      current_time += delay;
      continue;
    }

    auto it_join_many = map.find("join-many");
    if (it_join_many != map.end()) {
      parseJoinMany(current_time, default_delay, it_join_many);
      continue;
    }

    auto it_join_one = map.find("join-one");
    if (it_join_one != map.end()) {
      parseJoinOne(current_time, default_delay, it_join_one);
      continue;
    }

    auto it_static_build = map.find("static-build");
    if (it_static_build != map.end()) {
      parseStaticBuild(current_time, default_delay, it_static_build);
      continue;
    }

    auto it_leave_many = map.find("leave-many");
    if (it_leave_many != map.end()) {
      parseLeaveMany(current_time, default_delay, it_leave_many);
      continue;
    }

    auto it_leave_one = map.find("leave-one");
    if (it_leave_one != map.end()) {
      parseLeaveOne(current_time, default_delay, it_leave_one);
      continue;
    }

    auto it_fail_one = map.find("fail-one");
    if (it_fail_one != map.end()) {
      parseFailOne(current_time, default_delay, it_fail_one);
      continue;
    }

    auto it_fail_many = map.find("fail-many");
    if (it_fail_many != map.end()) {
      parseFailMany(current_time, default_delay, it_fail_many);
      continue;
    }

    auto it_search_all = map.find("search-all");
    if (it_search_all != map.end()) {
      parseSearchAll(current_time, default_delay, it_search_all);
      continue;
    }

    auto it_search_many = map.find("search-many");
    if (it_search_many != map.end()) {
      parseSearchMany(current_time, default_delay, it_search_many);
      continue;
    }

    auto it_mixed_execution = map.find("mixed-execution");
    if (it_mixed_execution != map.end()) {
      parseMixedExecution(current_time, default_delay, it_mixed_execution);
      continue;
    }

    auto it_validate_leave = map.find("validate-leave");
    if (it_validate_leave != map.end()) {
      parseValidateLeave(current_time, default_delay, it_validate_leave);
      continue;
    }

    auto it_find_query = map.find("find-query");
    if (it_find_query != map.end()) {
      parseFindQuery(current_time, default_delay, it_find_query);
      continue;
    }

    auto it_request_countdown = map.find("request-countdown");
    if (it_request_countdown != map.end()) {
      parseRequestCountdown(current_time, default_delay, it_request_countdown);
      continue;
    }
  }
}

void MinhtonManager::Scheduler::parseJoinMany(uint64_t &current_time, const uint64_t &default_delay,
                                              ScenariofileParser::Table::TableMap::iterator it) {
  uint64_t nodes_to_join = INNER_TABLE(it)->getRequired<uint64_t>("number");
  std::string mode = INNER_TABLE(it)->getRequired<std::string>("mode");

  std::optional<uint64_t> optional_delay = INNER_TABLE(it)->getOptional<uint64_t>("delay");
  uint64_t join_many_delay = optional_delay ? default_delay + *optional_delay : default_delay;
  current_time += join_many_delay;

  if (mode == "random") {
    for (uint64_t i = 0; i < nodes_to_join; i++) {
      Simulator::Schedule(MilliSeconds(current_time + join_many_delay * i),
                          &MinhtonManager::Scheduler::executeOneRandomJoin, this);
    }
  } else if (mode == "root") {
    for (uint64_t i = 0; i < nodes_to_join; i++) {
      Simulator::Schedule(MilliSeconds(current_time + join_many_delay * i),
                          &MinhtonManager::Scheduler::executeOneJoinOnRoot, this);
    }
  } else if (mode == "discover") {
    for (uint64_t i = 0; i < nodes_to_join; i++) {
      Simulator::Schedule(MilliSeconds(current_time + join_many_delay * i),
                          &MinhtonManager::Scheduler::executeOneJoinDiscover, this);
    }
  } else {
    throw std::invalid_argument("Invalid Join-Many mode");
  }

  current_time += join_many_delay * (nodes_to_join - 1);
}

void MinhtonManager::Scheduler::parseJoinOne(uint64_t &current_time, const uint64_t &default_delay,
                                             ScenariofileParser::Table::TableMap::iterator it) {
  std::optional<uint64_t> optional_delay = INNER_TABLE(it)->getOptional<uint64_t>("delay");
  uint64_t join_one_delay = optional_delay ? default_delay + *optional_delay : default_delay;
  current_time += join_one_delay;

  std::optional<uint64_t> optional_level = INNER_TABLE(it)->getOptional<uint64_t>("level");
  std::optional<uint64_t> optional_number = INNER_TABLE(it)->getOptional<uint64_t>("number");
  std::optional<uint64_t> optional_index = INNER_TABLE(it)->getOptional<uint64_t>("index");

  if (optional_level && optional_number) {
    Simulator::Schedule(MilliSeconds(current_time),
                        &MinhtonManager::Scheduler::executeOneJoinByPosition, this, *optional_level,
                        *optional_number);
  } else if (optional_index) {
    Simulator::Schedule(MilliSeconds(current_time),
                        &MinhtonManager::Scheduler::executeOneJoinByIndex, this, *optional_index);
  } else {
    throw std::invalid_argument(
        "Invalid Join-One Arguments. Either join by position (level and number) or by index");
  }
}

void MinhtonManager::Scheduler::parseLeaveMany(uint64_t &current_time,
                                               const uint64_t &default_delay,
                                               ScenariofileParser::Table::TableMap::iterator it) {
  uint64_t nodes_to_leave = INNER_TABLE(it)->getRequired<uint64_t>("number");
  std::string mode = INNER_TABLE(it)->getRequired<std::string>("mode");

  std::optional<uint64_t> optional_delay = INNER_TABLE(it)->getOptional<uint64_t>("delay");
  uint64_t leave_many_delay = optional_delay ? default_delay + *optional_delay : default_delay;
  current_time += leave_many_delay;

  if (mode == "random") {
    for (uint64_t i = 0; i < nodes_to_leave; i++) {
      Simulator::Schedule(MilliSeconds(current_time + leave_many_delay * i),
                          &MinhtonManager::Scheduler::executeOneRandomLeave, this);
    }
  } else if (mode == "root") {
    for (uint64_t i = 0; i < nodes_to_leave; i++) {
      Simulator::Schedule(MilliSeconds(current_time + leave_many_delay * i),
                          &MinhtonManager::Scheduler::executeOneLeaveOnRoot, this);
    }
  } else {
    throw std::invalid_argument("Invalid Leave-Many mode");
  }

  current_time += leave_many_delay * (nodes_to_leave - 1);
}

void MinhtonManager::Scheduler::parseLeaveOne(uint64_t &current_time, const uint64_t &default_delay,
                                              ScenariofileParser::Table::TableMap::iterator it) {
  std::optional<uint64_t> optional_delay = INNER_TABLE(it)->getOptional<uint64_t>("delay");
  uint64_t leave_one_delay = optional_delay ? default_delay + *optional_delay : default_delay;
  current_time += leave_one_delay;

  std::optional<uint64_t> optional_level = INNER_TABLE(it)->getOptional<uint64_t>("level");
  std::optional<uint64_t> optional_number = INNER_TABLE(it)->getOptional<uint64_t>("number");
  std::optional<uint64_t> optional_index = INNER_TABLE(it)->getOptional<uint64_t>("index");

  if (optional_level && optional_number) {
    Simulator::Schedule(MilliSeconds(current_time),
                        &MinhtonManager::Scheduler::executeOneLeaveByPosition, this,
                        *optional_level, *optional_number);
  } else if (optional_index) {
    Simulator::Schedule(MilliSeconds(current_time),
                        &MinhtonManager::Scheduler::executeOneLeaveByIndex, this, *optional_index);
  } else {
    throw std::invalid_argument(
        "Invalid Leave-One Arguments. Either join by position (level and number) or by index");
  }
}

void MinhtonManager::Scheduler::parseSearchAll(uint64_t &current_time,
                                               const uint64_t &default_delay,
                                               ScenariofileParser::Table::TableMap::iterator it) {
  std::optional<uint64_t> optional_delay = INNER_TABLE(it)->getOptional<uint64_t>("delay");
  uint64_t search_all_delay = optional_delay ? default_delay + *optional_delay : default_delay;

  Simulator::Schedule(MilliSeconds(current_time + search_all_delay),
                      &MinhtonManager::Scheduler::scheduleSearchExactAll, this, search_all_delay);
  current_time += search_all_delay * (manager_.getNumberOfNodes() + 1);
}

void MinhtonManager::Scheduler::parseSearchMany(uint64_t &current_time,
                                                const uint64_t &default_delay,
                                                ScenariofileParser::Table::TableMap::iterator it) {
  uint64_t number_of_searches = INNER_TABLE(it)->getRequired<uint64_t>("number");

  std::optional<uint64_t> optional_delay = INNER_TABLE(it)->getOptional<uint64_t>("delay");
  uint64_t search_many_delay = optional_delay ? default_delay + *optional_delay : default_delay;

  Simulator::Schedule(MilliSeconds(current_time),
                      &MinhtonManager::Scheduler::scheduleSearchExactMany, this, search_many_delay,
                      number_of_searches);
  current_time += search_many_delay * (number_of_searches + 1) - default_delay;
}

void MinhtonManager::Scheduler::parseFailMany(uint64_t &current_time, const uint64_t &default_delay,
                                              ScenariofileParser::Table::TableMap::iterator it) {
  std::optional<uint64_t> optional_delay = INNER_TABLE(it)->getOptional<uint64_t>("delay");
  uint64_t fail_one_delay = optional_delay ? default_delay + *optional_delay : default_delay;
  current_time += fail_one_delay;

  std::optional<uint64_t> optional_level = INNER_TABLE(it)->getOptional<uint64_t>("level");
  std::optional<uint64_t> optional_number = INNER_TABLE(it)->getOptional<uint64_t>("number");

  if (optional_level && optional_number) {
    Simulator::Schedule(MilliSeconds(current_time),
                        &MinhtonManager::Scheduler::executeOneFailByPosition, this, *optional_level,
                        *optional_number);
  } else {
    throw std::invalid_argument("Invalid Leave-One Arguments. Level and number must be given");
  }
}

void MinhtonManager::Scheduler::parseFailOne(uint64_t &current_time, const uint64_t &default_delay,
                                             ScenariofileParser::Table::TableMap::iterator it) {
  uint64_t nodes_to_fail = INNER_TABLE(it)->getRequired<uint64_t>("number");

  std::optional<uint64_t> optional_delay = INNER_TABLE(it)->getOptional<uint64_t>("delay");
  uint64_t fail_many_delay = optional_delay ? default_delay + *optional_delay : default_delay;
  current_time += fail_many_delay;

  for (uint64_t i = 0; i < nodes_to_fail; i++) {
    Simulator::Schedule(MilliSeconds(current_time + fail_many_delay * i),
                        &MinhtonManager::Scheduler::executeOneRandomFail, this);
  }

  current_time += fail_many_delay * (nodes_to_fail - 1);
}

void MinhtonManager::Scheduler::parseMixedExecution(
    uint64_t &current_time, const uint64_t &default_delay,
    ScenariofileParser::Table::TableMap::iterator it) {
  std::optional<uint64_t> optional_delay = INNER_TABLE(it)->getOptional<uint64_t>("delay");
  uint64_t mixed_execution_delay = optional_delay ? default_delay + *optional_delay : default_delay;

  uint64_t join_number = INNER_TABLE(it)->getRequired<uint64_t>("join-number");
  uint64_t leave_number = INNER_TABLE(it)->getRequired<uint64_t>("leave-number");
  uint64_t search_number = INNER_TABLE(it)->getRequired<uint64_t>("search-number");

  this->scheduleMixedExecution(join_number, leave_number, search_number, current_time,
                               mixed_execution_delay);
  current_time += mixed_execution_delay * (join_number + leave_number + search_number);
}

void MinhtonManager::Scheduler::parseValidateLeave(
    uint64_t &current_time, const uint64_t &default_delay,
    ScenariofileParser::Table::TableMap::iterator it) {
  std::optional<uint64_t> optional_delay = INNER_TABLE(it)->getOptional<uint64_t>("delay");
  uint64_t validate_leave_delay = optional_delay ? default_delay + *optional_delay : default_delay;

  current_time += validate_leave_delay;

  Simulator::Schedule(MilliSeconds(current_time), &MinhtonManager::Scheduler::scheduleValidateLeave,
                      this, validate_leave_delay);

  // upper bound, might be less though
  // times 2 for each leave and join
  current_time += validate_leave_delay * (manager_.getNumberOfNodes() - 1) * 2;
}

void MinhtonManager::Scheduler::parseFindQuery(uint64_t &current_time,
                                               const uint64_t &default_delay,
                                               ScenariofileParser::Table::TableMap::iterator it) {
  uint64_t level = INNER_TABLE(it)->getRequired<uint64_t>("level");
  uint64_t number = INNER_TABLE(it)->getRequired<uint64_t>("number");
  std::string scope = INNER_TABLE(it)->getRequired<std::string>("scope");
  std::string query_string = INNER_TABLE(it)->getRequired<std::string>("query");
  uint64_t validity_threshold = INNER_TABLE(it)->getRequired<uint64_t>("validity-threshold");

  minhton::FindQuery query(query_string, scope);

  minhton::NodeInfo requesting_node;
  requesting_node.setPeerInfo(minhton::PeerInfo(level, number));
  query.setRequestingNode(requesting_node);

  query.setValidityThreshold(validity_threshold);
  query.setSelection(minhton::FindQuery::FindQuerySelection::kSelectUnspecific);

  Simulator::Schedule(MilliSeconds(current_time), &MinhtonManager::Scheduler::executeFindQuery,
                      this, query);

  current_time += default_delay;
}

void MinhtonManager::Scheduler::parseRequestCountdown(
    uint64_t &current_time, const uint64_t &default_delay,
    ScenariofileParser::Table::TableMap::iterator it) {
  uint64_t count_value = *std::get_if<uint64_t>(&it->second);
  Simulator::Schedule(MilliSeconds(current_time),
                      &MinhtonManager::Scheduler::activateRequestCountdown, this, count_value);
}

void MinhtonManager::Scheduler::parseStaticBuild(uint64_t &current_time,
                                                 const uint64_t &default_delay,
                                                 ScenariofileParser::Table::TableMap::iterator it) {
  uint64_t number = INNER_TABLE(it)->getRequired<uint64_t>("number");

  Simulator::Schedule(MilliSeconds(current_time),
                      &MinhtonManager::Scheduler::executeStaticNetworkBuild, this, number);

  current_time += default_delay;
}

void MinhtonManager::Scheduler::parseRequests() {
  auto env =
      manager_.parser_.getParsedContent()->getRequired<std::shared_ptr<ScenariofileParser::Table>>(
          "peerDiscoveryEnvironment");

  requests_off_ = false;
  try {
    std::string requ_off = env->getRequired<std::string>("requests");
    if (requ_off == "off") {
      requests_off_ = true;
      return;
    }
  } catch (const std::invalid_argument &e) {
  }

  std::shared_ptr<ScenariofileParser::Table> requests;
  try {
    requests = env->getRequired<std::shared_ptr<ScenariofileParser::Table>>("requests");
  } catch (const std::invalid_argument &e) {
    std::cout << "No find query requests set" << std::endl;
    return;
  }

  auto nodes_cmd = requests->getRequired<std::shared_ptr<ScenariofileParser::Table>>("nodes");
  auto nodes_type = nodes_cmd->getRequired<std::string>("type");
  if (nodes_type == "random") {
    requests_.random_nodes_percentage = nodes_cmd->getRequired<float>("percentage");
  } else if (nodes_type == "absolute") {
    requests_.absolute_nodes_number = nodes_cmd->getRequired<uint64_t>("number");
  } else {
    throw std::logic_error("not implemented");
  }

  auto freq_cmd = requests->getRequired<std::shared_ptr<ScenariofileParser::Table>>("frequency");
  auto freq_type = freq_cmd->getRequired<std::string>("type");
  if (freq_type == "static") {
    uint64_t time_ms = freq_cmd->getRequired<uint64_t>("time");
    requests_.request_delay_distribution.dist = time_ms;

  } else if (freq_type == "gaussian") {
    uint64_t mean_value = freq_cmd->getRequired<uint64_t>("mean");
    uint64_t sigma_value = freq_cmd->getRequired<uint64_t>("sigma");

    requests_.request_delay_distribution.dist =
        std::normal_distribution<double>(mean_value, (sigma_value));
  } else {
    throw std::logic_error("not implemented");
  }

  auto depth_cmd = requests->getRequired<std::shared_ptr<ScenariofileParser::Table>>("depth");
  auto depth_type = depth_cmd->getRequired<std::string>("type");
  if (depth_type == "uniform") {
    uint64_t min_value = depth_cmd->getRequired<uint64_t>("min");
    uint64_t max_value = depth_cmd->getRequired<uint64_t>("max");
    requests_.query_depth_distribution.dist =
        std::uniform_int_distribution<uint8_t>(min_value, max_value);

  } else if (depth_type == "static") {
    uint64_t value = freq_cmd->getRequired<uint64_t>("value");
    requests_.query_depth_distribution.dist = (uint8_t)value;
  } else {
    throw std::logic_error("not implemented");
  }

  auto validity_cmd =
      requests->getRequired<std::shared_ptr<ScenariofileParser::Table>>("validity-threshold");
  auto validity_type = validity_cmd->getRequired<std::string>("type");
  if (validity_type == "constant") {
    uint64_t time_value = validity_cmd->getRequired<uint64_t>("time");

    requests_.validity_threshold_distribution.dist = time_value;
  } else {
    throw std::logic_error("not implemented");
  }

  auto inquire_outdated_string = requests->getRequired<std::string>("inquire_outdated");
  if (inquire_outdated_string == "on")
    requests_.query_inquire_outdated = true;
  else if (inquire_outdated_string == "off")
    requests_.query_inquire_outdated = false;
  else
    throw std::invalid_argument("invalid option");

  auto inquire_unknown_string = requests->getRequired<std::string>("inquire_unknown");
  if (inquire_unknown_string == "on")
    requests_.query_inquire_unknown = true;
  else if (inquire_unknown_string == "off")
    requests_.query_inquire_unknown = false;
  else
    throw std::invalid_argument("invalid option");
}

void MinhtonManager::Scheduler::parseNodeAttributes() {
  auto env =
      manager_.parser_.getParsedContent()->getRequired<std::shared_ptr<ScenariofileParser::Table>>(
          "peerDiscoveryEnvironment");

  attributes_off_ = false;
  try {
    std::string attr_off = env->getRequired<std::string>("attributes");
    if (attr_off == "off") {
      attributes_off_ = true;
      return;
    }
  } catch (const std::invalid_argument &e) {
  }

  std::shared_ptr<ScenariofileParser::Table> attributes;
  try {
    attributes = env->getRequired<std::shared_ptr<ScenariofileParser::Table>>("attributes");
  } catch (const std::invalid_argument &e) {
    std::cout << "No node content set" << std::endl;
    return;
  }

  for (auto &[name, attr_content] : attributes->content) {
    auto map = (*std::get_if<std::shared_ptr<ScenariofileParser::Table>>(&attr_content));

    Scheduler::NodeAttribute node_attribute;
    node_attribute.name = name;

    auto presence_percentage =
        map->getRequired<std::shared_ptr<ScenariofileParser::Table>>("presenceBehavior")
            ->getRequired<float>("percentage");
    node_attribute.presence_percentage = presence_percentage;

    auto content_behavior_cmd =
        map->getRequired<std::shared_ptr<ScenariofileParser::Table>>("contentBehavior");
    auto content_type = content_behavior_cmd->getRequired<std::string>("type");

    if (content_type == "uniform") {
      uint64_t min_value = content_behavior_cmd->getRequired<uint64_t>("min");
      uint64_t max_value = content_behavior_cmd->getRequired<uint64_t>("max");

      node_attribute.content_distribution = CreateObject<UniformRandomVariable>();
      node_attribute.content_distribution->SetAttribute("Min", ns3::DoubleValue(min_value));
      node_attribute.content_distribution->SetAttribute("Max", ns3::DoubleValue(max_value));

      node_attribute.content_is_numerical = true;

    } else if (content_type == "gaussian") {
      uint64_t mean_value = content_behavior_cmd->getRequired<uint64_t>("mean");
      uint64_t sigma_value = content_behavior_cmd->getRequired<uint64_t>("sigma");

      node_attribute.content_distribution = CreateObject<NormalRandomVariable>();
      node_attribute.content_distribution->SetAttribute("Mean", ns3::DoubleValue(mean_value));
      node_attribute.content_distribution->SetAttribute(
          "Variance", ns3::DoubleValue(sigma_value * sigma_value));

      node_attribute.content_is_numerical = true;

    } else if (content_type == "constant") {
      auto opt_constant_value = content_behavior_cmd->getValue("value");
      if (opt_constant_value.has_value()) {
        node_attribute.empirical_value_mapping[1] = opt_constant_value.value();
      } else {
        throw std::invalid_argument("no proper value for constant given");
      }

      node_attribute.content_distribution = CreateObject<ConstantRandomVariable>();
      node_attribute.content_distribution->SetAttribute("Constant", ns3::DoubleValue(1));

      node_attribute.content_is_numerical = false;

    } else if (content_type == "choice") {
      Ptr<EmpiricalRandomVariable> erv = CreateObject<EmpiricalRandomVariable>();
      erv->SetInterpolate(false);
      erv->CDF(0.0, 0.0);

      auto values =
          content_behavior_cmd
              ->getRequired<std::vector<std::shared_ptr<ScenariofileParser::Table>>>("values");

      double current_prob = 0.0;
      int i = 1;
      for (auto table : values) {
        auto content_behaviour_table_map =
            table->getRequired<std::shared_ptr<ScenariofileParser::Table>>(
                table->content.begin()->first);

        auto opt_content = content_behaviour_table_map->getValue("content");
        if (opt_content.has_value()) {
          node_attribute.empirical_value_mapping[i] = opt_content.value();
        } else {
          throw std::invalid_argument("invalid content in choice distribution");
        }

        double prob = content_behaviour_table_map->getRequired<float>("prob");
        current_prob += prob;

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

    } else {
      throw std::logic_error("not implemented");
    }

    auto update_behavior_cmd =
        map->getRequired<std::shared_ptr<ScenariofileParser::Table>>("updateBehavior");

    auto update_type = update_behavior_cmd->getRequired<std::string>("type");
    if (update_type == "constant") {
      uint64_t time_ms = update_behavior_cmd->getRequired<uint64_t>("value");
      node_attribute.update_delay_distribution.dist = time_ms;

      node_attribute.value_type = minhton::NodeData::ValueType::kValueDynamic;

    } else if (update_type == "gaussian") {
      uint64_t mean_value = update_behavior_cmd->getRequired<uint64_t>("mean");
      uint64_t sigma_value = update_behavior_cmd->getRequired<uint64_t>("sigma");

      node_attribute.update_delay_distribution.dist =
          std::normal_distribution<double>(mean_value, (sigma_value));

      node_attribute.value_type = minhton::NodeData::ValueType::kValueDynamic;

    } else if (update_type == "uniform") {
      uint64_t min_value = update_behavior_cmd->getRequired<uint64_t>("min");
      uint64_t max_value = update_behavior_cmd->getRequired<uint64_t>("max");

      node_attribute.update_delay_distribution.dist =
          std::uniform_int_distribution<uint64_t>(min_value, max_value);

      node_attribute.value_type = minhton::NodeData::ValueType::kValueDynamic;

    } else if (update_type == "static") {
      node_attribute.value_type = minhton::NodeData::ValueType::kValueStatic;

    } else {
      throw std::logic_error("not implemented");
    }

    this->node_content_attributes_.push_back(node_attribute);
    this->attribute_names_.push_back(node_attribute.name);
  }
}

}  // namespace daisi::minhton_ns3
