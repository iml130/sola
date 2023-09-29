// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#include "minhton/algorithms/esearch/find_query_parser.h"

namespace minhton {

// TODO: turn into singleton

FindQueryParser::FindQueryParser() { this->initBooleanExpressionParser(); }

static std::string booleanExpressionGrammar = R"(
    expr          <- exprOR / exprAND / exprNOT / exprPRES / exprSringEq / exprNumComp / exprEmpty

    exprOR        <- '(' expr opOR expr ')'
    exprAND       <- '(' expr opAND expr ')'
    exprNOT       <- '(' opNOT expr ')'

    exprPRES      <- '(' opPRES variable ')'
    exprSringEq   <- '(' variable compOpEqualTo string ')'
    exprNumComp   <- '(' variable compOP number ')'
    exprEmpty     <- '(' empty ')'

    variable      <- < [a-z'/'] [a-zA-Z0-9-'.'':''|''/']* >
    string        <- < [a-zA-Z0-9'.'':''|''/']* >
    float         <- < [0-9]+ > '.' < [0-9]* >
    integer       <- < [0-9]+ >
    number        <- float / integer
    empty         <- 'empty'

    opOR          <- 'OR'
    opAND         <- 'AND'
    opNOT         <- 'NOT'
    opPRES        <- 'HAS'

    compOP <- compOpEqualTo / compOpNotEqualTo / compOpGreaterThanOrEqualTo / compOpLessThanOrEqualTo / compOpGreaterThan / compOpLessThan

    compOpEqualTo               <- '=='
    compOpNotEqualTo            <- '!='
    compOpGreaterThan           <- '>'
    compOpGreaterThanOrEqualTo  <- '>='
    compOpLessThan              <- '<'
    compOpLessThanOrEqualTo     <- '<='

    %whitespace   <- [ \t]*
)";

void FindQueryParser::initBooleanExpressionParser() {
  boolean_expression_parser_.log = [](std::size_t line, std::size_t col, const std::string &msg) {
    std::cerr << line << ":" << col << ": " << msg << "\n";
  };

  bool ok = boolean_expression_parser_.load_grammar(booleanExpressionGrammar);
  if (!ok) {
    throw std::logic_error("Invalid Grammar for parsing Find Query Expressions");
  }

  boolean_expression_parser_["expr"] = [](const peg::SemanticValues &vs) {
    // expr <- exprOR / exprAND / exprNOT / exprPRES / exprSringEq / exprNumComp
    switch (vs.choice()) {
      case 0:  // exprOR
        return std::dynamic_pointer_cast<BooleanExpression>(
            std::any_cast<std::shared_ptr<OrExpression>>(vs[0]));
      case 1:  // exprAND
        return std::dynamic_pointer_cast<BooleanExpression>(
            std::any_cast<std::shared_ptr<AndExpression>>(vs[0]));
      case 2:  // exprNOT
        return std::dynamic_pointer_cast<BooleanExpression>(
            std::any_cast<std::shared_ptr<NotExpression>>(vs[0]));
      case 3: {  // exprPRES
        return std::dynamic_pointer_cast<BooleanExpression>(
            std::any_cast<std::shared_ptr<PresenceExpression>>(vs[0]));
      }
      case 4:  // exprSringEq
        return std::dynamic_pointer_cast<BooleanExpression>(
            std::any_cast<std::shared_ptr<StringEqualityExpression>>(vs[0]));
      case 5:  // exprNumComp
        return std::dynamic_pointer_cast<BooleanExpression>(
            std::any_cast<std::shared_ptr<NumericComparisonExpression<float>>>(vs[0]));
      default:  // exprEmpty
        return std::dynamic_pointer_cast<BooleanExpression>(
            std::any_cast<std::shared_ptr<EmptyExpression>>(vs[0]));
    }
  };

  boolean_expression_parser_["exprOR"] = [](const peg::SemanticValues &vs) {
    // exprOR <- '(' expr opOR expr ')'
    auto expr1 = std::any_cast<std::shared_ptr<BooleanExpression>>(vs[0]);
    auto expr2 = std::any_cast<std::shared_ptr<BooleanExpression>>(vs[2]);
    return std::make_shared<OrExpression>(expr1, expr2);
  };

  boolean_expression_parser_["exprAND"] = [](const peg::SemanticValues &vs) {
    // exprAND <- '(' expr opAND expr ')'
    auto expr1 = std::any_cast<std::shared_ptr<BooleanExpression>>(vs[0]);
    auto expr2 = std::any_cast<std::shared_ptr<BooleanExpression>>(vs[2]);
    return std::make_shared<AndExpression>(expr1, expr2);
  };

  boolean_expression_parser_["exprNOT"] = [](const peg::SemanticValues &vs) {
    // exprNOT       <- '(' opNOT expr')'
    auto expr = std::any_cast<std::shared_ptr<BooleanExpression>>(vs[1]);
    return std::make_shared<NotExpression>(expr);
  };

  boolean_expression_parser_["exprPRES"] = [](const peg::SemanticValues &vs) {
    // exprPRES <- '(' opPRES variable')'
    auto variable = std::any_cast<NodeData::Key>(vs[1]);
    return std::make_shared<PresenceExpression>(variable);
  };

  boolean_expression_parser_["exprSringEq"] = [](const peg::SemanticValues &vs) {
    // exprSringEq <- '(' variable compOpEqualTo string')'
    auto variable = std::any_cast<NodeData::Key>(vs[0]);
    auto string_to_compare_to = std::any_cast<std::string>(vs[2]);
    return std::make_shared<StringEqualityExpression>(variable, string_to_compare_to);
  };

  boolean_expression_parser_["exprNumComp"] = [](const peg::SemanticValues &vs) {
    // exprNumComp <- '(' variable compOP number')'
    auto variable = std::any_cast<NodeData::Key>(vs[0]);
    auto number_to_compare_to = std::any_cast<float>(vs[2]);
    auto comp_type = std::any_cast<ComparisonTypes>(vs[1]);
    return std::make_shared<NumericComparisonExpression<float>>(variable, comp_type,
                                                                number_to_compare_to);
  };

  boolean_expression_parser_["exprEmpty"] = []([[maybe_unused]] const peg::SemanticValues &vs) {
    // exprSringEq <- '(' empty ')'
    return std::make_shared<EmptyExpression>();
  };

  boolean_expression_parser_["variable"] = [](const peg::SemanticValues &vs) {
    return vs.token_to_string();
  };

  boolean_expression_parser_["string"] = [](const peg::SemanticValues &vs) {
    return vs.token_to_string();
  };

  boolean_expression_parser_["integer"] = [](const peg::SemanticValues &vs) {
    return vs.token_to_number<int>();
  };

  boolean_expression_parser_["float"] = [](const peg::SemanticValues &vs) {
    // token_to_number does not work for floats
    std::string float_string = vs.token_to_string(0) + "." + vs.token_to_string(1);
    float fl = std::stof(float_string);
    return fl;
  };

  boolean_expression_parser_["compOpEqualTo"] = []([[maybe_unused]] const peg::SemanticValues &vs) {
    return ComparisonTypes::kEqualTo;
  };

  boolean_expression_parser_["compOpNotEqualTo"] =
      []([[maybe_unused]] const peg::SemanticValues &vs) { return ComparisonTypes::kNotEqualTo; };

  boolean_expression_parser_["compOpGreaterThan"] =
      []([[maybe_unused]] const peg::SemanticValues &vs) { return ComparisonTypes::kGreater; };

  boolean_expression_parser_["compOpGreaterThanOrEqualTo"] =
      []([[maybe_unused]] const peg::SemanticValues &vs) {
        return ComparisonTypes::kGreaterThanOrEqualTo;
      };

  boolean_expression_parser_["compOpLessThan"] =
      []([[maybe_unused]] const peg::SemanticValues &vs) { return ComparisonTypes::kLessThan; };

  boolean_expression_parser_["compOpLessThanOrEqualTo"] =
      []([[maybe_unused]] const peg::SemanticValues &vs) {
        return ComparisonTypes::kLessThanOrEqualTo;
      };
}

std::shared_ptr<BooleanExpression> FindQueryParser::parseBooleanExpression(
    const std::string &expr_string) {
  std::shared_ptr<BooleanExpression> expr;
  boolean_expression_parser_.parse(expr_string, expr);
  return expr;
}

}  // namespace minhton
