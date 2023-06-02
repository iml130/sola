// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#include "minhton/algorithms/esearch/find_query.h"

#include "minhton/algorithms/esearch/find_query_parser.h"
namespace minhton {

FindQuery::FindQuery() {
  this->expr_ = nullptr;
  this->inquire_outdated_attributes_ = true;
  this->inquire_unknown_attributes_ = true;
  this->permissive_ = false;
  this->selection_ = FindQuery::FindQuerySelection::kSelectUnspecific;
}

FindQuery::FindQuery(const std::string &expr_string, const std::string &scope_string) {
  deserializeBooleanExpression(expr_string);
  deserializeScope(scope_string);
  this->inquire_outdated_attributes_ = true;
  this->inquire_unknown_attributes_ = true;
  this->permissive_ = false;
  this->selection_ = FindQuery::FindQuerySelection::kSelectUnspecific;
}

void FindQuery::deserializeBooleanExpression(const std::string &expr_string) {
  FindQueryParser parser;
  this->expr_ = parser.parseBooleanExpression(expr_string);
}

std::string FindQuery::serializeBooleanExpression() const { return this->expr_->serialize(); }

void FindQuery::deserializeScope(const std::string &scope_string) {
  if (scope_string == "all") {
    this->scope_ = FindQuery::kAll;
  } else if (scope_string == "some") {
    this->scope_ = FindQueryScope::kSome;
  } else {
    throw std::invalid_argument("invalid scope string");
  }
}

std::string FindQuery::serializeScope() const {
  if (this->scope_ == FindQuery::FindQueryScope::kAll) {
    return "all";
  }
  if (this->scope_ == FindQuery::FindQueryScope::kSome) {
    return "some";
  }
  throw std::logic_error("invalid scope");
}

FuzzyValue FindQuery::evaluate(NodeData &data, bool all_information_present,
                               const uint64_t &timestamp_now) {
  auto eval_info = createEvaluationInformation(all_information_present, timestamp_now);

  FuzzyValue val = expr_->evaluate(data, eval_info);
  if (all_information_present && val.isUndecided()) {
    // if we have local data, undecided means false
    // because we know all information and cannot be undecided

    return FuzzyValue::createFalse();
  }

  return val;
}

std::vector<NodeData::Key> FindQuery::evaluateMissingAttributes(
    NodeData &data, uint64_t const &timestamp_now) const {
  if (selection_ == FindQuery::FindQuerySelection::kSelectAll) {
    // just setting the flag to inquire all in the message
    return {};
  }

  auto eval_info = createEvaluationInformation(false, timestamp_now);
  auto missing_attributes = this->expr_->evaluateMissingAttributes(data, eval_info);

  if (selection_ == FindQuery::FindQuerySelection::kSelectUnspecific) {
    // returning only the least neccessary keys, because we don't care about the return values
    return missing_attributes;
  }

  if (selection_ == FindQuery::FindQuerySelection::kSelectSpecific) {
    // all keys we know about

    // union without duplicates
    missing_attributes.insert(missing_attributes.end(), selected_attribute_keys_.begin(),
                              selected_attribute_keys_.end());
    std::sort(missing_attributes.begin(), missing_attributes.end());
    missing_attributes.erase(std::unique(missing_attributes.begin(), missing_attributes.end()),
                             missing_attributes.end());

    return missing_attributes;
  }

  return {};
}

std::vector<NodeData::Key> FindQuery::getRelevantAttributes() const {
  return this->expr_->getRelevantKeys();
}

std::vector<NodeData::Key> FindQuery::getRelevantTopicAttributes() const {
  return this->expr_->getRelevantTopicKeys();
}

FindQuery::FindQueryScope FindQuery::getScope() const { return this->scope_; }

void FindQuery::setScope(const FindQuery::FindQueryScope &scope) { this->scope_ = scope; }

std::shared_ptr<BooleanExpression> FindQuery::getBooleanExpression() const { return this->expr_; }

void FindQuery::setBooleanExpression(std::shared_ptr<BooleanExpression> expr) {
  this->expr_ = expr;
}

void FindQuery::setRequestingNode(const NodeInfo &requesting_node) {
  this->requesting_node_ = requesting_node;
}

NodeInfo FindQuery::getRequestingNode() const { return this->requesting_node_; }

uint64_t FindQuery::getValidityThreshold() const { return this->validity_threshold_; }

void FindQuery::setValidityThreshold(const uint64_t &validity_threshold) {
  this->validity_threshold_ = validity_threshold;
}

bool FindQuery::getInquireUnknownAttributes() const { return inquire_unknown_attributes_; }

void FindQuery::setInquireUnknownAttributes(bool inquire_unknown_attributes) {
  inquire_unknown_attributes_ = inquire_unknown_attributes;
}

bool FindQuery::getInquireOutdatedAttributes() const { return inquire_outdated_attributes_; }

void FindQuery::setInquireOutdatedAttributes(bool inquire_outdated_attributes) {
  inquire_outdated_attributes_ = inquire_outdated_attributes;
}

bool FindQuery::getPermissive() const { return permissive_; }

void FindQuery::setPermissive(bool permissive) { permissive_ = permissive; }

std::vector<NodeData::Key> FindQuery::getSelectedAttributeKeys() const {
  return selected_attribute_keys_;
}

FindQuery::FindQuerySelection FindQuery::getSelection() const { return selection_; }

void FindQuery::setSelection(const FindQuery::FindQuerySelection &selection) {
  selection_ = selection;
}

void FindQuery::setSelectedAttributeKeys(const std::vector<NodeData::Key> &keys) {
  selected_attribute_keys_ = keys;
}

EvaluationInformation FindQuery::createEvaluationInformation(bool all_information_present,
                                                             const uint64_t &timestamp_now) const {
  return EvaluationInformation{timestamp_now - validity_threshold_, all_information_present,
                               inquire_unknown_attributes_, inquire_outdated_attributes_,
                               !inquire_outdated_attributes_ ? permissive_ : false};
}

}  // namespace minhton
