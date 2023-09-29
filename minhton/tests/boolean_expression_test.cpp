// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#include "algorithms/esearch/boolean_expression.h"

#include <catch2/catch_test_macros.hpp>
#include <memory>

#include "algorithms/esearch/distributed_data.h"
#include "algorithms/esearch/local_data.h"

using namespace minhton;

TEST_CASE("BooleanExpression PresenceExpression, Inquire all",
          "[BooleanExpression][PresenceExpression]") {
  PresenceExpression expr("topicA");

  // everything below threshold (older) is invalid if not subscribed
  uint64_t timestamp_now = 1000;
  uint64_t threshold1 = 900;
  uint64_t threshold2 = 1100;

  EvaluationInformation eval_info_1_1{threshold1, true, true, true};

  EvaluationInformation eval_info_2_1{threshold2, true, true, true};

  EvaluationInformation eval_info_1_2{threshold1,
                                      false,  // not all information present
                                      true, true};

  EvaluationInformation eval_info_2_2{threshold2,
                                      false,  // not all information present
                                      true, true};

  DistributedData data1;
  REQUIRE(data1.insert("topicA", {true, timestamp_now, NodeData::ValueType::kValueDynamic}));

  REQUIRE(expr.evaluateMissingAttributes(data1, eval_info_1_1).size() == 0);
  REQUIRE(expr.evaluateMissingAttributes(data1, eval_info_2_1)[0] == "topicA");
  REQUIRE(expr.getRelevantKeys()[0] == "topicA");
  REQUIRE(expr.getRelevantTopicKeys()[0] == "topicA");

  REQUIRE(expr.evaluate(data1, eval_info_1_1).isTrue());
  REQUIRE(expr.evaluate(data1, eval_info_2_1).isFalse());
  REQUIRE(expr.evaluate(data1, eval_info_1_2).isTrue());
  REQUIRE(expr.evaluate(data1, eval_info_2_2).isUndecided());

  DistributedData data2;
  REQUIRE(data2.insert("topicB", {true, timestamp_now, NodeData::ValueType::kValueDynamic}));

  REQUIRE(expr.evaluateMissingAttributes(data2, eval_info_1_1).size() == 1);
  REQUIRE(expr.evaluateMissingAttributes(data2, eval_info_2_1).size() == 1);

  REQUIRE(expr.evaluate(data2, eval_info_1_1).isFalse());
  REQUIRE(expr.evaluate(data2, eval_info_2_1).isFalse());
  REQUIRE(expr.evaluate(data2, eval_info_1_2).isUndecided());
  REQUIRE(expr.evaluate(data2, eval_info_2_2).isUndecided());

  DistributedData data3;
  REQUIRE(data3.insert("topicA", {false, timestamp_now, NodeData::ValueType::kValueDynamic}));

  REQUIRE(expr.evaluateMissingAttributes(data3, eval_info_1_1).size() == 0);
  REQUIRE(expr.evaluateMissingAttributes(data3, eval_info_2_1).size() == 1);

  REQUIRE(expr.evaluate(data3, eval_info_1_1).isFalse());
  REQUIRE(expr.evaluate(data3, eval_info_2_1).isFalse());
  REQUIRE(expr.evaluate(data3, eval_info_1_2).isFalse());
  REQUIRE(expr.evaluate(data3, eval_info_2_2).isUndecided());

  LocalData local1;
  REQUIRE(local1.insert("topicA", {true, timestamp_now, NodeData::ValueType::kValueDynamic}));

  REQUIRE(expr.evaluateMissingAttributes(local1, eval_info_1_1).size() == 0);
  REQUIRE(expr.evaluateMissingAttributes(local1, eval_info_2_1).size() == 0);

  REQUIRE(expr.evaluate(local1, eval_info_1_1).isTrue());
  REQUIRE(expr.evaluate(local1, eval_info_2_1).isTrue());
  REQUIRE(expr.evaluate(local1, eval_info_1_2).isTrue());
  REQUIRE(expr.evaluate(local1, eval_info_2_2).isTrue());

  LocalData local2;
  REQUIRE(local2.insert("topicA", {false, timestamp_now, NodeData::ValueType::kValueDynamic}));

  REQUIRE(expr.evaluateMissingAttributes(local2, eval_info_1_1).size() == 0);
  REQUIRE(expr.evaluateMissingAttributes(local2, eval_info_2_1).size() == 0);

  REQUIRE(expr.evaluate(local2, eval_info_1_1).isFalse());
  REQUIRE(expr.evaluate(local2, eval_info_2_1).isFalse());
  REQUIRE(expr.evaluate(local2, eval_info_1_2).isFalse());
  REQUIRE(expr.evaluate(local2, eval_info_2_2).isFalse());
}

TEST_CASE("BooleanExpression PresenceExpression, Different Inquiries",
          "[BooleanExpression][PresenceExpression]") {
  PresenceExpression expr("topicA");

  uint64_t timestamp_now = 1000;
  uint64_t threshold1 = 900;
  uint64_t threshold2 = 1100;

  // inquire unknown = false
  EvaluationInformation eval_info_1_1_no_unknown{threshold1, true, false, true};
  EvaluationInformation eval_info_2_1_no_unknown{threshold2, true, false, true};
  EvaluationInformation eval_info_1_2_no_unknown{threshold1, false, false, true};
  EvaluationInformation eval_info_2_2_no_unknown{threshold2, false, false, true};

  // inquire outdated = false
  EvaluationInformation eval_info_1_1_no_outdated{threshold1, true, true, false};
  EvaluationInformation eval_info_2_1_no_outdated{threshold2, true, true, false};
  EvaluationInformation eval_info_1_2_no_outdated{threshold1, false, true, false};
  EvaluationInformation eval_info_2_2_no_outdated{threshold2, false, true, false};

  // inquire unkonwn = false, inquire outdated = false
  EvaluationInformation eval_info_1_1_none{threshold1, true, false, false};
  EvaluationInformation eval_info_2_1_none{threshold2, true, false, false};
  EvaluationInformation eval_info_1_2_none{threshold1, false, false, false};
  EvaluationInformation eval_info_2_2_none{threshold2, false, false, false};

  DistributedData data1;
  REQUIRE(data1.insert("topicA", {true, timestamp_now, NodeData::ValueType::kValueDynamic}));

  REQUIRE(expr.evaluateMissingAttributes(data1, eval_info_1_1_no_unknown).size() == 0);
  REQUIRE(expr.evaluateMissingAttributes(data1, eval_info_2_1_no_unknown)[0] == "topicA");
  REQUIRE(expr.evaluateMissingAttributes(data1, eval_info_1_2_no_unknown).size() == 0);
  REQUIRE(expr.evaluateMissingAttributes(data1, eval_info_2_2_no_unknown)[0] == "topicA");

  REQUIRE(expr.evaluate(data1, eval_info_1_1_no_unknown).isTrue());
  REQUIRE(expr.evaluate(data1, eval_info_2_1_no_unknown).isFalse());  // true
  REQUIRE(expr.evaluate(data1, eval_info_1_2_no_unknown).isTrue());
  REQUIRE(expr.evaluate(data1, eval_info_2_2_no_unknown).isUndecided());

  REQUIRE(expr.evaluateMissingAttributes(data1, eval_info_1_1_no_outdated).size() == 0);
  REQUIRE(expr.evaluateMissingAttributes(data1, eval_info_2_1_no_outdated).size() == 0);
  REQUIRE(expr.evaluateMissingAttributes(data1, eval_info_1_2_no_outdated).size() == 0);
  REQUIRE(expr.evaluateMissingAttributes(data1, eval_info_2_2_no_outdated).size() == 0);

  REQUIRE(expr.evaluate(data1, eval_info_1_1_no_outdated).isTrue());
  REQUIRE(expr.evaluate(data1, eval_info_2_1_no_outdated).isTrue());
  REQUIRE(expr.evaluate(data1, eval_info_1_2_no_outdated).isTrue());
  REQUIRE(expr.evaluate(data1, eval_info_2_2_no_outdated).isTrue());

  REQUIRE(expr.evaluateMissingAttributes(data1, eval_info_1_1_none).size() == 0);
  REQUIRE(expr.evaluateMissingAttributes(data1, eval_info_2_1_none).size() == 0);
  REQUIRE(expr.evaluateMissingAttributes(data1, eval_info_1_2_none).size() == 0);
  REQUIRE(expr.evaluateMissingAttributes(data1, eval_info_2_2_none).size() == 0);

  REQUIRE(expr.evaluate(data1, eval_info_1_1_none).isTrue());
  REQUIRE(expr.evaluate(data1, eval_info_2_1_none).isTrue());
  REQUIRE(expr.evaluate(data1, eval_info_1_2_none).isTrue());
  REQUIRE(expr.evaluate(data1, eval_info_2_2_none).isTrue());

  DistributedData data2;
  REQUIRE(data2.insert("topicB", {true, timestamp_now, NodeData::ValueType::kValueDynamic}));

  REQUIRE(expr.evaluateMissingAttributes(data2, eval_info_1_1_no_unknown).size() == 0);
  REQUIRE(expr.evaluateMissingAttributes(data2, eval_info_2_1_no_unknown).size() == 0);
  REQUIRE(expr.evaluateMissingAttributes(data2, eval_info_1_2_no_unknown).size() == 0);
  REQUIRE(expr.evaluateMissingAttributes(data2, eval_info_2_2_no_unknown).size() == 0);

  REQUIRE(expr.evaluate(data2, eval_info_1_1_no_unknown).isFalse());
  REQUIRE(expr.evaluate(data2, eval_info_2_1_no_unknown).isFalse());
  REQUIRE(expr.evaluate(data2, eval_info_1_2_no_unknown).isFalse());
  REQUIRE(expr.evaluate(data2, eval_info_2_2_no_unknown).isFalse());

  REQUIRE(expr.evaluateMissingAttributes(data2, eval_info_1_1_no_unknown).size() == 0);
  REQUIRE(expr.evaluateMissingAttributes(data2, eval_info_2_1_no_unknown).size() == 0);
  REQUIRE(expr.evaluateMissingAttributes(data2, eval_info_1_2_no_unknown).size() == 0);
  REQUIRE(expr.evaluateMissingAttributes(data2, eval_info_2_2_no_unknown).size() == 0);

  REQUIRE(expr.evaluate(data2, eval_info_1_1_no_outdated).isFalse());
  REQUIRE(expr.evaluate(data2, eval_info_2_1_no_outdated).isFalse());
  REQUIRE(expr.evaluate(data2, eval_info_1_2_no_outdated).isUndecided());
  REQUIRE(expr.evaluate(data2, eval_info_2_2_no_outdated).isUndecided());

  REQUIRE(expr.evaluateMissingAttributes(data2, eval_info_1_1_none).size() == 0);
  REQUIRE(expr.evaluateMissingAttributes(data2, eval_info_2_1_none).size() == 0);
  REQUIRE(expr.evaluateMissingAttributes(data2, eval_info_1_2_none).size() == 0);
  REQUIRE(expr.evaluateMissingAttributes(data2, eval_info_2_2_none).size() == 0);

  REQUIRE(expr.evaluate(data2, eval_info_1_1_none).isFalse());
  REQUIRE(expr.evaluate(data2, eval_info_2_1_none).isFalse());
  REQUIRE(expr.evaluate(data2, eval_info_1_2_none).isFalse());
  REQUIRE(expr.evaluate(data2, eval_info_2_2_none).isFalse());

  DistributedData data3;
  REQUIRE(data3.insert("topicA", {false, timestamp_now, NodeData::ValueType::kValueDynamic}));

  REQUIRE(expr.evaluateMissingAttributes(data3, eval_info_1_1_no_unknown).size() == 0);
  REQUIRE(expr.evaluateMissingAttributes(data3, eval_info_2_1_no_unknown).size() == 1);
  REQUIRE(expr.evaluateMissingAttributes(data3, eval_info_1_2_no_unknown).size() == 0);
  REQUIRE(expr.evaluateMissingAttributes(data3, eval_info_2_2_no_unknown).size() == 1);

  REQUIRE(expr.evaluate(data3, eval_info_1_1_no_unknown).isFalse());
  REQUIRE(expr.evaluate(data3, eval_info_2_1_no_unknown).isFalse());
  REQUIRE(expr.evaluate(data3, eval_info_1_2_no_unknown).isFalse());
  REQUIRE(expr.evaluate(data3, eval_info_2_2_no_unknown).isUndecided());

  REQUIRE(expr.evaluateMissingAttributes(data3, eval_info_1_1_no_outdated).size() == 0);
  REQUIRE(expr.evaluateMissingAttributes(data3, eval_info_2_1_no_outdated).size() == 0);
  REQUIRE(expr.evaluateMissingAttributes(data3, eval_info_1_2_no_outdated).size() == 0);
  REQUIRE(expr.evaluateMissingAttributes(data3, eval_info_2_2_no_outdated).size() == 0);

  REQUIRE(expr.evaluate(data3, eval_info_1_1_no_outdated).isFalse());
  REQUIRE(expr.evaluate(data3, eval_info_2_1_no_outdated).isFalse());
  REQUIRE(expr.evaluate(data3, eval_info_1_2_no_outdated).isFalse());
  REQUIRE(expr.evaluate(data3, eval_info_2_2_no_outdated).isFalse());

  REQUIRE(expr.evaluateMissingAttributes(data3, eval_info_1_1_none).size() == 0);
  REQUIRE(expr.evaluateMissingAttributes(data3, eval_info_2_1_none).size() == 0);
  REQUIRE(expr.evaluateMissingAttributes(data3, eval_info_1_2_none).size() == 0);
  REQUIRE(expr.evaluateMissingAttributes(data3, eval_info_2_2_none).size() == 0);

  REQUIRE(expr.evaluate(data3, eval_info_1_1_none).isFalse());
  REQUIRE(expr.evaluate(data3, eval_info_2_1_none).isFalse());
  REQUIRE(expr.evaluate(data3, eval_info_1_2_none).isFalse());
  REQUIRE(expr.evaluate(data3, eval_info_2_2_none).isFalse());

  LocalData local1;
  REQUIRE(local1.insert("topicA", {true, timestamp_now, NodeData::ValueType::kValueDynamic}));

  REQUIRE(expr.evaluateMissingAttributes(local1, eval_info_1_1_no_unknown).size() == 0);
  REQUIRE(expr.evaluateMissingAttributes(local1, eval_info_2_1_no_unknown).size() == 0);
  REQUIRE(expr.evaluateMissingAttributes(local1, eval_info_1_2_no_unknown).size() == 0);
  REQUIRE(expr.evaluateMissingAttributes(local1, eval_info_2_2_no_unknown).size() == 0);

  REQUIRE(expr.evaluate(local1, eval_info_1_1_no_unknown).isTrue());
  REQUIRE(expr.evaluate(local1, eval_info_2_1_no_unknown).isTrue());
  REQUIRE(expr.evaluate(local1, eval_info_1_2_no_unknown).isTrue());
  REQUIRE(expr.evaluate(local1, eval_info_2_2_no_unknown).isTrue());

  REQUIRE(expr.evaluateMissingAttributes(local1, eval_info_1_1_no_outdated).size() == 0);
  REQUIRE(expr.evaluateMissingAttributes(local1, eval_info_2_1_no_outdated).size() == 0);
  REQUIRE(expr.evaluateMissingAttributes(local1, eval_info_1_2_no_outdated).size() == 0);
  REQUIRE(expr.evaluateMissingAttributes(local1, eval_info_2_2_no_outdated).size() == 0);

  REQUIRE(expr.evaluate(local1, eval_info_1_1_no_outdated).isTrue());
  REQUIRE(expr.evaluate(local1, eval_info_2_1_no_outdated).isTrue());
  REQUIRE(expr.evaluate(local1, eval_info_1_2_no_outdated).isTrue());
  REQUIRE(expr.evaluate(local1, eval_info_2_2_no_outdated).isTrue());

  REQUIRE(expr.evaluateMissingAttributes(local1, eval_info_1_1_none).size() == 0);
  REQUIRE(expr.evaluateMissingAttributes(local1, eval_info_2_1_none).size() == 0);
  REQUIRE(expr.evaluateMissingAttributes(local1, eval_info_1_2_none).size() == 0);
  REQUIRE(expr.evaluateMissingAttributes(local1, eval_info_2_2_none).size() == 0);

  REQUIRE(expr.evaluate(local1, eval_info_1_1_none).isTrue());
  REQUIRE(expr.evaluate(local1, eval_info_2_1_none).isTrue());
  REQUIRE(expr.evaluate(local1, eval_info_1_2_none).isTrue());
  REQUIRE(expr.evaluate(local1, eval_info_2_2_none).isTrue());

  LocalData local2;
  REQUIRE(local2.insert("topicA", {false, timestamp_now, NodeData::ValueType::kValueDynamic}));

  REQUIRE(expr.evaluateMissingAttributes(local2, eval_info_1_1_no_unknown).size() == 0);
  REQUIRE(expr.evaluateMissingAttributes(local2, eval_info_2_1_no_unknown).size() == 0);
  REQUIRE(expr.evaluateMissingAttributes(local2, eval_info_1_2_no_unknown).size() == 0);
  REQUIRE(expr.evaluateMissingAttributes(local2, eval_info_2_2_no_unknown).size() == 0);

  REQUIRE(expr.evaluate(local2, eval_info_1_1_no_unknown).isFalse());
  REQUIRE(expr.evaluate(local2, eval_info_2_1_no_unknown).isFalse());
  REQUIRE(expr.evaluate(local2, eval_info_1_2_no_unknown).isFalse());
  REQUIRE(expr.evaluate(local2, eval_info_2_2_no_unknown).isFalse());

  REQUIRE(expr.evaluateMissingAttributes(local2, eval_info_1_1_no_outdated).size() == 0);
  REQUIRE(expr.evaluateMissingAttributes(local2, eval_info_2_1_no_outdated).size() == 0);
  REQUIRE(expr.evaluateMissingAttributes(local2, eval_info_1_2_no_outdated).size() == 0);
  REQUIRE(expr.evaluateMissingAttributes(local2, eval_info_2_2_no_outdated).size() == 0);

  REQUIRE(expr.evaluate(local2, eval_info_1_1_no_outdated).isFalse());
  REQUIRE(expr.evaluate(local2, eval_info_2_1_no_outdated).isFalse());
  REQUIRE(expr.evaluate(local2, eval_info_1_2_no_outdated).isFalse());
  REQUIRE(expr.evaluate(local2, eval_info_2_2_no_outdated).isFalse());

  REQUIRE(expr.evaluateMissingAttributes(local2, eval_info_1_1_none).size() == 0);
  REQUIRE(expr.evaluateMissingAttributes(local2, eval_info_2_1_none).size() == 0);
  REQUIRE(expr.evaluateMissingAttributes(local2, eval_info_1_2_none).size() == 0);
  REQUIRE(expr.evaluateMissingAttributes(local2, eval_info_2_2_none).size() == 0);

  REQUIRE(expr.evaluate(local2, eval_info_1_1_none).isFalse());
  REQUIRE(expr.evaluate(local2, eval_info_2_1_none).isFalse());
  REQUIRE(expr.evaluate(local2, eval_info_1_2_none).isFalse());
  REQUIRE(expr.evaluate(local2, eval_info_2_2_none).isFalse());
}

TEST_CASE("BooleanExpression StringEqualityExpression, Inquire all",
          "[BooleanExpression][StringEqualityExpression]") {
  StringEqualityExpression expr("wetter", "schlecht");

  // everything below threshold (older) is invalid if not subscribed
  uint64_t timestamp_now = 1000;
  uint64_t threshold1 = 900;
  uint64_t threshold2 = 1100;

  EvaluationInformation eval_info_1_1{threshold1, true, true, true};

  EvaluationInformation eval_info_2_1{threshold2, true, true, true};

  EvaluationInformation eval_info_1_2{threshold1,
                                      false,  // not all information present
                                      true, true};

  EvaluationInformation eval_info_2_2{threshold2,
                                      false,  // not all information present
                                      true, true};

  DistributedData data1;
  std::string value1 = "schlecht";
  REQUIRE(data1.insert("wetter", {value1, timestamp_now, NodeData::ValueType::kValueDynamic}));

  REQUIRE(expr.evaluateMissingAttributes(data1, eval_info_1_1).empty());
  REQUIRE(expr.evaluateMissingAttributes(data1, eval_info_2_1)[0] == "wetter");
  REQUIRE(expr.getRelevantKeys()[0] == "wetter");
  REQUIRE(expr.getRelevantTopicKeys().empty());

  REQUIRE(expr.evaluate(data1, eval_info_1_1).isTrue());
  REQUIRE(expr.evaluate(data1, eval_info_2_1).isFalse());
  REQUIRE(expr.evaluate(data1, eval_info_1_2).isTrue());
  REQUIRE(expr.evaluate(data1, eval_info_2_2).isUndecided());

  DistributedData data2;
  std::string value2 = "gut";
  REQUIRE(data2.insert("wetter", {value2, timestamp_now, NodeData::ValueType::kValueDynamic}));

  REQUIRE(expr.evaluateMissingAttributes(data2, eval_info_1_1).empty());
  REQUIRE(expr.evaluateMissingAttributes(data2, eval_info_2_1)[0] == "wetter");

  REQUIRE(expr.evaluate(data2, eval_info_1_1).isFalse());
  REQUIRE(expr.evaluate(data2, eval_info_2_1).isFalse());
  REQUIRE(expr.evaluate(data2, eval_info_1_2).isFalse());
  REQUIRE(expr.evaluate(data2, eval_info_2_2).isUndecided());

  DistributedData data3;
  REQUIRE(
      data3.insert("kenn kein wetter", {99999, timestamp_now, NodeData::ValueType::kValueDynamic}));

  REQUIRE(expr.evaluateMissingAttributes(data3, eval_info_1_1).size() == 1);
  REQUIRE(expr.evaluateMissingAttributes(data3, eval_info_2_1).size() == 1);
  REQUIRE(expr.evaluateMissingAttributes(data3, eval_info_2_1)[0] == "wetter");

  REQUIRE(expr.evaluate(data3, eval_info_1_1).isFalse());
  REQUIRE(expr.evaluate(data3, eval_info_2_1).isFalse());
  REQUIRE(expr.evaluate(data3, eval_info_1_2).isUndecided());
  REQUIRE(expr.evaluate(data3, eval_info_2_2).isUndecided());

  LocalData local1;
  std::string value3 = "schlecht";
  REQUIRE(local1.insert("wetter", {value3, timestamp_now, NodeData::ValueType::kValueDynamic}));

  REQUIRE(expr.evaluateMissingAttributes(local1, eval_info_1_1).empty());
  REQUIRE(expr.evaluateMissingAttributes(local1, eval_info_2_1).empty());

  REQUIRE(expr.evaluate(local1, eval_info_1_1).isTrue());
  REQUIRE(expr.evaluate(local1, eval_info_2_1).isTrue());
  REQUIRE(expr.evaluate(local1, eval_info_1_2).isTrue());
  REQUIRE(expr.evaluate(local1, eval_info_2_2).isTrue());

  LocalData local2;
  std::string value4 = "gut";
  REQUIRE(local2.insert("wetter", {value4, timestamp_now, NodeData::ValueType::kValueDynamic}));

  REQUIRE(expr.evaluateMissingAttributes(local2, eval_info_1_1).empty());
  REQUIRE(expr.evaluateMissingAttributes(local2, eval_info_2_1).empty());

  REQUIRE(expr.evaluate(local2, eval_info_1_1).isFalse());
  REQUIRE(expr.evaluate(local2, eval_info_2_1).isFalse());
  REQUIRE(expr.evaluate(local2, eval_info_1_2).isFalse());
  REQUIRE(expr.evaluate(local2, eval_info_2_2).isFalse());
}

TEST_CASE("BooleanExpression StringEqualityExpression, Different Inquiries",
          "[BooleanExpression][StringEqualityExpression]") {
  StringEqualityExpression expr("wetter", "schlecht");

  // everything below threshold (older) is invalid if not subscribed
  uint64_t timestamp_now = 1000;
  uint64_t threshold1 = 900;
  uint64_t threshold2 = 1100;

  EvaluationInformation eval_info_1_1_no_unknown{threshold1, true, false, true};
  EvaluationInformation eval_info_2_1_no_unknown{threshold2, true, false, true};
  EvaluationInformation eval_info_1_2_no_unknown{threshold1, false, false, true};
  EvaluationInformation eval_info_2_2_no_unknown{threshold2, false, false, true};

  EvaluationInformation eval_info_1_1_no_outdated{threshold1, true, true, false};
  EvaluationInformation eval_info_2_1_no_outdated{threshold2, true, true, false};
  EvaluationInformation eval_info_1_2_no_outdated{threshold1, false, true, false};
  EvaluationInformation eval_info_2_2_no_outdated{threshold2, false, true, false};

  EvaluationInformation eval_info_1_1_none{threshold1, true, false, false};
  EvaluationInformation eval_info_2_1_none{threshold2, true, false, false};
  EvaluationInformation eval_info_1_2_none{threshold1, false, false, false};
  EvaluationInformation eval_info_2_2_none{threshold2, false, false, false};

  DistributedData data1;
  std::string value1 = "schlecht";
  REQUIRE(data1.insert("wetter", {value1, timestamp_now, NodeData::ValueType::kValueDynamic}));

  REQUIRE(expr.evaluateMissingAttributes(data1, eval_info_1_1_no_unknown).empty());
  REQUIRE(expr.evaluateMissingAttributes(data1, eval_info_2_1_no_unknown).size() == 1);

  REQUIRE(expr.evaluate(data1, eval_info_1_1_no_unknown).isTrue());
  REQUIRE(expr.evaluate(data1, eval_info_2_1_no_unknown).isFalse());
  REQUIRE(expr.evaluate(data1, eval_info_1_2_no_unknown).isTrue());
  REQUIRE(expr.evaluate(data1, eval_info_2_2_no_unknown).isUndecided());

  REQUIRE(expr.evaluateMissingAttributes(data1, eval_info_1_1_no_outdated).empty());
  REQUIRE(expr.evaluateMissingAttributes(data1, eval_info_2_1_no_outdated).empty());

  REQUIRE(expr.evaluate(data1, eval_info_1_1_no_outdated).isTrue());
  REQUIRE(expr.evaluate(data1, eval_info_2_1_no_outdated).isTrue());
  REQUIRE(expr.evaluate(data1, eval_info_1_2_no_outdated).isTrue());
  REQUIRE(expr.evaluate(data1, eval_info_2_2_no_outdated).isTrue());

  REQUIRE(expr.evaluateMissingAttributes(data1, eval_info_1_1_none).empty());
  REQUIRE(expr.evaluateMissingAttributes(data1, eval_info_2_1_none).empty());

  REQUIRE(expr.evaluate(data1, eval_info_1_1_none).isTrue());
  REQUIRE(expr.evaluate(data1, eval_info_2_1_none).isTrue());
  REQUIRE(expr.evaluate(data1, eval_info_1_2_none).isTrue());
  REQUIRE(expr.evaluate(data1, eval_info_2_2_none).isTrue());

  DistributedData data2;
  std::string value2 = "gut";
  REQUIRE(data2.insert("wetter", {value2, timestamp_now, NodeData::ValueType::kValueDynamic}));

  REQUIRE(expr.evaluateMissingAttributes(data2, eval_info_1_1_no_unknown).empty());
  REQUIRE(expr.evaluateMissingAttributes(data2, eval_info_2_1_no_unknown).size() == 1);

  REQUIRE(expr.evaluate(data2, eval_info_1_1_no_unknown).isFalse());
  REQUIRE(expr.evaluate(data2, eval_info_2_1_no_unknown).isFalse());
  REQUIRE(expr.evaluate(data2, eval_info_1_2_no_unknown).isFalse());
  REQUIRE(expr.evaluate(data2, eval_info_2_2_no_unknown).isUndecided());

  REQUIRE(expr.evaluateMissingAttributes(data2, eval_info_1_1_no_outdated).empty());
  REQUIRE(expr.evaluateMissingAttributes(data2, eval_info_2_1_no_outdated).empty());

  REQUIRE(expr.evaluate(data2, eval_info_1_1_no_outdated).isFalse());
  REQUIRE(expr.evaluate(data2, eval_info_2_1_no_outdated).isFalse());
  REQUIRE(expr.evaluate(data2, eval_info_1_2_no_outdated).isFalse());
  REQUIRE(expr.evaluate(data2, eval_info_2_2_no_outdated).isFalse());

  REQUIRE(expr.evaluateMissingAttributes(data2, eval_info_1_1_none).empty());
  REQUIRE(expr.evaluateMissingAttributes(data2, eval_info_2_1_none).empty());

  REQUIRE(expr.evaluate(data2, eval_info_1_1_none).isFalse());
  REQUIRE(expr.evaluate(data2, eval_info_2_1_none).isFalse());
  REQUIRE(expr.evaluate(data2, eval_info_1_2_none).isFalse());
  REQUIRE(expr.evaluate(data2, eval_info_2_2_none).isFalse());

  DistributedData data3;
  REQUIRE(
      data3.insert("kenn kein wetter", {99999, timestamp_now, NodeData::ValueType::kValueDynamic}));

  REQUIRE(expr.evaluateMissingAttributes(data3, eval_info_1_1_no_unknown).empty());
  REQUIRE(expr.evaluateMissingAttributes(data3, eval_info_2_1_no_unknown).empty());

  REQUIRE(expr.evaluate(data3, eval_info_1_1_no_unknown).isFalse());
  REQUIRE(expr.evaluate(data3, eval_info_2_1_no_unknown).isFalse());
  REQUIRE(expr.evaluate(data3, eval_info_1_2_no_unknown).isFalse());
  REQUIRE(expr.evaluate(data3, eval_info_2_2_no_unknown).isFalse());

  REQUIRE(expr.evaluateMissingAttributes(data3, eval_info_1_1_no_outdated).size() == 1);
  REQUIRE(expr.evaluateMissingAttributes(data3, eval_info_2_1_no_outdated).size() == 1);

  REQUIRE(expr.evaluate(data3, eval_info_1_1_no_outdated).isFalse());
  REQUIRE(expr.evaluate(data3, eval_info_2_1_no_outdated).isFalse());
  REQUIRE(expr.evaluate(data3, eval_info_1_2_no_outdated).isUndecided());
  REQUIRE(expr.evaluate(data3, eval_info_2_2_no_outdated).isUndecided());

  REQUIRE(expr.evaluateMissingAttributes(data3, eval_info_1_1_none).empty());
  REQUIRE(expr.evaluateMissingAttributes(data3, eval_info_2_1_none).empty());

  REQUIRE(expr.evaluate(data3, eval_info_1_1_none).isFalse());
  REQUIRE(expr.evaluate(data3, eval_info_2_1_none).isFalse());
  REQUIRE(expr.evaluate(data3, eval_info_1_2_none).isFalse());
  REQUIRE(expr.evaluate(data3, eval_info_2_2_none).isFalse());

  LocalData local1;
  std::string value3 = "schlecht";
  REQUIRE(local1.insert("wetter", {value3, timestamp_now, NodeData::ValueType::kValueDynamic}));

  REQUIRE(expr.evaluateMissingAttributes(local1, eval_info_1_1_no_unknown).empty());
  REQUIRE(expr.evaluateMissingAttributes(local1, eval_info_2_1_no_unknown).empty());

  REQUIRE(expr.evaluate(local1, eval_info_1_1_no_unknown).isTrue());
  REQUIRE(expr.evaluate(local1, eval_info_2_1_no_unknown).isTrue());
  REQUIRE(expr.evaluate(local1, eval_info_1_2_no_unknown).isTrue());
  REQUIRE(expr.evaluate(local1, eval_info_2_2_no_unknown).isTrue());

  REQUIRE(expr.evaluateMissingAttributes(local1, eval_info_1_1_no_outdated).empty());
  REQUIRE(expr.evaluateMissingAttributes(local1, eval_info_2_1_no_outdated).empty());

  REQUIRE(expr.evaluate(local1, eval_info_1_1_no_outdated).isTrue());
  REQUIRE(expr.evaluate(local1, eval_info_2_1_no_outdated).isTrue());
  REQUIRE(expr.evaluate(local1, eval_info_1_2_no_outdated).isTrue());
  REQUIRE(expr.evaluate(local1, eval_info_2_2_no_outdated).isTrue());

  REQUIRE(expr.evaluateMissingAttributes(local1, eval_info_1_1_none).empty());
  REQUIRE(expr.evaluateMissingAttributes(local1, eval_info_2_1_none).empty());

  REQUIRE(expr.evaluate(local1, eval_info_1_1_none).isTrue());
  REQUIRE(expr.evaluate(local1, eval_info_2_1_none).isTrue());
  REQUIRE(expr.evaluate(local1, eval_info_1_2_none).isTrue());
  REQUIRE(expr.evaluate(local1, eval_info_2_2_none).isTrue());

  LocalData local2;
  std::string value4 = "gut";
  REQUIRE(local2.insert("wetter", {value4, timestamp_now, NodeData::ValueType::kValueDynamic}));

  REQUIRE(expr.evaluateMissingAttributes(local2, eval_info_1_1_no_unknown).empty());
  REQUIRE(expr.evaluateMissingAttributes(local2, eval_info_2_1_no_unknown).empty());

  REQUIRE(expr.evaluate(local2, eval_info_1_1_no_unknown).isFalse());
  REQUIRE(expr.evaluate(local2, eval_info_2_1_no_unknown).isFalse());
  REQUIRE(expr.evaluate(local2, eval_info_1_2_no_unknown).isFalse());
  REQUIRE(expr.evaluate(local2, eval_info_2_2_no_unknown).isFalse());

  REQUIRE(expr.evaluateMissingAttributes(local2, eval_info_1_1_no_outdated).empty());
  REQUIRE(expr.evaluateMissingAttributes(local2, eval_info_2_1_no_outdated).empty());

  REQUIRE(expr.evaluate(local2, eval_info_1_1_no_outdated).isFalse());
  REQUIRE(expr.evaluate(local2, eval_info_2_1_no_outdated).isFalse());
  REQUIRE(expr.evaluate(local2, eval_info_1_2_no_outdated).isFalse());
  REQUIRE(expr.evaluate(local2, eval_info_2_2_no_outdated).isFalse());

  REQUIRE(expr.evaluateMissingAttributes(local2, eval_info_1_1_none).empty());
  REQUIRE(expr.evaluateMissingAttributes(local2, eval_info_2_1_none).empty());

  REQUIRE(expr.evaluate(local2, eval_info_1_1_none).isFalse());
  REQUIRE(expr.evaluate(local2, eval_info_2_1_none).isFalse());
  REQUIRE(expr.evaluate(local2, eval_info_1_2_none).isFalse());
  REQUIRE(expr.evaluate(local2, eval_info_2_2_none).isFalse());
}

TEST_CASE("BooleanExpression NumericComparisonExpression, Inquire all",
          "[BooleanExpression][NumericComparisonExpression]") {
  uint64_t timestamp_now = 1000;
  uint64_t threshold1 = 900;
  uint64_t threshold2 = 1100;

  EvaluationInformation eval_info_1_1{threshold1, true, true, true};

  EvaluationInformation eval_info_2_1{threshold2, true, true, true};

  EvaluationInformation eval_info_1_2{threshold1,
                                      false,  // not all information present
                                      true, true};

  EvaluationInformation eval_info_2_2{threshold2,
                                      false,  // not all information present
                                      true, true};

  SECTION("equal_to") {
    NumericComparisonExpression<int> expr("weight", ComparisonTypes::kEqualTo, 100);

    REQUIRE(expr.getRelevantKeys().size() == 1);
    REQUIRE(expr.getRelevantKeys()[0] == "weight");
    REQUIRE(expr.getRelevantTopicKeys().empty());

    DistributedData data1;
    REQUIRE(data1.insert("weight", {100, timestamp_now, NodeData::ValueType::kValueDynamic}));

    REQUIRE(expr.evaluateMissingAttributes(data1, eval_info_1_1).empty());
    REQUIRE(expr.evaluateMissingAttributes(data1, eval_info_2_1).size() == 1);
    REQUIRE(expr.evaluateMissingAttributes(data1, eval_info_2_1)[0] == "weight");

    REQUIRE(expr.evaluate(data1, eval_info_1_1).isTrue());
    REQUIRE(expr.evaluate(data1, eval_info_2_1).isFalse());
    REQUIRE(expr.evaluate(data1, eval_info_1_2).isTrue());
    REQUIRE(expr.evaluate(data1, eval_info_2_2).isUndecided());

    DistributedData data2;
    REQUIRE(data2.insert("weight", {99, timestamp_now, NodeData::ValueType::kValueDynamic}));

    REQUIRE(expr.evaluateMissingAttributes(data2, eval_info_1_1).empty());
    REQUIRE(expr.evaluateMissingAttributes(data2, eval_info_2_1).size() == 1);

    REQUIRE(expr.evaluate(data2, eval_info_1_1).isFalse());
    REQUIRE(expr.evaluate(data2, eval_info_2_1).isFalse());
    REQUIRE(expr.evaluate(data2, eval_info_1_2).isFalse());
    REQUIRE(expr.evaluate(data2, eval_info_2_2).isUndecided());

    DistributedData data3;
    REQUIRE(data3.insert("position", {99, timestamp_now, NodeData::ValueType::kValueDynamic}));

    REQUIRE(expr.evaluateMissingAttributes(data3, eval_info_1_1).size() == 1);
    REQUIRE(expr.evaluateMissingAttributes(data3, eval_info_2_1).size() == 1);
    REQUIRE(expr.evaluateMissingAttributes(data3, eval_info_1_1)[0] == "weight");
    REQUIRE(expr.evaluateMissingAttributes(data3, eval_info_2_1)[0] == "weight");

    REQUIRE(expr.evaluate(data3, eval_info_1_1).isFalse());
    REQUIRE(expr.evaluate(data3, eval_info_2_1).isFalse());
    REQUIRE(expr.evaluate(data3, eval_info_1_2).isUndecided());
    REQUIRE(expr.evaluate(data3, eval_info_2_2).isUndecided());

    LocalData local1;
    REQUIRE(local1.insert("weight", {100, timestamp_now, NodeData::ValueType::kValueDynamic}));

    REQUIRE(expr.evaluateMissingAttributes(local1, eval_info_1_1).empty());
    REQUIRE(expr.evaluateMissingAttributes(local1, eval_info_2_1).empty());

    REQUIRE(expr.evaluate(local1, eval_info_1_1).isTrue());
    REQUIRE(expr.evaluate(local1, eval_info_2_1).isTrue());
    REQUIRE(expr.evaluate(local1, eval_info_1_2).isTrue());
    REQUIRE(expr.evaluate(local1, eval_info_2_2).isTrue());

    LocalData local2;
    REQUIRE(local2.insert("weight", {99, timestamp_now, NodeData::ValueType::kValueDynamic}));

    REQUIRE(expr.evaluateMissingAttributes(local2, eval_info_1_1).empty());
    REQUIRE(expr.evaluateMissingAttributes(local2, eval_info_2_1).empty());

    REQUIRE(expr.evaluate(local2, eval_info_1_1).isFalse());
    REQUIRE(expr.evaluate(local2, eval_info_2_1).isFalse());
    REQUIRE(expr.evaluate(local2, eval_info_1_2).isFalse());
    REQUIRE(expr.evaluate(local2, eval_info_2_2).isFalse());

    LocalData local3;
    REQUIRE(local3.insert("position", {9999, timestamp_now, NodeData::ValueType::kValueDynamic}));

    REQUIRE(expr.evaluate(local3, eval_info_1_1).isFalse());
    REQUIRE(expr.evaluate(local3, eval_info_2_1).isFalse());
  }

  SECTION("comparisons with int") {
    NumericComparisonExpression<int> expr1("pos_x", ComparisonTypes::kGreaterThanOrEqualTo, 100);

    DistributedData data1;
    REQUIRE(data1.insert("pos_x", {100, timestamp_now, NodeData::ValueType::kValueDynamic}));
    REQUIRE(expr1.evaluate(data1, eval_info_1_1).isTrue());
    REQUIRE(expr1.evaluate(data1, eval_info_2_1).isFalse());
    REQUIRE(expr1.evaluate(data1, eval_info_1_2).isTrue());
    REQUIRE(expr1.evaluate(data1, eval_info_2_2).isUndecided());

    REQUIRE(data1.update("pos_x", {101, timestamp_now, NodeData::ValueType::kValueDynamic}));
    REQUIRE(expr1.evaluate(data1, eval_info_1_1).isTrue());
    REQUIRE(expr1.evaluate(data1, eval_info_2_1).isFalse());
    REQUIRE(expr1.evaluate(data1, eval_info_1_2).isTrue());
    REQUIRE(expr1.evaluate(data1, eval_info_2_2).isUndecided());

    REQUIRE(data1.update("pos_x", {10001, timestamp_now, NodeData::ValueType::kValueDynamic}));
    REQUIRE(expr1.evaluate(data1, eval_info_1_1).isTrue());
    REQUIRE(expr1.evaluate(data1, eval_info_2_1).isFalse());
    REQUIRE(expr1.evaluate(data1, eval_info_1_2).isTrue());
    REQUIRE(expr1.evaluate(data1, eval_info_2_2).isUndecided());

    REQUIRE(data1.update("pos_x", {99, timestamp_now, NodeData::ValueType::kValueDynamic}));
    REQUIRE(expr1.evaluate(data1, eval_info_1_1).isFalse());
    REQUIRE(expr1.evaluate(data1, eval_info_2_1).isFalse());
    REQUIRE(expr1.evaluate(data1, eval_info_1_2).isFalse());
    REQUIRE(expr1.evaluate(data1, eval_info_2_2).isUndecided());

    NumericComparisonExpression<int> expr2("pos_x", ComparisonTypes::kGreater, 100);

    DistributedData data2;
    REQUIRE(data2.insert("pos_x", {101, timestamp_now, NodeData::ValueType::kValueDynamic}));
    REQUIRE(expr2.evaluate(data2, eval_info_1_1).isTrue());
    REQUIRE(expr2.evaluate(data2, eval_info_2_1).isFalse());
    REQUIRE(expr2.evaluate(data2, eval_info_1_2).isTrue());
    REQUIRE(expr2.evaluate(data2, eval_info_2_2).isUndecided());

    REQUIRE(data2.update("pos_x", {10001, timestamp_now, NodeData::ValueType::kValueDynamic}));
    REQUIRE(expr2.evaluate(data2, eval_info_1_1).isTrue());
    REQUIRE(expr2.evaluate(data2, eval_info_2_1).isFalse());
    REQUIRE(expr2.evaluate(data2, eval_info_1_2).isTrue());
    REQUIRE(expr2.evaluate(data2, eval_info_2_2).isUndecided());

    REQUIRE(data2.update("pos_x", {99, timestamp_now, NodeData::ValueType::kValueDynamic}));
    REQUIRE(expr2.evaluate(data2, eval_info_1_1).isFalse());
    REQUIRE(expr2.evaluate(data2, eval_info_2_1).isFalse());
    REQUIRE(expr2.evaluate(data2, eval_info_1_2).isFalse());
    REQUIRE(expr2.evaluate(data2, eval_info_2_2).isUndecided());

    REQUIRE(data2.update("pos_x", {100, timestamp_now, NodeData::ValueType::kValueDynamic}));
    REQUIRE(expr2.evaluate(data2, eval_info_1_1).isFalse());
    REQUIRE(expr2.evaluate(data2, eval_info_2_1).isFalse());
    REQUIRE(expr2.evaluate(data2, eval_info_1_2).isFalse());
    REQUIRE(expr2.evaluate(data2, eval_info_2_2).isUndecided());

    NumericComparisonExpression<int> expr3("pos_x", ComparisonTypes::kLessThanOrEqualTo, 100);

    DistributedData data3;
    REQUIRE(data3.insert("pos_x", {100, timestamp_now, NodeData::ValueType::kValueDynamic}));
    REQUIRE(expr3.evaluate(data3, eval_info_1_1).isTrue());
    REQUIRE(expr3.evaluate(data3, eval_info_2_1).isFalse());
    REQUIRE(expr3.evaluate(data3, eval_info_1_2).isTrue());
    REQUIRE(expr3.evaluate(data3, eval_info_2_2).isUndecided());

    REQUIRE(data3.update("pos_x", {99, timestamp_now, NodeData::ValueType::kValueDynamic}));
    REQUIRE(expr3.evaluate(data3, eval_info_1_1).isTrue());
    REQUIRE(expr3.evaluate(data3, eval_info_2_1).isFalse());
    REQUIRE(expr3.evaluate(data3, eval_info_1_2).isTrue());
    REQUIRE(expr3.evaluate(data3, eval_info_2_2).isUndecided());

    REQUIRE(data3.update("pos_x", {-1, timestamp_now, NodeData::ValueType::kValueDynamic}));
    REQUIRE(expr3.evaluate(data3, eval_info_1_1).isTrue());
    REQUIRE(expr3.evaluate(data3, eval_info_2_1).isFalse());
    REQUIRE(expr3.evaluate(data3, eval_info_1_2).isTrue());
    REQUIRE(expr3.evaluate(data3, eval_info_2_2).isUndecided());

    REQUIRE(data3.update("pos_x", {101, timestamp_now, NodeData::ValueType::kValueDynamic}));
    REQUIRE(expr3.evaluate(data3, eval_info_1_1).isFalse());
    REQUIRE(expr3.evaluate(data3, eval_info_2_1).isFalse());
    REQUIRE(expr3.evaluate(data3, eval_info_1_2).isFalse());
    REQUIRE(expr3.evaluate(data3, eval_info_2_2).isUndecided());

    NumericComparisonExpression<int> expr4("pos_y", ComparisonTypes::kLessThan, 100);

    DistributedData data4;
    REQUIRE(data4.insert("pos_y", {100, timestamp_now, NodeData::ValueType::kValueDynamic}));
    REQUIRE(expr4.evaluate(data4, eval_info_1_1).isFalse());
    REQUIRE(expr4.evaluate(data4, eval_info_2_1).isFalse());
    REQUIRE(expr4.evaluate(data4, eval_info_1_2).isFalse());
    REQUIRE(expr4.evaluate(data4, eval_info_2_2).isUndecided());

    REQUIRE(data4.update("pos_y", {99, timestamp_now, NodeData::ValueType::kValueDynamic}));
    REQUIRE(expr4.evaluate(data4, eval_info_1_1).isTrue());
    REQUIRE(expr4.evaluate(data4, eval_info_2_1).isFalse());
    REQUIRE(expr4.evaluate(data4, eval_info_1_2).isTrue());
    REQUIRE(expr4.evaluate(data4, eval_info_2_2).isUndecided());

    REQUIRE(data4.update("pos_y", {-100, timestamp_now, NodeData::ValueType::kValueDynamic}));
    REQUIRE(expr4.evaluate(data4, eval_info_1_1).isTrue());
    REQUIRE(expr4.evaluate(data4, eval_info_2_1).isFalse());
    REQUIRE(expr4.evaluate(data4, eval_info_1_2).isTrue());
    REQUIRE(expr4.evaluate(data4, eval_info_2_2).isUndecided());

    REQUIRE(data4.update("pos_y", {1000, timestamp_now, NodeData::ValueType::kValueDynamic}));
    REQUIRE(expr4.evaluate(data4, eval_info_1_1).isFalse());
    REQUIRE(expr4.evaluate(data4, eval_info_2_1).isFalse());
    REQUIRE(expr4.evaluate(data4, eval_info_1_2).isFalse());
    REQUIRE(expr4.evaluate(data4, eval_info_2_2).isUndecided());

    NumericComparisonExpression<int> expr5("pos_y", ComparisonTypes::kNotEqualTo, 100);

    DistributedData data5;
    REQUIRE(data5.insert("pos_y", {100, timestamp_now, NodeData::ValueType::kValueDynamic}));
    REQUIRE(expr5.evaluate(data5, eval_info_1_1).isFalse());
    REQUIRE(expr5.evaluate(data5, eval_info_2_1).isFalse());
    REQUIRE(expr5.evaluate(data5, eval_info_1_2).isFalse());
    REQUIRE(expr5.evaluate(data5, eval_info_2_2).isUndecided());

    REQUIRE(data5.update("pos_y", {99, timestamp_now, NodeData::ValueType::kValueDynamic}));
    REQUIRE(expr5.evaluate(data5, eval_info_1_1).isTrue());
    REQUIRE(expr5.evaluate(data5, eval_info_2_1).isFalse());
    REQUIRE(expr5.evaluate(data5, eval_info_1_2).isTrue());
    REQUIRE(expr5.evaluate(data5, eval_info_2_2).isUndecided());
  }

  SECTION("comparisons with float") {
    NumericComparisonExpression<float> expr1("pos_x", ComparisonTypes::kGreaterThanOrEqualTo,
                                             42.14f);

    DistributedData data1;
    REQUIRE(data1.insert("pos_x", {50.55f, timestamp_now, NodeData::ValueType::kValueDynamic}));
    REQUIRE(expr1.evaluate(data1, eval_info_1_1).isTrue());
    REQUIRE(expr1.evaluate(data1, eval_info_2_1).isFalse());
    REQUIRE(expr1.evaluate(data1, eval_info_1_2).isTrue());
    REQUIRE(expr1.evaluate(data1, eval_info_2_2).isUndecided());

    // pos_x becomes int
    REQUIRE(data1.update("pos_x", {101, timestamp_now, NodeData::ValueType::kValueDynamic}));
    REQUIRE(expr1.evaluate(data1, eval_info_1_1).isTrue());
    REQUIRE(expr1.evaluate(data1, eval_info_2_1).isFalse());
    REQUIRE(expr1.evaluate(data1, eval_info_1_2).isTrue());
    REQUIRE(expr1.evaluate(data1, eval_info_2_2).isUndecided());
  }

  SECTION("invalid types") {
    NumericComparisonExpression<float> expr("pos_x", ComparisonTypes::kGreater, 100);

    DistributedData data;
    std::string my_value = "not a number";
    REQUIRE(data.insert("pos_x", {my_value, timestamp_now, NodeData::ValueType::kValueDynamic}));

    // maybe it shouldnt throw and just return false
    REQUIRE_THROWS(expr.evaluate(data, eval_info_1_1));
  }
}

TEST_CASE("BooleanExpression NumericComparisonExpression, Different Inquiries",
          "[BooleanExpression][NumericComparisonExpression]") {
  uint64_t timestamp_now = 1000;
  uint64_t threshold1 = 900;
  uint64_t threshold2 = 1100;

  EvaluationInformation eval_info_1_1_no_unknown{threshold1, true, false, true};
  EvaluationInformation eval_info_2_1_no_unknown{threshold2, true, false, true};
  EvaluationInformation eval_info_1_2_no_unknown{threshold1, false, false, true};
  EvaluationInformation eval_info_2_2_no_unknown{threshold2, false, false, true};

  EvaluationInformation eval_info_1_1_no_outdated{threshold1, true, true, false};
  EvaluationInformation eval_info_2_1_no_outdated{threshold2, true, true, false};
  EvaluationInformation eval_info_1_2_no_outdated{threshold1, false, true, false};
  EvaluationInformation eval_info_2_2_no_outdated{threshold2, false, true, false};

  EvaluationInformation eval_info_1_1_none{threshold1, true, false};
  EvaluationInformation eval_info_2_1_none{threshold2, true, false};
  EvaluationInformation eval_info_1_2_none{threshold1, false, false};
  EvaluationInformation eval_info_2_2_none{threshold2, false, false};

  SECTION("equal_to") {
    NumericComparisonExpression<int> expr("weight", ComparisonTypes::kEqualTo, 100);

    DistributedData data1;
    REQUIRE(data1.insert("weight", {100, timestamp_now, NodeData::ValueType::kValueDynamic}));

    REQUIRE(expr.evaluateMissingAttributes(data1, eval_info_1_1_no_unknown).empty());
    REQUIRE(expr.evaluateMissingAttributes(data1, eval_info_2_1_no_unknown).size() == 1);

    REQUIRE(expr.evaluate(data1, eval_info_1_1_no_unknown).isTrue());
    REQUIRE(expr.evaluate(data1, eval_info_2_1_no_unknown).isFalse());
    REQUIRE(expr.evaluate(data1, eval_info_1_2_no_unknown).isTrue());
    REQUIRE(expr.evaluate(data1, eval_info_2_2_no_unknown).isUndecided());

    REQUIRE(expr.evaluateMissingAttributes(data1, eval_info_1_1_no_outdated).empty());
    REQUIRE(expr.evaluateMissingAttributes(data1, eval_info_2_1_no_outdated).empty());

    REQUIRE(expr.evaluate(data1, eval_info_1_1_no_outdated).isTrue());
    REQUIRE(expr.evaluate(data1, eval_info_2_1_no_outdated).isTrue());
    REQUIRE(expr.evaluate(data1, eval_info_1_2_no_outdated).isTrue());
    REQUIRE(expr.evaluate(data1, eval_info_2_2_no_outdated).isTrue());

    REQUIRE(expr.evaluateMissingAttributes(data1, eval_info_1_1_none).empty());
    REQUIRE(expr.evaluateMissingAttributes(data1, eval_info_2_1_none).empty());

    REQUIRE(expr.evaluate(data1, eval_info_1_1_none).isTrue());
    REQUIRE(expr.evaluate(data1, eval_info_2_1_none).isTrue());
    REQUIRE(expr.evaluate(data1, eval_info_1_2_none).isTrue());
    REQUIRE(expr.evaluate(data1, eval_info_2_2_none).isTrue());

    DistributedData data2;
    REQUIRE(data2.insert("weight", {99, timestamp_now, NodeData::ValueType::kValueDynamic}));

    REQUIRE(expr.evaluateMissingAttributes(data2, eval_info_1_1_no_unknown).empty());
    REQUIRE(expr.evaluateMissingAttributes(data2, eval_info_2_1_no_unknown).size() == 1);

    REQUIRE(expr.evaluate(data2, eval_info_1_1_no_unknown).isFalse());
    REQUIRE(expr.evaluate(data2, eval_info_2_1_no_unknown).isFalse());
    REQUIRE(expr.evaluate(data2, eval_info_1_2_no_unknown).isFalse());
    REQUIRE(expr.evaluate(data2, eval_info_2_2_no_unknown).isUndecided());

    REQUIRE(expr.evaluateMissingAttributes(data2, eval_info_1_1_no_outdated).empty());
    REQUIRE(expr.evaluateMissingAttributes(data2, eval_info_2_1_no_outdated).empty());

    REQUIRE(expr.evaluate(data2, eval_info_1_1_no_outdated).isFalse());
    REQUIRE(expr.evaluate(data2, eval_info_2_1_no_outdated).isFalse());
    REQUIRE(expr.evaluate(data2, eval_info_1_2_no_outdated).isFalse());
    REQUIRE(expr.evaluate(data2, eval_info_2_2_no_outdated).isFalse());

    REQUIRE(expr.evaluateMissingAttributes(data2, eval_info_1_1_none).empty());
    REQUIRE(expr.evaluateMissingAttributes(data2, eval_info_2_1_none).empty());

    REQUIRE(expr.evaluate(data2, eval_info_1_1_none).isFalse());
    REQUIRE(expr.evaluate(data2, eval_info_2_1_none).isFalse());
    REQUIRE(expr.evaluate(data2, eval_info_1_2_none).isFalse());
    REQUIRE(expr.evaluate(data2, eval_info_2_2_none).isFalse());

    DistributedData data3;
    REQUIRE(data3.insert("position", {99, timestamp_now, NodeData::ValueType::kValueDynamic}));

    REQUIRE(expr.evaluateMissingAttributes(data3, eval_info_1_1_no_unknown).empty());
    REQUIRE(expr.evaluateMissingAttributes(data3, eval_info_2_1_no_unknown).empty());

    REQUIRE(expr.evaluate(data3, eval_info_1_1_no_unknown).isFalse());
    REQUIRE(expr.evaluate(data3, eval_info_2_1_no_unknown).isFalse());
    REQUIRE(expr.evaluate(data3, eval_info_1_2_no_unknown).isFalse());
    REQUIRE(expr.evaluate(data3, eval_info_2_2_no_unknown).isFalse());

    REQUIRE(expr.evaluateMissingAttributes(data3, eval_info_1_1_no_outdated).size() == 1);
    REQUIRE(expr.evaluateMissingAttributes(data3, eval_info_2_1_no_outdated).size() == 1);

    REQUIRE(expr.evaluate(data3, eval_info_1_1_no_outdated).isFalse());
    REQUIRE(expr.evaluate(data3, eval_info_2_1_no_outdated).isFalse());
    REQUIRE(expr.evaluate(data3, eval_info_1_2_no_outdated).isUndecided());
    REQUIRE(expr.evaluate(data3, eval_info_2_2_no_outdated).isUndecided());

    REQUIRE(expr.evaluateMissingAttributes(data3, eval_info_1_1_none).empty());
    REQUIRE(expr.evaluateMissingAttributes(data3, eval_info_2_1_none).empty());

    REQUIRE(expr.evaluate(data3, eval_info_1_1_none).isFalse());
    REQUIRE(expr.evaluate(data3, eval_info_2_1_none).isFalse());
    REQUIRE(expr.evaluate(data3, eval_info_1_2_none).isFalse());
    REQUIRE(expr.evaluate(data3, eval_info_2_2_none).isFalse());

    LocalData local1;
    REQUIRE(local1.insert("weight", {100, timestamp_now, NodeData::ValueType::kValueDynamic}));

    REQUIRE(expr.evaluateMissingAttributes(local1, eval_info_1_1_no_unknown).empty());
    REQUIRE(expr.evaluateMissingAttributes(local1, eval_info_2_1_no_unknown).empty());

    REQUIRE(expr.evaluate(local1, eval_info_1_1_no_unknown).isTrue());
    REQUIRE(expr.evaluate(local1, eval_info_2_1_no_unknown).isTrue());
    REQUIRE(expr.evaluate(local1, eval_info_1_2_no_unknown).isTrue());
    REQUIRE(expr.evaluate(local1, eval_info_2_2_no_unknown).isTrue());

    REQUIRE(expr.evaluateMissingAttributes(local1, eval_info_1_1_no_outdated).empty());
    REQUIRE(expr.evaluateMissingAttributes(local1, eval_info_2_1_no_outdated).empty());

    REQUIRE(expr.evaluate(local1, eval_info_1_1_no_outdated).isTrue());
    REQUIRE(expr.evaluate(local1, eval_info_2_1_no_outdated).isTrue());
    REQUIRE(expr.evaluate(local1, eval_info_1_2_no_outdated).isTrue());
    REQUIRE(expr.evaluate(local1, eval_info_2_2_no_outdated).isTrue());

    REQUIRE(expr.evaluateMissingAttributes(local1, eval_info_1_1_none).empty());
    REQUIRE(expr.evaluateMissingAttributes(local1, eval_info_2_1_none).empty());

    REQUIRE(expr.evaluate(local1, eval_info_1_1_none).isTrue());
    REQUIRE(expr.evaluate(local1, eval_info_2_1_none).isTrue());
    REQUIRE(expr.evaluate(local1, eval_info_1_2_none).isTrue());
    REQUIRE(expr.evaluate(local1, eval_info_2_2_none).isTrue());

    LocalData local2;
    REQUIRE(local2.insert("weight", {99, timestamp_now, NodeData::ValueType::kValueDynamic}));

    REQUIRE(expr.evaluateMissingAttributes(local2, eval_info_1_1_no_unknown).empty());
    REQUIRE(expr.evaluateMissingAttributes(local2, eval_info_2_1_no_unknown).empty());

    REQUIRE(expr.evaluate(local2, eval_info_1_1_no_unknown).isFalse());
    REQUIRE(expr.evaluate(local2, eval_info_2_1_no_unknown).isFalse());
    REQUIRE(expr.evaluate(local2, eval_info_1_2_no_unknown).isFalse());
    REQUIRE(expr.evaluate(local2, eval_info_2_2_no_unknown).isFalse());

    REQUIRE(expr.evaluateMissingAttributes(local2, eval_info_1_1_no_outdated).empty());
    REQUIRE(expr.evaluateMissingAttributes(local2, eval_info_2_1_no_outdated).empty());

    REQUIRE(expr.evaluate(local2, eval_info_1_1_no_outdated).isFalse());
    REQUIRE(expr.evaluate(local2, eval_info_2_1_no_outdated).isFalse());
    REQUIRE(expr.evaluate(local2, eval_info_1_2_no_outdated).isFalse());
    REQUIRE(expr.evaluate(local2, eval_info_2_2_no_outdated).isFalse());

    REQUIRE(expr.evaluateMissingAttributes(local2, eval_info_1_1_none).empty());
    REQUIRE(expr.evaluateMissingAttributes(local2, eval_info_2_1_none).empty());

    REQUIRE(expr.evaluate(local2, eval_info_1_1_none).isFalse());
    REQUIRE(expr.evaluate(local2, eval_info_2_1_none).isFalse());
    REQUIRE(expr.evaluate(local2, eval_info_1_2_none).isFalse());
    REQUIRE(expr.evaluate(local2, eval_info_2_2_none).isFalse());

    LocalData local3;
    REQUIRE(local3.insert("position", {9999, timestamp_now, NodeData::ValueType::kValueDynamic}));

    REQUIRE(expr.evaluate(local3, eval_info_1_1_no_unknown).isFalse());
    REQUIRE(expr.evaluate(local3, eval_info_2_1_no_unknown).isFalse());
    REQUIRE(expr.evaluate(local3, eval_info_1_2_no_unknown).isFalse());
    REQUIRE(expr.evaluate(local3, eval_info_2_2_no_unknown).isFalse());

    REQUIRE(expr.evaluate(local3, eval_info_1_1_no_outdated).isFalse());
    REQUIRE(expr.evaluate(local3, eval_info_2_1_no_outdated).isFalse());

    REQUIRE(expr.evaluate(local3, eval_info_1_2_no_outdated).isFalse());
    REQUIRE(expr.evaluate(local3, eval_info_2_2_no_outdated).isFalse());

    REQUIRE(expr.evaluate(local3, eval_info_1_1_none).isFalse());
    REQUIRE(expr.evaluate(local3, eval_info_2_1_none).isFalse());
    REQUIRE(expr.evaluate(local3, eval_info_1_2_none).isFalse());
    REQUIRE(expr.evaluate(local3, eval_info_2_2_none).isFalse());
  }

  SECTION("comparisons with int") {
    NumericComparisonExpression<int> expr1("pos_x", ComparisonTypes::kGreaterThanOrEqualTo, 100);

    DistributedData data1;
    REQUIRE(data1.insert("pos_x", {100, timestamp_now, NodeData::ValueType::kValueDynamic}));

    REQUIRE(expr1.evaluate(data1, eval_info_1_1_no_unknown).isTrue());
    REQUIRE(expr1.evaluate(data1, eval_info_2_1_no_unknown).isFalse());
    REQUIRE(expr1.evaluate(data1, eval_info_1_2_no_unknown).isTrue());
    REQUIRE(expr1.evaluate(data1, eval_info_2_2_no_unknown).isUndecided());

    REQUIRE(expr1.evaluate(data1, eval_info_1_1_no_outdated).isTrue());
    REQUIRE(expr1.evaluate(data1, eval_info_2_1_no_outdated).isTrue());
    REQUIRE(expr1.evaluate(data1, eval_info_1_2_no_outdated).isTrue());
    REQUIRE(expr1.evaluate(data1, eval_info_2_2_no_outdated).isTrue());

    REQUIRE(expr1.evaluate(data1, eval_info_1_1_none).isTrue());
    REQUIRE(expr1.evaluate(data1, eval_info_2_1_none).isTrue());
    REQUIRE(expr1.evaluate(data1, eval_info_1_2_none).isTrue());
    REQUIRE(expr1.evaluate(data1, eval_info_2_2_none).isTrue());

    REQUIRE(data1.update("pos_x", {101, timestamp_now, NodeData::ValueType::kValueDynamic}));

    REQUIRE(expr1.evaluate(data1, eval_info_1_1_no_unknown).isTrue());
    REQUIRE(expr1.evaluate(data1, eval_info_2_1_no_unknown).isFalse());
    REQUIRE(expr1.evaluate(data1, eval_info_1_2_no_unknown).isTrue());
    REQUIRE(expr1.evaluate(data1, eval_info_2_2_no_unknown).isUndecided());

    REQUIRE(expr1.evaluate(data1, eval_info_1_1_no_outdated).isTrue());
    REQUIRE(expr1.evaluate(data1, eval_info_2_1_no_outdated).isTrue());
    REQUIRE(expr1.evaluate(data1, eval_info_1_2_no_outdated).isTrue());
    REQUIRE(expr1.evaluate(data1, eval_info_2_2_no_outdated).isTrue());

    REQUIRE(expr1.evaluate(data1, eval_info_1_1_none).isTrue());
    REQUIRE(expr1.evaluate(data1, eval_info_2_1_none).isTrue());
    REQUIRE(expr1.evaluate(data1, eval_info_1_2_none).isTrue());
    REQUIRE(expr1.evaluate(data1, eval_info_2_2_none).isTrue());

    REQUIRE(data1.update("pos_x", {10001, timestamp_now, NodeData::ValueType::kValueDynamic}));

    REQUIRE(expr1.evaluate(data1, eval_info_1_1_no_unknown).isTrue());
    REQUIRE(expr1.evaluate(data1, eval_info_2_1_no_unknown).isFalse());
    REQUIRE(expr1.evaluate(data1, eval_info_1_2_no_unknown).isTrue());
    REQUIRE(expr1.evaluate(data1, eval_info_2_2_no_unknown).isUndecided());

    REQUIRE(expr1.evaluate(data1, eval_info_1_1_no_outdated).isTrue());
    REQUIRE(expr1.evaluate(data1, eval_info_2_1_no_outdated).isTrue());
    REQUIRE(expr1.evaluate(data1, eval_info_1_2_no_outdated).isTrue());
    REQUIRE(expr1.evaluate(data1, eval_info_2_2_no_outdated).isTrue());

    REQUIRE(expr1.evaluate(data1, eval_info_1_1_none).isTrue());
    REQUIRE(expr1.evaluate(data1, eval_info_2_1_none).isTrue());
    REQUIRE(expr1.evaluate(data1, eval_info_1_2_none).isTrue());
    REQUIRE(expr1.evaluate(data1, eval_info_2_2_none).isTrue());

    REQUIRE(data1.update("pos_x", {99, timestamp_now, NodeData::ValueType::kValueDynamic}));

    REQUIRE(expr1.evaluate(data1, eval_info_1_1_no_unknown).isFalse());
    REQUIRE(expr1.evaluate(data1, eval_info_2_1_no_unknown).isFalse());
    REQUIRE(expr1.evaluate(data1, eval_info_1_2_no_unknown).isFalse());
    REQUIRE(expr1.evaluate(data1, eval_info_2_2_no_unknown).isUndecided());

    REQUIRE(expr1.evaluate(data1, eval_info_1_1_no_outdated).isFalse());
    REQUIRE(expr1.evaluate(data1, eval_info_2_1_no_outdated).isFalse());
    REQUIRE(expr1.evaluate(data1, eval_info_1_2_no_outdated).isFalse());
    REQUIRE(expr1.evaluate(data1, eval_info_2_2_no_outdated).isFalse());

    REQUIRE(expr1.evaluate(data1, eval_info_1_1_none).isFalse());
    REQUIRE(expr1.evaluate(data1, eval_info_2_1_none).isFalse());
    REQUIRE(expr1.evaluate(data1, eval_info_1_2_none).isFalse());
    REQUIRE(expr1.evaluate(data1, eval_info_2_2_none).isFalse());
  }
}

TEST_CASE("BooleanExpression NotExpression, Inquire all", "[BooleanExpression][NotExpression]") {
  uint64_t timestamp_now = 1000;
  uint64_t threshold1 = 900;
  uint64_t threshold2 = 1100;

  EvaluationInformation eval_info_1_1{threshold1, true, true, true};

  EvaluationInformation eval_info_2_1{threshold2, true, true, true};

  EvaluationInformation eval_info_1_2{threshold1,
                                      false,  // not all information present
                                      true, true};

  EvaluationInformation eval_info_2_2{threshold2,
                                      false,  // not all information present
                                      true, true};

  SECTION("Negation of Presence Expressions") {
    auto subexpr1 = std::make_shared<PresenceExpression>("topicA");
    NotExpression expr1(subexpr1);
    REQUIRE(expr1.getRelevantKeys()[0] == "topicA");
    REQUIRE(expr1.getRelevantTopicKeys()[0] == "topicA");

    DistributedData data1;
    REQUIRE(data1.insert("topicA", {true, timestamp_now, NodeData::ValueType::kValueDynamic}));
    REQUIRE(expr1.evaluateMissingAttributes(data1, eval_info_1_1).size() == 0);
    REQUIRE(expr1.evaluateMissingAttributes(data1, eval_info_2_1)[0] == "topicA");
    REQUIRE((!expr1.evaluate(data1, eval_info_1_1)).isTrue());
    REQUIRE((!expr1.evaluate(data1, eval_info_2_1)).isFalse());
    REQUIRE((!expr1.evaluate(data1, eval_info_1_2)).isTrue());
    REQUIRE((!expr1.evaluate(data1, eval_info_2_2)).isUndecided());

    DistributedData data2;
    REQUIRE(data2.insert("topicB", {true, timestamp_now, NodeData::ValueType::kValueDynamic}));
    REQUIRE(expr1.evaluateMissingAttributes(data2, eval_info_1_1).size() == 1);
    REQUIRE(expr1.evaluateMissingAttributes(data2, eval_info_2_1).size() == 1);
    REQUIRE((!expr1.evaluate(data2, eval_info_1_1)).isFalse());
    REQUIRE((!expr1.evaluate(data2, eval_info_2_1)).isFalse());
    REQUIRE((!expr1.evaluate(data2, eval_info_1_2)).isUndecided());
    REQUIRE((!expr1.evaluate(data2, eval_info_2_2)).isUndecided());
  }

  SECTION("Negation of String Equality Expressions") {
    auto subexpr = std::make_shared<StringEqualityExpression>("wetter", "schlecht");
    NotExpression expr(subexpr);
    REQUIRE(expr.getRelevantKeys()[0] == "wetter");
    REQUIRE(expr.getRelevantTopicKeys().empty());

    DistributedData data1;
    std::string value1 = "schlecht";
    REQUIRE(data1.insert("wetter", {value1, timestamp_now, NodeData::ValueType::kValueDynamic}));

    REQUIRE(expr.evaluateMissingAttributes(data1, eval_info_1_1).empty());
    REQUIRE(expr.evaluateMissingAttributes(data1, eval_info_2_1)[0] == "wetter");
    REQUIRE((!expr.evaluate(data1, eval_info_1_1)).isTrue());
    REQUIRE((!expr.evaluate(data1, eval_info_2_1)).isFalse());
    REQUIRE((!expr.evaluate(data1, eval_info_1_2)).isTrue());
    REQUIRE((!expr.evaluate(data1, eval_info_2_2)).isUndecided());

    LocalData local2;
    std::string value4 = "gut";
    REQUIRE(local2.insert("wetter", {value4, timestamp_now, NodeData::ValueType::kValueDynamic}));

    REQUIRE(expr.evaluateMissingAttributes(local2, eval_info_1_1).empty());
    REQUIRE(expr.evaluateMissingAttributes(local2, eval_info_2_1).empty());
    REQUIRE((!expr.evaluate(local2, eval_info_1_1)).isFalse());
    REQUIRE((!expr.evaluate(local2, eval_info_2_1)).isFalse());
    REQUIRE((!expr.evaluate(local2, eval_info_1_2)).isFalse());
    REQUIRE((!expr.evaluate(local2, eval_info_2_2)).isFalse());
  }

  SECTION("Negation of Numeric Comparison Expressions") {
    auto subexpr = std::make_shared<NumericComparisonExpression<int>>(
        "weight", ComparisonTypes::kEqualTo, 100);
    NotExpression expr(subexpr);

    REQUIRE(expr.getRelevantKeys().size() == 1);
    REQUIRE(expr.getRelevantKeys()[0] == "weight");
    REQUIRE(expr.getRelevantTopicKeys().empty());

    DistributedData data1;
    REQUIRE(data1.insert("weight", {100, timestamp_now, NodeData::ValueType::kValueDynamic}));
    REQUIRE(expr.evaluateMissingAttributes(data1, eval_info_1_1).empty());
    REQUIRE(expr.evaluateMissingAttributes(data1, eval_info_2_1).size() == 1);
    REQUIRE(expr.evaluateMissingAttributes(data1, eval_info_2_1)[0] == "weight");

    REQUIRE((!expr.evaluate(data1, eval_info_1_1)).isTrue());
    REQUIRE((!expr.evaluate(data1, eval_info_2_1)).isFalse());
    REQUIRE((!expr.evaluate(data1, eval_info_1_2)).isTrue());
    REQUIRE((!expr.evaluate(data1, eval_info_2_2)).isUndecided());

    DistributedData data3;
    REQUIRE(data3.insert("position", {99, timestamp_now, NodeData::ValueType::kValueDynamic}));

    REQUIRE(expr.evaluateMissingAttributes(data3, eval_info_1_1).size() == 1);
    REQUIRE(expr.evaluateMissingAttributes(data3, eval_info_2_1).size() == 1);
    REQUIRE(expr.evaluateMissingAttributes(data3, eval_info_1_1)[0] == "weight");
    REQUIRE(expr.evaluateMissingAttributes(data3, eval_info_2_1)[0] == "weight");

    REQUIRE((!expr.evaluate(data3, eval_info_1_1)).isFalse());
    REQUIRE((!expr.evaluate(data3, eval_info_2_1)).isFalse());
    REQUIRE((!expr.evaluate(data3, eval_info_1_2)).isUndecided());
    REQUIRE((!expr.evaluate(data3, eval_info_2_2)).isUndecided());
  }
}

TEST_CASE("BooleanExpression AndExpression, Inquire all", "[BooleanExpression][AndExpression]") {
  uint64_t threshold1 = 900;
  uint64_t threshold2 = 1000;
  uint64_t threshold3 = 1100;

  EvaluationInformation eval_info_1_1{threshold1, true, true, true};

  EvaluationInformation eval_info_2_1{threshold2, true, true, true};

  EvaluationInformation eval_info_3_1{threshold3, true, true, true};

  EvaluationInformation eval_info_1_2{threshold1,
                                      false,  // not all information present
                                      true, true};

  EvaluationInformation eval_info_2_2{threshold2,
                                      false,  // not all information present
                                      true, true};

  EvaluationInformation eval_info_3_2{threshold3,
                                      false,  // not all information present
                                      true, true};

  auto subexpr1 = std::make_shared<StringEqualityExpression>("weather", "rain");
  auto subexpr2 = std::make_shared<PresenceExpression>("coffee");
  auto subexpr3 = std::make_shared<NumericComparisonExpression<float>>(
      "enthusiasm", ComparisonTypes::kLessThanOrEqualTo, 3.14159f);
  auto subexpr4 = std::make_shared<PresenceExpression>("bugs");

  AndExpression expr_2_4(subexpr2, subexpr4);
  REQUIRE(expr_2_4.getRelevantKeys().size() == 2);
  REQUIRE(
      ((expr_2_4.getRelevantKeys()[0] == "coffee") || (expr_2_4.getRelevantKeys()[1] == "coffee")));
  REQUIRE(((expr_2_4.getRelevantKeys()[0] == "bugs") || (expr_2_4.getRelevantKeys()[1] == "bugs")));
  REQUIRE(expr_2_4.getRelevantTopicKeys().size() == 2);

  DistributedData data1;
  REQUIRE(data1.insert("coffee", {false, 999, NodeData::ValueType::kValueDynamic}));
  REQUIRE(data1.insert("bugs", {true, 1001, NodeData::ValueType::kValueDynamic}));

  REQUIRE(expr_2_4.evaluateMissingAttributes(data1, eval_info_1_1).empty());
  REQUIRE(expr_2_4.evaluateMissingAttributes(data1, eval_info_2_1).size() == 1);
  REQUIRE(expr_2_4.evaluateMissingAttributes(data1, eval_info_3_1).size() == 2);

  REQUIRE((subexpr2->evaluate(data1, eval_info_1_1) && subexpr4->evaluate(data1, eval_info_1_1)) ==
          expr_2_4.evaluate(data1, eval_info_1_1));
  REQUIRE((subexpr2->evaluate(data1, eval_info_2_1) && subexpr4->evaluate(data1, eval_info_2_1)) ==
          expr_2_4.evaluate(data1, eval_info_2_1));
  REQUIRE((subexpr2->evaluate(data1, eval_info_3_1) && subexpr4->evaluate(data1, eval_info_3_1)) ==
          expr_2_4.evaluate(data1, eval_info_3_1));
  REQUIRE((subexpr2->evaluate(data1, eval_info_1_2) && subexpr4->evaluate(data1, eval_info_1_2)) ==
          expr_2_4.evaluate(data1, eval_info_1_2));
  REQUIRE((subexpr2->evaluate(data1, eval_info_2_2) && subexpr4->evaluate(data1, eval_info_2_2)) ==
          expr_2_4.evaluate(data1, eval_info_2_2));
  REQUIRE((subexpr2->evaluate(data1, eval_info_3_2) && subexpr4->evaluate(data1, eval_info_3_2)) ==
          expr_2_4.evaluate(data1, eval_info_3_2));

  REQUIRE(data1.update("coffee", {true, 999, NodeData::ValueType::kValueDynamic}));
  REQUIRE(data1.update("bugs", {true, 1001, NodeData::ValueType::kValueDynamic}));

  REQUIRE((subexpr2->evaluate(data1, eval_info_1_1) && subexpr4->evaluate(data1, eval_info_1_1)) ==
          expr_2_4.evaluate(data1, eval_info_1_1));
  REQUIRE((subexpr2->evaluate(data1, eval_info_2_1) && subexpr4->evaluate(data1, eval_info_2_1)) ==
          expr_2_4.evaluate(data1, eval_info_2_1));
  REQUIRE((subexpr2->evaluate(data1, eval_info_3_1) && subexpr4->evaluate(data1, eval_info_3_1)) ==
          expr_2_4.evaluate(data1, eval_info_3_1));
  REQUIRE((subexpr2->evaluate(data1, eval_info_1_2) && subexpr4->evaluate(data1, eval_info_1_2)) ==
          expr_2_4.evaluate(data1, eval_info_1_2));
  REQUIRE((subexpr2->evaluate(data1, eval_info_2_2) && subexpr4->evaluate(data1, eval_info_2_2)) ==
          expr_2_4.evaluate(data1, eval_info_2_2));
  REQUIRE((subexpr2->evaluate(data1, eval_info_3_2) && subexpr4->evaluate(data1, eval_info_3_2)) ==
          expr_2_4.evaluate(data1, eval_info_3_2));

  AndExpression expr_1_3(subexpr1, subexpr3);

  REQUIRE(expr_1_3.getRelevantKeys().size() == 2);
  REQUIRE(((expr_1_3.getRelevantKeys()[0] == "weather") ||
           (expr_1_3.getRelevantKeys()[1] == "weather")));
  REQUIRE(((expr_1_3.getRelevantKeys()[0] == "enthusiasm") ||
           (expr_1_3.getRelevantKeys()[1] == "enthusiasm")));
  REQUIRE(expr_1_3.getRelevantTopicKeys().empty());

  DistributedData data2;
  REQUIRE(data2.insert("coffee", {false, 999, NodeData::ValueType::kValueDynamic}));
  REQUIRE(data2.insert("weather", {"really really bad", 1001, NodeData::ValueType::kValueDynamic}));
  REQUIRE(data2.insert("enthusiasm", {1000, 999, NodeData::ValueType::kValueDynamic}));

  REQUIRE(expr_1_3.evaluateMissingAttributes(data2, eval_info_1_1).empty());
  REQUIRE(expr_1_3.evaluateMissingAttributes(data2, eval_info_2_1).size() == 1);
  REQUIRE(expr_1_3.evaluateMissingAttributes(data2, eval_info_3_1).size() == 2);

  REQUIRE((subexpr1->evaluate(data2, eval_info_1_1) && subexpr3->evaluate(data2, eval_info_1_1)) ==
          expr_1_3.evaluate(data2, eval_info_1_1));
  REQUIRE((subexpr1->evaluate(data2, eval_info_2_1) && subexpr3->evaluate(data2, eval_info_2_1)) ==
          expr_1_3.evaluate(data2, eval_info_2_1));
  REQUIRE((subexpr1->evaluate(data2, eval_info_3_1) && subexpr3->evaluate(data2, eval_info_3_1)) ==
          expr_1_3.evaluate(data2, eval_info_3_1));
  REQUIRE((subexpr1->evaluate(data2, eval_info_1_2) && subexpr3->evaluate(data2, eval_info_1_2)) ==
          expr_1_3.evaluate(data2, eval_info_1_2));
  REQUIRE((subexpr1->evaluate(data2, eval_info_2_2) && subexpr3->evaluate(data2, eval_info_2_2)) ==
          expr_1_3.evaluate(data2, eval_info_2_2));
  REQUIRE((subexpr1->evaluate(data2, eval_info_3_2) && subexpr3->evaluate(data2, eval_info_3_2)) ==
          expr_1_3.evaluate(data2, eval_info_3_2));

  REQUIRE(data2.update("enthusiasm", {0, 1111, NodeData::ValueType::kValueDynamic}));
  REQUIRE((subexpr1->evaluate(data2, eval_info_1_1) && subexpr3->evaluate(data2, eval_info_1_1)) ==
          expr_1_3.evaluate(data2, eval_info_1_1));
  REQUIRE((subexpr1->evaluate(data2, eval_info_2_1) && subexpr3->evaluate(data2, eval_info_2_1)) ==
          expr_1_3.evaluate(data2, eval_info_2_1));
  REQUIRE((subexpr1->evaluate(data2, eval_info_3_1) && subexpr3->evaluate(data2, eval_info_3_1)) ==
          expr_1_3.evaluate(data2, eval_info_3_1));
  REQUIRE((subexpr1->evaluate(data2, eval_info_1_2) && subexpr3->evaluate(data2, eval_info_1_2)) ==
          expr_1_3.evaluate(data2, eval_info_1_2));
  REQUIRE((subexpr1->evaluate(data2, eval_info_2_2) && subexpr3->evaluate(data2, eval_info_2_2)) ==
          expr_1_3.evaluate(data2, eval_info_2_2));
  REQUIRE((subexpr1->evaluate(data2, eval_info_3_2) && subexpr3->evaluate(data2, eval_info_3_2)) ==
          expr_1_3.evaluate(data2, eval_info_3_2));

  REQUIRE(data2.update("weather", {"rain", 1101, NodeData::ValueType::kValueDynamic}));
  REQUIRE((subexpr1->evaluate(data2, eval_info_1_1) && subexpr3->evaluate(data2, eval_info_1_1)) ==
          expr_1_3.evaluate(data2, eval_info_1_1));
  REQUIRE((subexpr1->evaluate(data2, eval_info_2_1) && subexpr3->evaluate(data2, eval_info_2_1)) ==
          expr_1_3.evaluate(data2, eval_info_2_1));
  REQUIRE((subexpr1->evaluate(data2, eval_info_3_1) && subexpr3->evaluate(data2, eval_info_3_1)) ==
          expr_1_3.evaluate(data2, eval_info_3_1));
  REQUIRE((subexpr1->evaluate(data2, eval_info_1_2) && subexpr3->evaluate(data2, eval_info_1_2)) ==
          expr_1_3.evaluate(data2, eval_info_1_2));
  REQUIRE((subexpr1->evaluate(data2, eval_info_2_2) && subexpr3->evaluate(data2, eval_info_2_2)) ==
          expr_1_3.evaluate(data2, eval_info_2_2));
  REQUIRE((subexpr1->evaluate(data2, eval_info_3_2) && subexpr3->evaluate(data2, eval_info_3_2)) ==
          expr_1_3.evaluate(data2, eval_info_3_2));
}

TEST_CASE("BooleanExpression OrExpression, Inquire all", "[BooleanExpression][OrExpression]") {
  uint64_t threshold1 = 900;
  uint64_t threshold2 = 1000;
  uint64_t threshold3 = 1100;

  //   uint64_t timestamp_now = 1000;
  //   uint64_t threshold1 = 900;
  //   uint64_t threshold2 = 1100;

  EvaluationInformation eval_info_1_1{threshold1, true, true, true};

  EvaluationInformation eval_info_2_1{threshold2, true, true, true};

  EvaluationInformation eval_info_3_1{threshold3, true, true, true};

  EvaluationInformation eval_info_1_2{threshold1,
                                      false,  // not all information present
                                      true, true};

  EvaluationInformation eval_info_2_2{threshold2,
                                      false,  // not all information present
                                      true, true};

  EvaluationInformation eval_info_3_2{threshold3,
                                      false,  // not all information present
                                      true, true};

  auto subexpr1 = std::make_shared<StringEqualityExpression>("weather", "rain");
  auto subexpr2 = std::make_shared<PresenceExpression>("coffee");
  auto subexpr3 = std::make_shared<NumericComparisonExpression<float>>(
      "enthusiasm", ComparisonTypes::kLessThanOrEqualTo, 3.14159f);
  auto subexpr4 = std::make_shared<PresenceExpression>("bugs");

  OrExpression expr_2_4(subexpr2, subexpr4);
  REQUIRE(expr_2_4.getRelevantKeys().size() == 2);
  REQUIRE(
      ((expr_2_4.getRelevantKeys()[0] == "coffee") || (expr_2_4.getRelevantKeys()[1] == "coffee")));
  REQUIRE(((expr_2_4.getRelevantKeys()[0] == "bugs") || (expr_2_4.getRelevantKeys()[1] == "bugs")));
  REQUIRE(expr_2_4.getRelevantTopicKeys().size() == 2);

  DistributedData data1;
  REQUIRE(data1.insert("coffee", {false, 999, NodeData::ValueType::kValueDynamic}));
  REQUIRE(data1.insert("bugs", {true, 1001, NodeData::ValueType::kValueDynamic}));

  REQUIRE(expr_2_4.evaluateMissingAttributes(data1, eval_info_1_1).empty());
  REQUIRE(expr_2_4.evaluateMissingAttributes(data1, eval_info_2_1).empty());
  REQUIRE(expr_2_4.evaluateMissingAttributes(data1, eval_info_3_1).size() == 2);

  REQUIRE((subexpr2->evaluate(data1, eval_info_1_1) || subexpr4->evaluate(data1, eval_info_1_1)) ==
          expr_2_4.evaluate(data1, eval_info_1_1));
  REQUIRE((subexpr2->evaluate(data1, eval_info_2_1) || subexpr4->evaluate(data1, eval_info_2_1)) ==
          expr_2_4.evaluate(data1, eval_info_2_1));
  REQUIRE((subexpr2->evaluate(data1, eval_info_3_1) || subexpr4->evaluate(data1, eval_info_3_1)) ==
          expr_2_4.evaluate(data1, eval_info_3_1));
  REQUIRE((subexpr2->evaluate(data1, eval_info_1_2) || subexpr4->evaluate(data1, eval_info_1_2)) ==
          expr_2_4.evaluate(data1, eval_info_1_2));
  REQUIRE((subexpr2->evaluate(data1, eval_info_2_2) || subexpr4->evaluate(data1, eval_info_2_2)) ==
          expr_2_4.evaluate(data1, eval_info_2_2));
  REQUIRE((subexpr2->evaluate(data1, eval_info_3_2) || subexpr4->evaluate(data1, eval_info_3_2)) ==
          expr_2_4.evaluate(data1, eval_info_3_2));

  REQUIRE(data1.update("coffee", {true, 999, NodeData::ValueType::kValueDynamic}));
  REQUIRE(data1.update("bugs", {true, 1001, NodeData::ValueType::kValueDynamic}));

  REQUIRE((subexpr2->evaluate(data1, eval_info_1_1) || subexpr4->evaluate(data1, eval_info_1_1)) ==
          expr_2_4.evaluate(data1, eval_info_1_1));
  REQUIRE((subexpr2->evaluate(data1, eval_info_2_1) || subexpr4->evaluate(data1, eval_info_2_1)) ==
          expr_2_4.evaluate(data1, eval_info_2_1));
  REQUIRE((subexpr2->evaluate(data1, eval_info_3_1) || subexpr4->evaluate(data1, eval_info_3_1)) ==
          expr_2_4.evaluate(data1, eval_info_3_1));
  REQUIRE((subexpr2->evaluate(data1, eval_info_1_2) || subexpr4->evaluate(data1, eval_info_1_2)) ==
          expr_2_4.evaluate(data1, eval_info_1_2));
  REQUIRE((subexpr2->evaluate(data1, eval_info_2_2) || subexpr4->evaluate(data1, eval_info_2_2)) ==
          expr_2_4.evaluate(data1, eval_info_2_2));
  REQUIRE((subexpr2->evaluate(data1, eval_info_3_2) || subexpr4->evaluate(data1, eval_info_3_2)) ==
          expr_2_4.evaluate(data1, eval_info_3_2));

  OrExpression expr_1_3(subexpr1, subexpr3);

  REQUIRE(expr_1_3.getRelevantKeys().size() == 2);
  REQUIRE(((expr_1_3.getRelevantKeys()[0] == "weather") ||
           (expr_1_3.getRelevantKeys()[1] == "weather")));
  REQUIRE(((expr_1_3.getRelevantKeys()[0] == "enthusiasm") ||
           (expr_1_3.getRelevantKeys()[1] == "enthusiasm")));
  REQUIRE(expr_1_3.getRelevantTopicKeys().empty());

  DistributedData data2;
  REQUIRE(data2.insert("coffee", {false, 999, NodeData::ValueType::kValueDynamic}));
  REQUIRE(data2.insert("weather", {"really really bad", 1001, NodeData::ValueType::kValueDynamic}));
  REQUIRE(data2.insert("enthusiasm", {1000, 999, NodeData::ValueType::kValueDynamic}));

  REQUIRE(expr_1_3.evaluateMissingAttributes(data2, eval_info_1_1).size() == 0);
  REQUIRE(expr_1_3.evaluateMissingAttributes(data2, eval_info_2_1).size() == 1);
  REQUIRE(expr_1_3.evaluateMissingAttributes(data2, eval_info_3_1).size() == 2);

  REQUIRE((subexpr1->evaluate(data2, eval_info_1_1) || subexpr3->evaluate(data2, eval_info_1_1)) ==
          expr_1_3.evaluate(data2, eval_info_1_1));
  REQUIRE((subexpr1->evaluate(data2, eval_info_2_1) || subexpr3->evaluate(data2, eval_info_2_1)) ==
          expr_1_3.evaluate(data2, eval_info_2_1));
  REQUIRE((subexpr1->evaluate(data2, eval_info_3_1) || subexpr3->evaluate(data2, eval_info_3_1)) ==
          expr_1_3.evaluate(data2, eval_info_3_1));
  REQUIRE((subexpr1->evaluate(data2, eval_info_1_2) || subexpr3->evaluate(data2, eval_info_1_2)) ==
          expr_1_3.evaluate(data2, eval_info_1_2));
  REQUIRE((subexpr1->evaluate(data2, eval_info_2_2) || subexpr3->evaluate(data2, eval_info_2_2)) ==
          expr_1_3.evaluate(data2, eval_info_2_2));
  REQUIRE((subexpr1->evaluate(data2, eval_info_3_2) || subexpr3->evaluate(data2, eval_info_3_2)) ==
          expr_1_3.evaluate(data2, eval_info_3_2));

  REQUIRE(data2.update("enthusiasm", {0, 1111, NodeData::ValueType::kValueDynamic}));
  REQUIRE((subexpr1->evaluate(data2, eval_info_1_1) || subexpr3->evaluate(data2, eval_info_1_1)) ==
          expr_1_3.evaluate(data2, eval_info_1_1));
  REQUIRE((subexpr1->evaluate(data2, eval_info_2_1) || subexpr3->evaluate(data2, eval_info_2_1)) ==
          expr_1_3.evaluate(data2, eval_info_2_1));
  REQUIRE((subexpr1->evaluate(data2, eval_info_3_1) || subexpr3->evaluate(data2, eval_info_3_1)) ==
          expr_1_3.evaluate(data2, eval_info_3_1));
  REQUIRE((subexpr1->evaluate(data2, eval_info_1_2) || subexpr3->evaluate(data2, eval_info_1_2)) ==
          expr_1_3.evaluate(data2, eval_info_1_2));
  REQUIRE((subexpr1->evaluate(data2, eval_info_2_2) || subexpr3->evaluate(data2, eval_info_2_2)) ==
          expr_1_3.evaluate(data2, eval_info_2_2));
  REQUIRE((subexpr1->evaluate(data2, eval_info_3_2) || subexpr3->evaluate(data2, eval_info_3_2)) ==
          expr_1_3.evaluate(data2, eval_info_3_2));

  REQUIRE(data2.update("weather", {"rain", 1101, NodeData::ValueType::kValueDynamic}));
  REQUIRE((subexpr1->evaluate(data2, eval_info_1_1) || subexpr3->evaluate(data2, eval_info_1_1)) ==
          expr_1_3.evaluate(data2, eval_info_1_1));
  REQUIRE((subexpr1->evaluate(data2, eval_info_2_1) || subexpr3->evaluate(data2, eval_info_2_1)) ==
          expr_1_3.evaluate(data2, eval_info_2_1));
  REQUIRE((subexpr1->evaluate(data2, eval_info_3_1) || subexpr3->evaluate(data2, eval_info_3_1)) ==
          expr_1_3.evaluate(data2, eval_info_3_1));
  REQUIRE((subexpr1->evaluate(data2, eval_info_1_2) || subexpr3->evaluate(data2, eval_info_1_2)) ==
          expr_1_3.evaluate(data2, eval_info_1_2));
  REQUIRE((subexpr1->evaluate(data2, eval_info_2_2) || subexpr3->evaluate(data2, eval_info_2_2)) ==
          expr_1_3.evaluate(data2, eval_info_2_2));
  REQUIRE((subexpr1->evaluate(data2, eval_info_3_2) || subexpr3->evaluate(data2, eval_info_3_2)) ==
          expr_1_3.evaluate(data2, eval_info_3_2));

  OrExpression expr_1_4(subexpr1, subexpr4);
  REQUIRE(expr_1_4.getRelevantKeys().size() == 2);
  REQUIRE(((expr_1_4.getRelevantKeys()[0] == "weather") ||
           (expr_1_4.getRelevantKeys()[1] == "weather")));
  REQUIRE(((expr_1_4.getRelevantKeys()[0] == "bugs") || (expr_1_4.getRelevantKeys()[1] == "bugs")));
  REQUIRE(expr_1_4.getRelevantTopicKeys().size() == 1);

  DistributedData data3;
  REQUIRE(data3.insert("weather", {"thunderstorm", 999, NodeData::ValueType::kValueDynamic}));

  REQUIRE(expr_1_4.evaluateMissingAttributes(data3, eval_info_1_1).size() == 1);
  REQUIRE(expr_1_4.evaluateMissingAttributes(data3, eval_info_2_1).size() == 2);
  REQUIRE(expr_1_4.evaluateMissingAttributes(data3, eval_info_3_1).size() == 2);

  REQUIRE(data3.insert("bugs", {false, 1001, NodeData::ValueType::kValueDynamic}));
  REQUIRE(expr_1_4.evaluateMissingAttributes(data3, eval_info_1_1).size() == 0);
  REQUIRE(expr_1_4.evaluateMissingAttributes(data3, eval_info_2_1).size() == 1);
  REQUIRE(expr_1_4.evaluateMissingAttributes(data3, eval_info_3_1).size() == 2);

  REQUIRE(data3.update("bugs", {true, 1001, NodeData::ValueType::kValueDynamic}));
  REQUIRE(expr_1_4.evaluateMissingAttributes(data3, eval_info_1_1).size() == 0);
  REQUIRE(expr_1_4.evaluateMissingAttributes(data3, eval_info_2_1).size() == 0);
  REQUIRE(expr_1_4.evaluateMissingAttributes(data3, eval_info_3_1).size() == 2);

  REQUIRE((subexpr1->evaluate(data3, eval_info_1_1) || subexpr4->evaluate(data3, eval_info_1_1)) ==
          expr_1_4.evaluate(data3, eval_info_1_1));
  REQUIRE((subexpr1->evaluate(data3, eval_info_2_1) || subexpr4->evaluate(data3, eval_info_2_1)) ==
          expr_1_4.evaluate(data3, eval_info_2_1));
  REQUIRE((subexpr1->evaluate(data3, eval_info_3_1) || subexpr4->evaluate(data3, eval_info_3_1)) ==
          expr_1_4.evaluate(data3, eval_info_3_1));
  REQUIRE((subexpr1->evaluate(data3, eval_info_1_2) || subexpr4->evaluate(data3, eval_info_1_2)) ==
          expr_1_4.evaluate(data3, eval_info_1_2));
  REQUIRE((subexpr1->evaluate(data3, eval_info_2_2) || subexpr4->evaluate(data3, eval_info_2_2)) ==
          expr_1_4.evaluate(data3, eval_info_2_2));
  REQUIRE((subexpr1->evaluate(data3, eval_info_3_2) || subexpr4->evaluate(data3, eval_info_3_2)) ==
          expr_1_4.evaluate(data3, eval_info_3_2));
}

TEST_CASE("BooleanExpression OrExpression, Different Inquiries",
          "[BooleanExpression][OrExpression]") {
  uint64_t threshold1 = 900;
  uint64_t threshold2 = 1000;
  uint64_t threshold3 = 1100;

  EvaluationInformation eval_info_1_1_no_unknown{threshold1, true, false, true};
  EvaluationInformation eval_info_2_1_no_unknown{threshold2, true, false, true};
  EvaluationInformation eval_info_3_1_no_unknown{threshold3, true, false, true};
  EvaluationInformation eval_info_1_2_no_unknown{threshold1, false, false, true};
  EvaluationInformation eval_info_2_2_no_unknown{threshold2, false, false, true};
  EvaluationInformation eval_info_3_2_no_unknown{threshold3, false, false, true};

  EvaluationInformation eval_info_1_1_no_outdated{threshold1, true, true, false};
  EvaluationInformation eval_info_2_1_no_outdated{threshold2, true, true, false};
  EvaluationInformation eval_info_3_1_no_outdated{threshold3, true, true, false};
  EvaluationInformation eval_info_1_2_no_outdated{threshold1, false, true, false};
  EvaluationInformation eval_info_2_2_no_outdated{threshold2, false, true, false};
  EvaluationInformation eval_info_3_2_no_outdated{threshold3, false, true, false};

  EvaluationInformation eval_info_1_1_none{threshold1, true, false, false};
  EvaluationInformation eval_info_2_1_none{threshold2, true, false, false};
  EvaluationInformation eval_info_3_1_none{threshold3, true, false, false};
  EvaluationInformation eval_info_1_2_none{threshold1, false, false, false};
  EvaluationInformation eval_info_2_2_none{threshold2, false, false, false};
  EvaluationInformation eval_info_3_2_none{threshold3, false, false, false};

  auto subexpr1 = std::make_shared<StringEqualityExpression>("weather", "rain");
  auto subexpr2 = std::make_shared<PresenceExpression>("coffee");
  auto subexpr3 = std::make_shared<NumericComparisonExpression<float>>(
      "enthusiasm", ComparisonTypes::kLessThanOrEqualTo, 3.14159f);
  auto subexpr4 = std::make_shared<PresenceExpression>("bugs");

  OrExpression expr_2_4(subexpr2, subexpr4);

  DistributedData data1;
  REQUIRE(data1.insert("coffee", {false, 999, NodeData::ValueType::kValueDynamic}));
  REQUIRE(data1.insert("bugs", {true, 1001, NodeData::ValueType::kValueDynamic}));

  REQUIRE(expr_2_4.evaluateMissingAttributes(data1, eval_info_1_1_no_unknown).empty());
  REQUIRE(expr_2_4.evaluateMissingAttributes(data1, eval_info_2_1_no_unknown).empty());
  REQUIRE(expr_2_4.evaluateMissingAttributes(data1, eval_info_3_1_no_unknown).size() == 2);
  REQUIRE(expr_2_4.evaluateMissingAttributes(data1, eval_info_1_2_no_unknown).empty());
  REQUIRE(expr_2_4.evaluateMissingAttributes(data1, eval_info_2_2_no_unknown).empty());
  REQUIRE(expr_2_4.evaluateMissingAttributes(data1, eval_info_3_2_no_unknown).size() == 2);

  REQUIRE(expr_2_4.evaluateMissingAttributes(data1, eval_info_1_1_no_outdated).empty());
  REQUIRE(expr_2_4.evaluateMissingAttributes(data1, eval_info_2_1_no_outdated).empty());
  REQUIRE(expr_2_4.evaluateMissingAttributes(data1, eval_info_3_1_no_outdated).empty());
  REQUIRE(expr_2_4.evaluateMissingAttributes(data1, eval_info_1_2_no_outdated).empty());
  REQUIRE(expr_2_4.evaluateMissingAttributes(data1, eval_info_2_2_no_outdated).empty());
  REQUIRE(expr_2_4.evaluateMissingAttributes(data1, eval_info_3_2_no_outdated).empty());

  REQUIRE(expr_2_4.evaluateMissingAttributes(data1, eval_info_1_1_none).empty());
  REQUIRE(expr_2_4.evaluateMissingAttributes(data1, eval_info_2_1_none).empty());
  REQUIRE(expr_2_4.evaluateMissingAttributes(data1, eval_info_3_1_none).empty());
  REQUIRE(expr_2_4.evaluateMissingAttributes(data1, eval_info_1_2_none).empty());
  REQUIRE(expr_2_4.evaluateMissingAttributes(data1, eval_info_2_2_none).empty());
  REQUIRE(expr_2_4.evaluateMissingAttributes(data1, eval_info_3_2_none).empty());

  //   REQUIRE((subexpr2->evaluate(data1, eval_info_1_1) || subexpr4->evaluate(data1,
  //   eval_info_1_1)) ==
  //           expr_2_4.evaluate(data1, eval_info_1_1));
  //   REQUIRE((subexpr2->evaluate(data1, eval_info_2_1) || subexpr4->evaluate(data1,
  //   eval_info_2_1)) ==
  //           expr_2_4.evaluate(data1, eval_info_2_1));
  //   REQUIRE((subexpr2->evaluate(data1, eval_info_3_1) || subexpr4->evaluate(data1,
  //   eval_info_3_1)) ==
  //           expr_2_4.evaluate(data1, eval_info_3_1));
  //   REQUIRE((subexpr2->evaluate(data1, eval_info_1_2) || subexpr4->evaluate(data1,
  //   eval_info_1_2)) ==
  //           expr_2_4.evaluate(data1, eval_info_1_2));
  //   REQUIRE((subexpr2->evaluate(data1, eval_info_2_2) || subexpr4->evaluate(data1,
  //   eval_info_2_2)) ==
  //           expr_2_4.evaluate(data1, eval_info_2_2));
  //   REQUIRE((subexpr2->evaluate(data1, eval_info_3_2) || subexpr4->evaluate(data1,
  //   eval_info_3_2)) ==
  //           expr_2_4.evaluate(data1, eval_info_3_2));

  //   REQUIRE(data1.update("coffee", {true, 999, NodeData::ValueType::kValueDynamic}));
  //   REQUIRE(data1.update("bugs", {true, 1001, NodeData::ValueType::kValueDynamic}));

  //   REQUIRE((subexpr2->evaluate(data1, eval_info_1_1) || subexpr4->evaluate(data1,
  //   eval_info_1_1)) ==
  //           expr_2_4.evaluate(data1, eval_info_1_1));
  //   REQUIRE((subexpr2->evaluate(data1, eval_info_2_1) || subexpr4->evaluate(data1,
  //   eval_info_2_1)) ==
  //           expr_2_4.evaluate(data1, eval_info_2_1));
  //   REQUIRE((subexpr2->evaluate(data1, eval_info_3_1) || subexpr4->evaluate(data1,
  //   eval_info_3_1)) ==
  //           expr_2_4.evaluate(data1, eval_info_3_1));
  //   REQUIRE((subexpr2->evaluate(data1, eval_info_1_2) || subexpr4->evaluate(data1,
  //   eval_info_1_2)) ==
  //           expr_2_4.evaluate(data1, eval_info_1_2));
  //   REQUIRE((subexpr2->evaluate(data1, eval_info_2_2) || subexpr4->evaluate(data1,
  //   eval_info_2_2)) ==
  //           expr_2_4.evaluate(data1, eval_info_2_2));
  //   REQUIRE((subexpr2->evaluate(data1, eval_info_3_2) || subexpr4->evaluate(data1,
  //   eval_info_3_2)) ==
  //           expr_2_4.evaluate(data1, eval_info_3_2));

  //   OrExpression expr_1_3(subexpr1, subexpr3);

  //   REQUIRE(expr_1_3.getRelevantKeys().size() == 2);
  //   REQUIRE(((expr_1_3.getRelevantKeys()[0] == "weather") ||
  //            (expr_1_3.getRelevantKeys()[1] == "weather")));
  //   REQUIRE(((expr_1_3.getRelevantKeys()[0] == "enthusiasm") ||
  //            (expr_1_3.getRelevantKeys()[1] == "enthusiasm")));
  //   REQUIRE(expr_1_3.getRelevantTopicKeys().empty());

  //   DistributedData data2;
  //   REQUIRE(data2.insert("coffee", {false, 999, NodeData::ValueType::kValueDynamic}));
  //   REQUIRE(
  //       data2.insert("weather", {"really really bad", 1001,
  //       NodeData::ValueType::kValueDynamic}));
  //   REQUIRE(data2.insert("enthusiasm", {1000, 999, NodeData::ValueType::kValueDynamic}));

  //   REQUIRE(expr_1_3.evaluateMissingAttributes(data2, eval_info_1_1).size() == 0);
  //   REQUIRE(expr_1_3.evaluateMissingAttributes(data2, eval_info_2_1).size() == 1);
  //   REQUIRE(expr_1_3.evaluateMissingAttributes(data2, eval_info_3_1).size() == 2);

  //   REQUIRE((subexpr1->evaluate(data2, eval_info_1_1) || subexpr3->evaluate(data2,
  //   eval_info_1_1)) ==
  //           expr_1_3.evaluate(data2, eval_info_1_1));
  //   REQUIRE((subexpr1->evaluate(data2, eval_info_2_1) || subexpr3->evaluate(data2,
  //   eval_info_2_1)) ==
  //           expr_1_3.evaluate(data2, eval_info_2_1));
  //   REQUIRE((subexpr1->evaluate(data2, eval_info_3_1) || subexpr3->evaluate(data2,
  //   eval_info_3_1)) ==
  //           expr_1_3.evaluate(data2, eval_info_3_1));
  //   REQUIRE((subexpr1->evaluate(data2, eval_info_1_2) || subexpr3->evaluate(data2,
  //   eval_info_1_2)) ==
  //           expr_1_3.evaluate(data2, eval_info_1_2));
  //   REQUIRE((subexpr1->evaluate(data2, eval_info_2_2) || subexpr3->evaluate(data2,
  //   eval_info_2_2)) ==
  //           expr_1_3.evaluate(data2, eval_info_2_2));
  //   REQUIRE((subexpr1->evaluate(data2, eval_info_3_2) || subexpr3->evaluate(data2,
  //   eval_info_3_2)) ==
  //           expr_1_3.evaluate(data2, eval_info_3_2));

  //   REQUIRE(data2.update("enthusiasm", {0, 1111, NodeData::ValueType::kValueDynamic}));
  //   REQUIRE((subexpr1->evaluate(data2, eval_info_1_1) || subexpr3->evaluate(data2,
  //   eval_info_1_1)) ==
  //           expr_1_3.evaluate(data2, eval_info_1_1));
  //   REQUIRE((subexpr1->evaluate(data2, eval_info_2_1) || subexpr3->evaluate(data2,
  //   eval_info_2_1)) ==
  //           expr_1_3.evaluate(data2, eval_info_2_1));
  //   REQUIRE((subexpr1->evaluate(data2, eval_info_3_1) || subexpr3->evaluate(data2,
  //   eval_info_3_1)) ==
  //           expr_1_3.evaluate(data2, eval_info_3_1));
  //   REQUIRE((subexpr1->evaluate(data2, eval_info_1_2) || subexpr3->evaluate(data2,
  //   eval_info_1_2)) ==
  //           expr_1_3.evaluate(data2, eval_info_1_2));
  //   REQUIRE((subexpr1->evaluate(data2, eval_info_2_2) || subexpr3->evaluate(data2,
  //   eval_info_2_2)) ==
  //           expr_1_3.evaluate(data2, eval_info_2_2));
  //   REQUIRE((subexpr1->evaluate(data2, eval_info_3_2) || subexpr3->evaluate(data2,
  //   eval_info_3_2)) ==
  //           expr_1_3.evaluate(data2, eval_info_3_2));

  //   REQUIRE(data2.update("weather", {"rain", 1101, NodeData::ValueType::kValueDynamic}));
  //   REQUIRE((subexpr1->evaluate(data2, eval_info_1_1) || subexpr3->evaluate(data2,
  //   eval_info_1_1)) ==
  //           expr_1_3.evaluate(data2, eval_info_1_1));
  //   REQUIRE((subexpr1->evaluate(data2, eval_info_2_1) || subexpr3->evaluate(data2,
  //   eval_info_2_1)) ==
  //           expr_1_3.evaluate(data2, eval_info_2_1));
  //   REQUIRE((subexpr1->evaluate(data2, eval_info_3_1) || subexpr3->evaluate(data2,
  //   eval_info_3_1)) ==
  //           expr_1_3.evaluate(data2, eval_info_3_1));
  //   REQUIRE((subexpr1->evaluate(data2, eval_info_1_2) || subexpr3->evaluate(data2,
  //   eval_info_1_2)) ==
  //           expr_1_3.evaluate(data2, eval_info_1_2));
  //   REQUIRE((subexpr1->evaluate(data2, eval_info_2_2) || subexpr3->evaluate(data2,
  //   eval_info_2_2)) ==
  //           expr_1_3.evaluate(data2, eval_info_2_2));
  //   REQUIRE((subexpr1->evaluate(data2, eval_info_3_2) || subexpr3->evaluate(data2,
  //   eval_info_3_2)) ==
  //           expr_1_3.evaluate(data2, eval_info_3_2));
}
