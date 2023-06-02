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

#ifndef DAISI_DATASTRUCTURE_WEIGHTED_DIRECTED_GRAPH_IMPL_H_
#define DAISI_DATASTRUCTURE_WEIGHTED_DIRECTED_GRAPH_IMPL_H_

#include "directed_graph_impl.h"
#include "weighted_directed_graph.h"

namespace daisi::datastructure {

template <typename Vertex, typename Edge>
std::vector<std::vector<double>> WeightedDirectedGraph<Vertex, Edge>::getWeightMatrix() const {
  double inf = std::numeric_limits<double>::infinity();
  std::vector<std::vector<double>> matrix;

  for (size_t i = 0; i < this->vertices_.size(); i++) {
    matrix.push_back({});

    for (size_t k = 0; k < this->vertices_.size(); k++) {
      auto &opt_edge = this->adjacency_matrix_[i][k];

      if (opt_edge.has_value()) {
        matrix[i].push_back(opt_edge.value().getWeight());
      } else {
        if (i == k) {
          matrix[i].push_back(0.0);
        } else {
          matrix[i].push_back(inf);
        }
      }
    }
  }

  return matrix;
}

template <typename Vertex, typename Edge>
std::vector<std::vector<double>> WeightedDirectedGraph<Vertex, Edge>::floydWarshall() const {
  const int n = this->vertices_.size();

  auto dist = this->getWeightMatrix();

  for (size_t k = 0; k < n; k++) {
    for (size_t i = 0; i < n; i++) {
      for (size_t j = 0; j < n; j++) {
        double alternative = dist[i][k] + dist[k][j];
        if (dist[i][j] > alternative) {
          dist[i][j] = alternative;
        }
      }
    }
  }

  return dist;
}

}  // namespace daisi::datastructure

#endif
