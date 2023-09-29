// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#ifndef MINHTON_ALGORITHMS_ESEARCH_BOOLEAN_EXPRESSION_H_
#define MINHTON_ALGORITHMS_ESEARCH_BOOLEAN_EXPRESSION_H_

#include <memory>
#include <vector>

#include "minhton/algorithms/esearch/evaluation_information.h"
#include "minhton/algorithms/esearch/fuzzy_value.h"
#include "minhton/algorithms/esearch/node_data.h"

namespace minhton {

// TODO move into numeric comparison expression
enum class ComparisonTypes {
  kEqualTo,
  kNotEqualTo,
  kLessThan,
  kGreater,
  kLessThanOrEqualTo,
  kGreaterThanOrEqualTo
};

class BooleanExpression {
public:
  virtual ~BooleanExpression() = default;
  virtual FuzzyValue evaluate(NodeData &data, const EvaluationInformation &eval_info) = 0;

  virtual std::vector<NodeData::Key> evaluateMissingAttributes(
      NodeData &data, const EvaluationInformation &eval_info) const = 0;

  virtual std::vector<NodeData::Key> getRelevantKeys() const = 0;
  virtual std::vector<NodeData::Key> getRelevantTopicKeys() const = 0;

  virtual uint8_t getDepth() = 0;

  virtual std::string serialize() = 0;
};

class AtomicBooleanExpression : public BooleanExpression {
public:
  explicit AtomicBooleanExpression(NodeData::Key key) : key_(std::move(key)){};

  ~AtomicBooleanExpression() override = default;

  std::vector<NodeData::Key> evaluateMissingAttributes(
      NodeData &data, const EvaluationInformation &eval_info) const override {
    if (eval_info.inquire_unknown_attributes && eval_info.inquire_outdated_attributes) {
      // if we do not have the key, or its outdated, we inquire
      if (!data.hasKey(key_) ||
          !data.isValueUpToDate(key_, eval_info.validity_threshold_timestamp)) {
        return {key_};
      }
      return {};
    }

    if (eval_info.inquire_unknown_attributes && !eval_info.inquire_outdated_attributes) {
      // if we have the key, we dont have to do anything
      // if we do not have the key, we inquire
      if (!data.hasKey(key_)) {
        return {key_};
      }
      return {};
    }

    if (!eval_info.inquire_unknown_attributes && eval_info.inquire_outdated_attributes) {
      // if we have the key, we inquire outdated
      // if we do not have the key, we do not inquire
      if (data.hasKey(key_) &&
          !data.isValueUpToDate(key_, eval_info.validity_threshold_timestamp)) {
        return {key_};
      }
      return {};
    }

    if (!eval_info.inquire_unknown_attributes && !eval_info.inquire_outdated_attributes) {
      // not inquiring anything
      return {};
    }

    throw std::logic_error("should not be reached");
  }

  FuzzyValue evaluate(NodeData &data, const EvaluationInformation &eval_info) override {
    if (data.isLocal()) {
      return evaluateExisting(data, eval_info);
    }

    if (!data.hasKey(key_)) {
      if (!eval_info.all_information_present && eval_info.inquire_unknown_attributes) {
        return FuzzyValue::createUndecided();
      }
      return FuzzyValue::createFalse();
    }

    // we have the key

    if (eval_info.inquire_outdated_attributes &&
        !data.isValueUpToDate(key_, eval_info.validity_threshold_timestamp)) {
      if (eval_info.all_information_present) {
        return FuzzyValue::createFalse();
      }
      return FuzzyValue::createUndecided();
    }

    // the value is up to date, or we do not care about whether the value is up to date or not
    FuzzyValue value = evaluateExisting(data, eval_info);
    if (value.isUndecided() && !eval_info.inquire_outdated_attributes && eval_info.permissive) {
      return FuzzyValue::createTrue();
    }

    return value;
  }

  virtual FuzzyValue evaluateExisting(NodeData &data, const EvaluationInformation &eval_info) = 0;

  uint8_t getDepth() override { return 1; }

protected:
  NodeData::Key key_;
};

class OrExpression : public BooleanExpression {
public:
  explicit OrExpression(std::shared_ptr<BooleanExpression> expr1,
                        std::shared_ptr<BooleanExpression> expr2)
      : expr1_(std::move(expr1)), expr2_(std::move(expr2)){};

  ~OrExpression() override = default;

  FuzzyValue evaluate(NodeData &data, const EvaluationInformation &eval_info) override {
    return expr1_->evaluate(data, eval_info) || expr2_->evaluate(data, eval_info);
  }

  std::vector<NodeData::Key> evaluateMissingAttributes(
      NodeData &data, const EvaluationInformation &eval_info) const override {
    EvaluationInformation eval_info_c = eval_info;
    eval_info_c.all_information_present = false;

    auto expr1_eval = expr1_->evaluate(data, eval_info_c);
    auto expr2_eval = expr2_->evaluate(data, eval_info_c);

    if (expr1_eval.isTrue() || expr2_eval.isTrue()) {
      return {};
    }

    auto missings = expr1_->evaluateMissingAttributes(data, eval_info_c);
    auto temp_missings = expr2_->evaluateMissingAttributes(data, eval_info_c);
    missings.insert(missings.end(), temp_missings.begin(), temp_missings.end());
    return missings;
  }

  std::vector<NodeData::Key> getRelevantKeys() const override {
    auto relevants = expr1_->getRelevantKeys();
    auto temp_relevants = expr2_->getRelevantKeys();
    relevants.insert(relevants.end(), temp_relevants.begin(), temp_relevants.end());
    return relevants;
  }

  std::vector<NodeData::Key> getRelevantTopicKeys() const override {
    auto relevants = expr1_->getRelevantTopicKeys();
    auto temp_relevants = expr2_->getRelevantTopicKeys();
    relevants.insert(relevants.end(), temp_relevants.begin(), temp_relevants.end());
    return relevants;
  }

  std::string serialize() override {
    return "( " + expr1_->serialize() + " OR " + expr2_->serialize() + " )";
  }

  uint8_t getDepth() override { return expr1_->getDepth() + expr2_->getDepth(); }

private:
  std::shared_ptr<BooleanExpression> expr1_;
  std::shared_ptr<BooleanExpression> expr2_;
};

class AndExpression : public BooleanExpression {
public:
  explicit AndExpression(std::shared_ptr<BooleanExpression> expr1,
                         std::shared_ptr<BooleanExpression> expr2)
      : expr1_(std::move(expr1)), expr2_(std::move(expr2)){};

  ~AndExpression() override = default;

  FuzzyValue evaluate(NodeData &data, const EvaluationInformation &eval_info) override {
    return expr1_->evaluate(data, eval_info) && expr2_->evaluate(data, eval_info);
  }

  std::vector<NodeData::Key> evaluateMissingAttributes(
      NodeData &data, const EvaluationInformation &eval_info) const override {
    auto missings = expr1_->evaluateMissingAttributes(data, eval_info);
    auto temp_missings = expr2_->evaluateMissingAttributes(data, eval_info);
    missings.insert(missings.end(), temp_missings.begin(), temp_missings.end());
    return missings;
  }

  std::vector<NodeData::Key> getRelevantKeys() const override {
    auto relevants = expr1_->getRelevantKeys();
    auto temp_relevants = expr2_->getRelevantKeys();
    relevants.insert(relevants.end(), temp_relevants.begin(), temp_relevants.end());
    return relevants;
  }

  std::vector<NodeData::Key> getRelevantTopicKeys() const override {
    auto relevants = expr1_->getRelevantTopicKeys();
    auto temp_relevants = expr2_->getRelevantTopicKeys();
    relevants.insert(relevants.end(), temp_relevants.begin(), temp_relevants.end());
    return relevants;
  }

  std::string serialize() override {
    return "( " + expr1_->serialize() + " AND " + expr2_->serialize() + " )";
  }

  uint8_t getDepth() override { return expr1_->getDepth() + expr2_->getDepth(); }

private:
  std::shared_ptr<BooleanExpression> expr1_;
  std::shared_ptr<BooleanExpression> expr2_;
};

class NotExpression : public BooleanExpression {
public:
  explicit NotExpression(std::shared_ptr<BooleanExpression> expr) : expr_(std::move(expr)){};

  ~NotExpression() override = default;

  FuzzyValue evaluate(NodeData &data, const EvaluationInformation &eval_info) override {
    auto val_pos = expr_->evaluate(data, eval_info);
    auto val_neg = !val_pos;
    return val_neg;
  }

  std::vector<NodeData::Key> evaluateMissingAttributes(
      NodeData &data, const EvaluationInformation &eval_info) const override {
    return expr_->evaluateMissingAttributes(data, eval_info);
  }

  std::vector<NodeData::Key> getRelevantKeys() const override { return expr_->getRelevantKeys(); }
  std::vector<NodeData::Key> getRelevantTopicKeys() const override {
    return expr_->getRelevantTopicKeys();
  }

  std::string serialize() override { return "( NOT " + expr_->serialize() + " )"; }

  uint8_t getDepth() override { return expr_->getDepth(); }

private:
  std::shared_ptr<BooleanExpression> expr_;
};

class PresenceExpression : public AtomicBooleanExpression {
public:
  explicit PresenceExpression(const NodeData::Key &key) : AtomicBooleanExpression(key){};

  ~PresenceExpression() override = default;

  FuzzyValue evaluateExisting(NodeData &data,
                              [[maybe_unused]] const EvaluationInformation &eval_info) override {
    if (!data.hasKey(key_)) {
      return FuzzyValue::createFalse();
    }

    auto variant = data.getValue(key_);
    auto pval = std::get_if<bool>(&variant);
    if (pval != nullptr) {
      // if value is bool, return value converted to fuzzy value
      return *pval ? FuzzyValue::createTrue() : FuzzyValue::createFalse();
    }
    return FuzzyValue::createTrue();
  }

  std::vector<NodeData::Key> getRelevantKeys() const override { return {key_}; }
  std::vector<NodeData::Key> getRelevantTopicKeys() const override { return {key_}; }

  std::string serialize() override { return "( HAS " + key_ + " )"; }
};

class StringEqualityExpression : public AtomicBooleanExpression {
public:
  explicit StringEqualityExpression(const NodeData::Key &key, std::string value)
      : AtomicBooleanExpression(key), value_(std::move(value)){};

  ~StringEqualityExpression() override = default;

  FuzzyValue evaluateExisting(NodeData &data,
                              [[maybe_unused]] const EvaluationInformation &eval_info) override {
    if (!data.hasKey(key_)) {
      return FuzzyValue::createFalse();
    }
    NodeData::Value variant = data.getValue(key_);
    auto pval = std::get_if<std::string>(&variant);
    if (pval != nullptr) {
      if ((*pval) == value_) {
        return FuzzyValue::createTrue();
      }
    }

    return FuzzyValue::createFalse();
  }

  std::vector<NodeData::Key> getRelevantKeys() const override { return {key_}; }
  std::vector<NodeData::Key> getRelevantTopicKeys() const override { return {}; }

  std::string serialize() override { return "( " + key_ + " == " + value_ + " )"; }

private:
  std::string value_;
};

template <typename NumericType> class NumericComparisonExpression : public AtomicBooleanExpression {
  static_assert(std::is_same_v<NumericType, int> || std::is_same_v<NumericType, float>,
                "NumericType must be int or float");

public:
  explicit NumericComparisonExpression(NodeData::Key key, ComparisonTypes comparison_type,
                                       NumericType comparison_value)
      : AtomicBooleanExpression(key),
        comparison_type_(comparison_type),
        comparison_value_(comparison_value){};

  ~NumericComparisonExpression() override = default;

  FuzzyValue evaluateExisting(NodeData &data,
                              [[maybe_unused]] const EvaluationInformation &eval_info) override {
    if (!data.hasKey(key_)) return FuzzyValue::createFalse();

    NodeData::Value variant = data.getValue(key_);

    bool comparison = false;
    bool compared = false;

    try {
      int int_value = std::get<int>(variant);
      comparison = applyComparison<int>(int_value);
      compared = true;
    } catch (std::bad_variant_access const &ex) {
    }

    if (!compared) {
      try {
        float float_value = std::get<float>(variant);
        comparison = applyComparison<float>(float_value);
        compared = true;
      } catch (std::bad_variant_access const &ex) {
      }
    }

    if (compared) {
      if (comparison) {
        return FuzzyValue::createTrue();
      }
      return FuzzyValue::createFalse();
    }
    throw std::invalid_argument("Could not compare");
  }

  std::vector<NodeData::Key> getRelevantKeys() const override { return {key_}; }
  std::vector<NodeData::Key> getRelevantTopicKeys() const override { return {}; }

  std::string serialize() override {
    std::string comp_str;
    switch (comparison_type_) {
      case ComparisonTypes::kEqualTo:
        comp_str = "==";
        break;
      case ComparisonTypes::kGreater:
        comp_str = ">";
        break;
      case ComparisonTypes::kGreaterThanOrEqualTo:
        comp_str = ">=";
        break;
      case ComparisonTypes::kLessThan:
        comp_str = "<";
        break;
      case ComparisonTypes::kLessThanOrEqualTo:
        comp_str = "<=";
        break;
      case ComparisonTypes::kNotEqualTo:
        comp_str = "!=";
        break;
    }

    return "( " + key_ + " " + comp_str + " " + std::to_string(comparison_value_) + " )";
  }

private:
  ComparisonTypes comparison_type_;
  NumericType comparison_value_;

  template <typename OtherNumericType> bool applyComparison(OtherNumericType evaluation_value) {
    static_assert(std::is_arithmetic<OtherNumericType>::value, "OtherNumericType must be numeric");

    switch (comparison_type_) {
      case ComparisonTypes::kEqualTo: {
        return evaluation_value == comparison_value_;
      }
      case ComparisonTypes::kNotEqualTo: {
        return evaluation_value != comparison_value_;
      }
      case ComparisonTypes::kLessThan: {
        return evaluation_value < comparison_value_;
      }
      case ComparisonTypes::kGreater: {
        return evaluation_value > comparison_value_;
      }
      case ComparisonTypes::kLessThanOrEqualTo: {
        return evaluation_value <= comparison_value_;
      }
      case ComparisonTypes::kGreaterThanOrEqualTo: {
        return evaluation_value >= comparison_value_;
      }
    }

    return false;
  }
};

class EmptyExpression : public BooleanExpression {
public:
  explicit EmptyExpression() = default;
  ~EmptyExpression() override = default;

  FuzzyValue evaluate([[maybe_unused]] NodeData &data,
                      [[maybe_unused]] const EvaluationInformation &eval_info) override {
    return FuzzyValue::createTrue();
  }

  std::vector<NodeData::Key> evaluateMissingAttributes(
      [[maybe_unused]] NodeData &data,
      [[maybe_unused]] const EvaluationInformation &eval_info) const override {
    return {};
  }

  std::vector<NodeData::Key> getRelevantKeys() const override { return {}; }
  std::vector<NodeData::Key> getRelevantTopicKeys() const override { return {}; }
  std::string serialize() override { return "( empty )"; }
  uint8_t getDepth() override { return 0; }
};

}  // namespace minhton

#endif
