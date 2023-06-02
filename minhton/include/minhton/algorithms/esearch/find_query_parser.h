// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#ifndef ALGORITHMS_ESEARCH_FIND_QUERY_PARSER_H_
#define ALGORITHMS_ESEARCH_FIND_QUERY_PARSER_H_

#include <memory>

#include "minhton/algorithms/esearch/find_query.h"
#include "peglib/peglib.h"

namespace minhton {

class FindQueryParser {
public:
  FindQueryParser();

  std::shared_ptr<BooleanExpression> parseBooleanExpression(const std::string &expr_string);

private:
  peg::parser boolean_expression_parser_;

  void initBooleanExpressionParser();
};

}  // namespace minhton

#endif
