// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#ifndef NATTER_UTILS_TREE_HELPER_H_
#define NATTER_UTILS_TREE_HELPER_H_

#include <cassert>
#include <cstdint>
#include <set>
#include <string>

#include "natter/minhcast_level_number.h"

namespace natter::minhcast {

/**
 * Check if own_node is a (higher level) children of initial_node.
 * E.g. all nodes except root are higher level children of {0,0,m}
 * @param own_node
 * @param initial_node
 * @return
 */
constexpr bool childFromNode(LevelNumber own_node, LevelNumber initial_node) {
  const auto [own_level, own_number, fanout] = own_node;
  const auto [initial_level, initial_number, initial_fanout] = initial_node;
  assert(fanout == initial_fanout);
  uint32_t parent = own_number;
  if (own_level <= initial_level) return false;
  for (uint32_t current_level = own_level; current_level > initial_level; current_level--) {
    parent = parent / fanout;
    if ((current_level - 1) == initial_level && parent == initial_number) return true;
  }
  return false;
}

/**
 * Calculate right routing table of node
 * @param node
 * @return
 */
std::set<LevelNumber> calculateRRT(LevelNumber node);

/**
 * Calculate left routing table of node
 * @param node
 * @return
 */
std::set<LevelNumber> calculateLRT(LevelNumber node);

std::string toLevelNumberPair(LevelNumber node);
LevelNumber fromStringToLevelNumber(const std::string &node);
}  // namespace natter::minhcast

#endif  // DAISI_TREE_HELPER_H_
