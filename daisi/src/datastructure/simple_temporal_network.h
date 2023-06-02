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

#ifndef DAISI_DATASTRUCTURE_SIMPLE_TEMPORAL_NETWORK_H_
#define DAISI_DATASTRUCTURE_SIMPLE_TEMPORAL_NETWORK_H_

#include <optional>

#include "weighted_directed_graph.h"

namespace daisi::datastructure {

template <typename Vertex, typename Edge>
class SimpleTemporalNetwork : public WeightedDirectedGraph<Vertex, Edge> {
public:
  SimpleTemporalNetwork();

  void addBinaryConstraint(const Vertex &start, const Vertex &end,
                           const std::optional<double> &lower_bound,
                           const std::optional<double> &upper_bound);

  void addUnaryConstraint(const Vertex &vertex, const std::optional<double> &lower_bound,
                          const std::optional<double> &upper_bound);

  void updateLastBinaryConstraint(const Vertex &start, const Vertex &end,
                                  const std::optional<double> &lower_bound,
                                  const std::optional<double> &upper_bound);

  void removeVertex(const Vertex &vertex);

  Vertex &getOrigin();
  std::vector<std::pair<Vertex, double>> getEarliestSolution();
  std::vector<std::pair<Vertex, double>> getLatestSolution();

  virtual bool solve();

protected:
  std::vector<std::vector<double>> d_graph_;
};

}  // namespace daisi::datastructure

#endif
