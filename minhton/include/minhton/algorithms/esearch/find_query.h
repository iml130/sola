// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#ifndef ALGORITHMS_ESEARCH_FIND_QUERY_H_
#define ALGORITHMS_ESEARCH_FIND_QUERY_H_

#include <memory>

#include "minhton/algorithms/esearch/boolean_expression.h"
#include "minhton/algorithms/esearch/distributed_data.h"
#include "minhton/algorithms/esearch/evaluation_information.h"
#include "minhton/algorithms/esearch/local_data.h"
#include "minhton/algorithms/esearch/node_data.h"
#include "minhton/core/node_info.h"
#include "solanet/serializer/serialize.h"

namespace minhton {
class FindQuery {
public:
  enum FindQueryScope {
    kAll,
    kSome,
  };

  enum FindQuerySelection {
    kSelectAll,         // every possible attribute -> SELECT * FROM ...
    kSelectSpecific,    // specific list of returns -> SELECT x,y,z FROM ...
    kSelectUnspecific,  // dont care what is being returned, except for NodeInfo
  };

  FindQuery();

  FindQuery(const std::string &expr_string, const std::string &scope_string);

  std::string serializeBooleanExpression() const;
  std::string serializeScope() const;

  void deserializeBooleanExpression(const std::string &expr_string);
  void deserializeScope(const std::string &scope_string);

  FuzzyValue evaluate(NodeData &data, bool all_information_present, const uint64_t &timestamp_now);
  std::vector<NodeData::Key> evaluateMissingAttributes(NodeData &data,
                                                       uint64_t const &timestamp_now) const;
  std::vector<NodeData::Key> getRelevantAttributes() const;
  std::vector<NodeData::Key> getRelevantTopicAttributes() const;

  FindQueryScope getScope() const;
  void setScope(const FindQueryScope &scope);

  std::shared_ptr<BooleanExpression> getBooleanExpression() const;
  void setBooleanExpression(std::shared_ptr<BooleanExpression> expr);

  void setRequestingNode(const NodeInfo &requesting_node);
  NodeInfo getRequestingNode() const;

  uint64_t getValidityThreshold() const;
  void setValidityThreshold(const uint64_t &validity_threshold);

  bool getInquireUnknownAttributes() const;
  void setInquireUnknownAttributes(bool inquire_unknown_attributes);

  bool getInquireOutdatedAttributes() const;
  void setInquireOutdatedAttributes(bool inquire_outdated_attributes);

  bool getPermissive() const;
  void setPermissive(bool permissive);

  FindQuerySelection getSelection() const;
  void setSelection(const FindQuerySelection &selection);

  std::vector<NodeData::Key> getSelectedAttributeKeys() const;
  void setSelectedAttributeKeys(const std::vector<NodeData::Key> &keys);

  template <class Archive> void save(Archive &archive) const {
    if (!this->expr_) {
      throw std::runtime_error("Expression in FindQuery is empty!");
    }
    archive(validity_threshold_, serializeBooleanExpression(), scope_, requesting_node_,
            inquire_unknown_attributes_, inquire_outdated_attributes_, permissive_, selection_,
            selected_attribute_keys_);
  }

  template <class Archive> void load(Archive &archive) {
    std::string expr_string;
    archive(validity_threshold_, expr_string, scope_, requesting_node_, inquire_unknown_attributes_,
            inquire_outdated_attributes_, permissive_, selection_, selected_attribute_keys_);
    deserializeBooleanExpression(expr_string);
  }

private:
  EvaluationInformation createEvaluationInformation(bool all_information_present,
                                                    const uint64_t &timestamp_now) const;

  // LATER change to unique_ptr if possible
  std::shared_ptr<BooleanExpression> expr_;

  // all, some...
  FindQueryScope scope_ = FindQuery::FindQueryScope::kAll;

  /// The node who initiated this find query
  NodeInfo requesting_node_;

  /// How old a value may be to be considered as up to date in milliseconds
  uint64_t validity_threshold_ = 0;  // in ms

  /// If a nodes distributed data does not contain a given key
  /// a inquiry will be sent, otherwise not
  bool inquire_unknown_attributes_;

  /// If a nodes distributed data does contain an outdated key
  /// (older than the validity threshold)
  /// a inquiry will be sent, otherwise not
  bool inquire_outdated_attributes_;

  bool permissive_;  // only relevant if inquire_outdated_attributes_=false

  FindQuerySelection selection_;

  /// only needed if selection_ == FindQuerySelection::kSelectSpecific
  std::vector<NodeData::Key> selected_attribute_keys_;
};

}  // namespace minhton

#endif
