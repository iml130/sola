// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#include "minhton/core/routing_calculations.h"

#include <algorithm>
#include <cmath>
#include <stdexcept>

#include "minhton/core/constants.h"
namespace minhton {

bool getFillLevelRightToLeft(uint32_t level) { return (level % 2) == 0; }

bool isNodePartOfPrioSet(uint32_t level, uint32_t number, uint16_t fanout) {
  std::set<uint32_t> prio_set = calcPrioSet(level, fanout);

  return prio_set.find(number) != prio_set.end();
}

std::tuple<uint32_t, uint32_t> calcParent(uint32_t level, uint32_t number, uint16_t fanout) {
  if (!isPositionValid(level, number, fanout)) {
    throw std::invalid_argument("Invalid Position");
  }

  if (level == 0) {
    throw std::invalid_argument("Cannot get the parent of root");
  }

  uint32_t t = number - (number % fanout);
  uint32_t offset = t % fanout != 0 ? 1 : 0;
  // Upper Division
  uint32_t p_number = t / fanout + offset;
  uint32_t p_level = level - 1;

  return std::make_tuple(p_level, p_number);
}

std::vector<std::tuple<uint32_t, uint32_t>> calcChildren(uint32_t level, uint32_t number,
                                                         uint16_t fanout) {
  if (!isPositionValid(level, number, fanout)) {
    throw std::invalid_argument("Invalid Position");
  }

  if (level == UINT16_MAX) {
    throw std::invalid_argument("Overflow in level");
  }

  uint32_t c_level = level + 1;
  std::vector<std::tuple<uint32_t, uint32_t>> child_vector;

  for (int i = 0; i < fanout; i++) {
    uint32_t c_number = (number * fanout) + i;

    if (c_number < number) {
      // This case happens after an Overflow of Number
      // usually, the child should have a higher number
      throw std::invalid_argument("Overflow in Number");
    }

    child_vector.push_back(std::make_tuple(c_level, c_number));
  }

  return child_vector;
}

std::vector<uint32_t> calcRoutingSequence(uint32_t level, uint16_t fanout) {
  if (!isFanoutValid(fanout)) {
    throw std::invalid_argument("Invalid Fanout");
  }

  uint16_t i_fanout = fanout;
  uint32_t i_level = level;

  std::vector<uint32_t> seq;
  for (uint32_t i = 0; i < (i_fanout - 1) * i_level; i++) {
    for (uint32_t d = 1; d < i_fanout; d++) {
      seq.push_back(d * (pow(i_fanout, i)));
    }
  }

  return seq;
}

std::vector<std::tuple<uint32_t, uint32_t>> calcLeftRT(uint32_t level, uint32_t number,
                                                       uint16_t fanout) {
  if (!isPositionValid(level, number, fanout)) {
    throw std::invalid_argument("Invalid Position");
  }

  std::vector<uint32_t> seq = calcRoutingSequence(level, fanout);
  std::vector<std::tuple<uint32_t, uint32_t>> lrt;

  for (const unsigned int i : seq) {
    if (i > number) {
      break;
    }
    lrt.push_back(std::make_tuple(level, number - i));
  }

  return lrt;
}

std::vector<std::tuple<uint32_t, uint32_t>> calcRightRT(uint32_t level, uint32_t number,
                                                        uint16_t fanout) {
  if (!isPositionValid(level, number, fanout)) {
    throw std::invalid_argument("Invalid Position");
  }

  std::vector<uint32_t> seq = calcRoutingSequence(level, fanout);
  std::vector<std::tuple<uint32_t, uint32_t>> rrt;

  // Preventing Overflow, at leadt for uint16_t
  uint64_t i_fanout = fanout;
  uint64_t i_level = level;
  uint64_t max_number = pow(i_fanout, i_level);

  for (const unsigned int i : seq) {
    uint32_t k = number + i;
    if (k >= max_number) {
      break;
    }
    if (k < number) {
      // This case happens after an Overflow of Number
      // usually, the child should have a higher number
      throw std::invalid_argument("Overflow in Number");
    }

    rrt.push_back(std::make_tuple(level, k));
  }

  return rrt;
}

/// returns an ordered set
std::set<uint32_t> calcPrioSet(uint32_t level, uint16_t fanout) {
  if (level == 0) {
    return std::set<uint32_t>{0};
  }
  if (level == 1) {
    return std::set<uint32_t>{(uint32_t)ceil(fanout / 2.0)};  // NOLINT(readability-magic-numbers)
  }

  uint32_t i = fanout;
  uint32_t max_num = pow(fanout, level) - 1;

  std::set<uint32_t> prio_set{fanout};
  while (i <= max_num) {
    i += 2 * fanout;
    if (i <= max_num) {
      prio_set.insert(i);
    }
  }

  uint16_t last_prio = *prio_set.rbegin();
  if (last_prio < max_num - fanout) {
    prio_set.insert(max_num - fanout);
  }

  return prio_set;
}

// depending on the level, number and fanout this method valides if the computed position is valid
// returns true if valid; false if not valid
bool isPositionValid(uint32_t level, uint32_t number, uint16_t fanout) {
  if (!isFanoutValid(fanout)) {
    return false;
  }

  if (number > pow(fanout, level) - 1) {
    return false;
  }

  return true;
}

bool isFanoutValid(uint16_t fanout) {
  if (fanout < kFanoutMinimum) {
    return false;
  }

  if (fanout > kFanoutMaximum) {
    return false;
  }

  return true;
}

std::tuple<double, double, double> treeMapperInternal(uint16_t level, uint16_t number,
                                                      uint8_t fanout, uint8_t K) {
  double lower = NAN;
  double upper = NAN;
  double center = NAN;

  if (level == 0) {
    lower = 0;
    upper = (K / ceil(fanout / 2.0)) * fanout;  // NOLINT(readability-magic-numbers)
    center = lower +
             ((upper - lower) / fanout) * ceil(fanout / 2.0);  // NOLINT(readability-magic-numbers)
  } else {
    uint16_t parent_level = level - 1;
    uint16_t parent_number = (number - (number % fanout)) / fanout;

    // recursive call to get the parent bounds
    std::tuple<double, double, double> parent_results =
        treeMapperInternal(parent_level, parent_number, fanout, K);

    double lower_parent = std::get<0>(parent_results);
    double upper_parent = std::get<1>(parent_results);
    double next_width =
        (upper_parent - lower_parent) / (fanout * 1.0);  // step width of children of our parent

    lower = lower_parent + next_width * (number % fanout);
    upper = lower + next_width;
    center = lower +
             ((upper - lower) / fanout) * ceil(fanout / 2.0);  // NOLINT(readability-magic-numbers)
  }

  return std::make_tuple(lower, upper, center);
}

double treeMapper(uint16_t level, uint16_t number, uint8_t fanout, double K) {
  // extracting the center from the full results with bounds
  std::tuple<double, double, double> result = treeMapperInternal(level, number, fanout, K);
  return std::get<2>(result);
}

std::vector<uint32_t> getDSNSet(uint32_t level, uint16_t fanout) {
  std::vector<uint32_t> dsn_set;

  if (level % 2 == 1) {
    return dsn_set;
  }
  if (level == 0) {
    dsn_set.push_back(0);
    return dsn_set;
  }

  uint32_t max_number = pow(fanout, level) - 1;
  uint32_t i = fanout;

  while (i <= max_number) {
    dsn_set.push_back(i);
    i += 2 * fanout;
  }

  if (dsn_set[dsn_set.size() - 1] < max_number - fanout) {
    dsn_set.push_back(max_number - fanout);
  }

  return dsn_set;
}

std::vector<std::tuple<uint32_t, uint32_t>> getCoverArea(uint32_t level, uint32_t number,
                                                         uint16_t fanout) {
  std::vector<std::tuple<uint32_t, uint32_t>> area;

  if (level == 0 && number == 0) {
    for (uint16_t i = 0; i < fanout; i++) {
      area.push_back({1, i});
    }
    return area;
  }

  auto dsn_set = getDSNSet(level, fanout);
  if (dsn_set.empty()) {
    return area;
  }

  auto iter = std::find(dsn_set.begin(), dsn_set.end(), number);

  if (iter == dsn_set.end()) {
    return area;
  }

  uint32_t max_number = pow(fanout, level) - 1;
  uint32_t index = std::distance(dsn_set.begin(), iter);

  bool first = index == 0;
  bool last = index == dsn_set.size() - 1;

  if (first) {
    for (uint32_t i = 0; i < number; i++) {
      area.push_back(std::make_tuple(level, i));
    }
  } else {
    uint32_t prior = dsn_set[index - 1];
    auto our_gap = (uint32_t)ceil((number - prior) / 2);
    for (uint32_t i = number - our_gap + 1; i < number; i++) {
      area.push_back(std::make_tuple(level, i));
    }
  }

  if (last) {
    for (uint32_t i = number + 1; i <= max_number; i++) {
      area.push_back(std::make_tuple(level, i));
    }
  } else {
    uint32_t after = dsn_set[index + 1];
    auto our_gap = (uint32_t)floor((after - number) / 2);
    for (uint32_t i = number + 1; i < number + our_gap + 1; i++) {
      area.push_back(std::make_tuple(level, i));
    }
  }

  std::vector<std::tuple<uint32_t, uint32_t>> covered_children;
  for (auto const &covered : area) {
    auto children = calcChildren(std::get<0>(covered), std::get<1>(covered), fanout);
    covered_children.insert(covered_children.end(), children.begin(), children.end());
  }
  area.insert(area.end(), covered_children.begin(), covered_children.end());

  auto dsn_children = calcChildren(level, number, fanout);
  area.insert(area.end(), dsn_children.begin(), dsn_children.end());

  return area;
}

std::tuple<uint32_t, uint32_t> getCoveringDSN(uint32_t level, uint32_t number, uint16_t fanout) {
  std::vector<uint32_t> dsn_set;

  if (level % 2 == 0) {
    dsn_set = getDSNSet(level, fanout);
  } else {
    dsn_set = getDSNSet(level - 1, fanout);

    auto parent_pos = calcParent(level, number, fanout);
    level = std::get<0>(parent_pos);
    number = std::get<1>(parent_pos);
  }

  uint32_t last_dsn = dsn_set[dsn_set.size() - 1];
  auto lower = std::lower_bound(dsn_set.begin(), dsn_set.end(), number);

  bool is_dsn = *lower == number;
  bool before_first_dsn = lower == dsn_set.begin() && number < *lower;
  bool after_last_dsn = number > last_dsn;

  if (is_dsn || before_first_dsn) {
    return std::make_tuple(level, *lower);
  }

  if (after_last_dsn) {
    return std::make_tuple(level, last_dsn);
  }

  uint32_t after = *lower;
  uint32_t prior = *--lower;

  uint32_t lower_dist = number - prior;
  uint32_t upper_dist = after - number;

  if (upper_dist < lower_dist) {
    return std::make_tuple(level, after);
  }
  return std::make_tuple(level, prior);
}

}  // namespace minhton
