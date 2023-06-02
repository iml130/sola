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

#ifndef DAISI_MINHTON_NS3_MINHTON_MANAGER_SCHEDULER_H_
#define DAISI_MINHTON_NS3_MINHTON_MANAGER_SCHEDULER_H_

#include "manager/scenariofileparser.h"
#include "minhton/algorithms/esearch/boolean_expression.h"
#include "minhton/algorithms/esearch/node_data.h"
#include "minhton_manager.h"
#include "ns3/random-variable-stream.h"
#include "utils/distribution.h"

namespace daisi::minhton_ns3 {

class MinhtonManager::Scheduler {
public:
  explicit Scheduler(MinhtonManager &manager);
  void schedule();

private:
  MinhtonManager &manager_;  // ref to parent class

  void setupIndexQueues();
  void setupRequestingNodes();
  void setupRootBehavior();

  void executeOneJoinByPosition(uint16_t level, uint16_t number);
  void executeOneJoinByIndex(uint16_t index);
  void executeOneJoinOnRoot();
  void executeOneRandomJoin();
  void executeOneJoinDiscover();

  void executeOneLeaveByPosition(uint16_t level, uint16_t number);
  void executeOneLeaveByIndex(uint16_t index);
  void executeOneLeaveOnRoot();
  void executeOneRandomLeave();

  void executeOneFailByPosition(uint16_t level, uint16_t number);
  void executeOneRandomFail();

  void initiateJoinNow(uint64_t node_to_join_to_index, uint64_t entering_node_index);
  void initiateJoinNowDiscover(uint64_t entering_node_index);
  void initiateLeaveNow(uint64_t node_to_leave_to_index);
  void initiateFailureNow(uint64_t node_to_fail_to_index);

  void scheduleSearchExactAll(uint64_t delay);
  void scheduleSearchExactMany(uint64_t delay, uint16_t number);
  void executeOneSearchExact(ns3::Ptr<MinhtonNodeNs3> ns3_src_node, uint32_t dest_level,
                             uint32_t dest_number);
  void executeOneRandomSearchExact();
  void executeFindQuery(minhton::FindQuery query);
  void executeStaticNetworkBuild(uint32_t number);

  void scheduleMixedExecution(uint64_t join_num, uint64_t leave_num, uint64_t search_num,
                              uint64_t current_time, uint64_t delay);
  void scheduleValidateLeave(uint64_t delay);

  uint64_t getRootIndex();
  std::vector<uint64_t> getInitializedNodeIndexes();
  std::tuple<std::vector<uint64_t>, std::vector<std::tuple<uint16_t, uint16_t>>>
  getExistingPositions();
  ns3::Ptr<MinhtonNodeNs3> getNodeAtPosition(uint16_t level, uint16_t number);

  std::deque<uint64_t> uninit_index_deque_;
  std::deque<uint64_t> init_index_deque_;
  uint64_t latest_root_index_ = 0;

  // parsing

  void parseJoinMany(uint64_t &current_time, const uint64_t &default_delay,
                     ScenariofileParser::Table::TableMap::iterator it);
  void parseJoinOne(uint64_t &current_time, const uint64_t &default_delay,
                    ScenariofileParser::Table::TableMap::iterator it);
  void parseLeaveMany(uint64_t &current_time, const uint64_t &default_delay,
                      ScenariofileParser::Table::TableMap::iterator it);
  void parseLeaveOne(uint64_t &current_time, const uint64_t &default_delay,
                     ScenariofileParser::Table::TableMap::iterator it);
  void parseSearchAll(uint64_t &current_time, const uint64_t &default_delay,
                      ScenariofileParser::Table::TableMap::iterator it);
  void parseSearchMany(uint64_t &current_time, const uint64_t &default_delay,
                       ScenariofileParser::Table::TableMap::iterator it);
  void parseFailMany(uint64_t &current_time, const uint64_t &default_delay,
                     ScenariofileParser::Table::TableMap::iterator it);
  void parseFailOne(uint64_t &current_time, const uint64_t &default_delay,
                    ScenariofileParser::Table::TableMap::iterator it);
  void parseMixedExecution(uint64_t &current_time, const uint64_t &default_delay,
                           ScenariofileParser::Table::TableMap::iterator it);
  void parseValidateLeave(uint64_t &current_time, const uint64_t &default_delay,
                          ScenariofileParser::Table::TableMap::iterator it);
  void parseFindQuery(uint64_t &current_time, const uint64_t &default_delay,
                      ScenariofileParser::Table::TableMap::iterator it);
  void parseRequestCountdown(uint64_t &current_time, const uint64_t &default_delay,
                             ScenariofileParser::Table::TableMap::iterator it);
  void parseStaticBuild(uint64_t &current_time, const uint64_t &default_delay,
                        ScenariofileParser::Table::TableMap::iterator it);

  void parseNodeAttributes();
  void parseRequests();

  // peer discovery environment

  struct NodeAttribute {
    std::string name;

    minhton::NodeData::ValueType value_type;

    float presence_percentage;

    // delay in milliseconds
    Dist<uint64_t> update_delay_distribution;

    ns3::Ptr<ns3::RandomVariableStream> content_distribution;

    // false -> NodeData::Values
    bool content_is_numerical;

    // ns3 random variable streams only return doubles
    // -> mapping a casted double to the actual value, specified in the testfile
    std::unordered_map<uint8_t, minhton::NodeData::Value> empirical_value_mapping;

    minhton::NodeData::Value getRandomContent() {
      minhton::NodeData::Value value;

      if (content_is_numerical) {
        value = (float)content_distribution->GetValue();
      } else {
        uint8_t num_val = content_distribution->GetInteger();
        value = empirical_value_mapping[num_val];
      }
      return value;
    }
  };
  std::vector<NodeAttribute> node_content_attributes_;
  std::vector<std::string> attribute_names_;

  struct {
    Dist<uint8_t> query_depth_distribution;
    Dist<uint64_t> request_delay_distribution;
    Dist<uint64_t> validity_threshold_distribution;

    std::optional<float> random_nodes_percentage;
    std::optional<int> absolute_nodes_number;

    bool query_inquire_outdated = false;
    bool query_inquire_unknown = false;

  } requests_;

  void initiatePeerDiscoverEnvironmentAfterStaticBuild(ns3::Ptr<MinhtonNodeNs3> node_ns3,
                                                       uint64_t index);
  void initiatePeerDiscoverEnvironmentAfterJoin(ns3::Ptr<MinhtonNodeNs3> node_ns3);
  void initiatePeerDiscoverEnvironment(ns3::Ptr<MinhtonNodeNs3> node_ns3,
                                       uint64_t init_content_delay, uint64_t init_request_delay);

  void initiateNodeContent(ns3::Ptr<MinhtonNodeNs3> node_ns3);
  void updateNodeAttribute(ns3::Ptr<MinhtonNodeNs3> node_ns3, minhton::NodeData::Key key);

  void initiateRequestsOnAllNodes();
  void makeRequest(ns3::Ptr<MinhtonNodeNs3> node_ns3);
  minhton::FindQuery createFindQuery(uint8_t depth);
  std::shared_ptr<minhton::BooleanExpression> createBooleanExpression(uint8_t depth);
  std::shared_ptr<minhton::BooleanExpression> createBooleanExpressionRecursive(
      std::vector<minhton::NodeData::Key> open_keys);
  std::shared_ptr<minhton::BooleanExpression> createBooleanExpressionForKey(
      minhton::NodeData::Key key);
  void activateRequestCountdown(uint16_t limit);

  std::uniform_real_distribution<double> uniform_zero_one_distribution_;

  bool attributes_off_ = false;
  bool requests_off_ = false;

  uint16_t requests_counter_ = 0;
  uint16_t requests_limit_ = 0;
  bool requests_limit_on_ = false;

  std::vector<std::string> requesting_nodes_ips_;
};

}  // namespace daisi::minhton_ns3

#endif
