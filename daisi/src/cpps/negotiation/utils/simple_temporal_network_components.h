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

#ifndef DAISI_NEGOTIATION_UTILS_SIMPLE_TEMPORAL_NETWORK_COMPONENTS_NS3_H_
#define DAISI_NEGOTIATION_UTILS_SIMPLE_TEMPORAL_NETWORK_COMPONENTS_NS3_H_

#include <cassert>

#include "ns3/vector.h"

namespace daisi::cpps {

struct STN_Vertex {  // NOLINT(readability-identifier-naming)
  bool start;
  std::string task_id;
  ns3::Vector position;
  double value;

  friend bool operator==(const STN_Vertex &v1, const STN_Vertex &v2) {
    return v1.start == v2.start && v1.task_id == v2.task_id;
  }

  friend bool operator<(const STN_Vertex &v1, const STN_Vertex &v2) {
    if (v1.start == v2.start) {
      return v1.task_id < v2.task_id;
    }
    return v1.start < v2.start;  // NOLINT(readability-implicit-bool-conversion)
  }

  friend bool operator<=(const STN_Vertex &v1, const STN_Vertex &v2) {
    if (v1.start == v2.start && v1.task_id == v2.task_id) {
      return true;
    }
    if (v1.start == v2.start) {
      return v1.task_id < v2.task_id;
    }
    return v1.start < v2.start;  // NOLINT(readability-implicit-bool-conversion)
  }

  friend bool operator>(const STN_Vertex &v1, const STN_Vertex &v2) {
    if (v1.start == v2.start) {
      return v1.task_id > v2.task_id;
    }
    return v1.start > v2.start;  // NOLINT(readability-implicit-bool-conversion)
  }

  friend bool operator>=(const STN_Vertex &v1, const STN_Vertex &v2) {
    if (v1.start == v2.start && v1.task_id == v2.task_id) {
      return true;
    }
    if (v1.start == v2.start) {
      return v1.task_id > v2.task_id;
    }
    return v1.start > v2.start;  // NOLINT(readability-implicit-bool-conversion)
  }

  friend std::ostream &operator<<(std::ostream &stream, const STN_Vertex &v) {
    stream << v.task_id << "|" << (v.start ? "start" : "finish");

    return stream;
  }
};

struct STN_Edge {  // NOLINT(readability-identifier-naming)
  double weight;
  double getWeight() const { return weight; };

  friend std::ostream &operator<<(std::ostream &stream, const STN_Edge &e) {
    stream << e.getWeight();
    return stream;
  }
};

}  // namespace daisi::cpps

namespace std {

template <> struct hash<daisi::cpps::STN_Vertex> {
  std::size_t operator()(const daisi::cpps::STN_Vertex &v) const {
    using std::hash;
    using std::size_t;
    using std::string;

    assert(!v.task_id.empty());

    string repr = v.task_id + "|" + (v.start ? "start" : "finish");

    return hash<string>()(repr);
  }
};

}  // namespace std

#endif
