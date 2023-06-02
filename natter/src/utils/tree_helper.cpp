// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#include "tree_helper.h"

#include <cmath>
#include <sstream>

namespace natter::minhcast {

std::set<LevelNumber> calculateRRT(LevelNumber node) {
  const auto [level, number, fanout] = node;
  std::set<LevelNumber> nodes;
  const uint32_t max_number_on_level = std::pow(fanout, level);
  for (int d = 1; d < fanout; d++) {
    int j = 0;
    while (number + d * std::pow(fanout, j) < max_number_on_level) {
      const uint32_t current_neighbor_number = number + d * std::pow(fanout, j);
      nodes.insert({level, current_neighbor_number, fanout});
      j++;
    }
  }
  return nodes;
}

std::set<LevelNumber> calculateLRT(LevelNumber node) {
  const auto [level, number, fanout] = node;
  std::set<LevelNumber> nodes;
  for (int d = 1; d < fanout; d++) {
    int j = 0;
    while (number - d * std::pow(fanout, j) >= 0) {
      const uint32_t current_neighbor_number = number - d * std::pow(fanout, j);
      nodes.insert({level, current_neighbor_number, fanout});
      j++;
    }
  }
  return nodes;
}

std::string toLevelNumberPair(LevelNumber node) {
  std::stringstream stream;
  const auto [level, number, fanout] = node;
  stream << std::to_string(level) << ":" << std::to_string(number);
  return stream.str();
}

LevelNumber fromStringToLevelNumber(const std::string &node) {
  uint32_t level = std::stoi(node.substr(0, node.find(':')));
  uint32_t number = std::stoi(node.substr(node.find(':') + 1, node.size()));
  return {level, number, 0};
}

}  // namespace natter::minhcast
