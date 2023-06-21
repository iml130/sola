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

#ifndef DAISI_DATASTRUCTURE_SIMPLE_TEMPORAL_NETWORK_IMPL_H_
#define DAISI_DATASTRUCTURE_SIMPLE_TEMPORAL_NETWORK_IMPL_H_

#include "simple_temporal_network.h"

namespace daisi::datastructure {
template <typename Vertex, typename Edge>
SimpleTemporalNetwork<Vertex, Edge>::SimpleTemporalNetwork() {
  this->addVertex(Vertex::createOrigin());
}

template <typename Vertex, typename Edge>
void SimpleTemporalNetwork<Vertex, Edge>::removeVertex(const Vertex &vertex) {
  if (vertex == this->vertices_[0]) {
    throw std::invalid_argument("Cannot remove origin from Simple Temporal Network");
  }

  WeightedDirectedGraph<Vertex, Edge>::removeVertex(vertex);
}

template <typename Vertex, typename Edge> Vertex &SimpleTemporalNetwork<Vertex, Edge>::getOrigin() {
  return this->vertices_.front();
}

template <typename Vertex, typename Edge>
void SimpleTemporalNetwork<Vertex, Edge>::addBinaryConstraint(
    const Vertex &start, const Vertex &end, const std::optional<double> &lower_bound,
    const std::optional<double> &upper_bound) {
  if (!this->hasVertex(start) || !this->hasVertex(end)) {
    throw std::invalid_argument("Bound cannot be added for non-existent vertices");
  }

  if (upper_bound.has_value()) {
    if (this->hasEdge(start, end)) {
      this->getEdge(start, end).addWeight(upper_bound.value());
    } else {
      Edge edge{true};
      edge.addWeight(upper_bound.value());
      this->addEdge(start, end, edge);
    }
  }

  if (lower_bound.has_value()) {
    if (this->hasEdge(end, start)) {
      this->getEdge(end, start).addWeight(-lower_bound.value());
    } else {
      Edge edge{false};
      edge.addWeight(-lower_bound.value());
      this->addEdge(end, start, edge);
    }
  }
}

template <typename Vertex, typename Edge>
void SimpleTemporalNetwork<Vertex, Edge>::updateLastBinaryConstraint(
    const Vertex &start, const Vertex &end, const std::optional<double> &lower_bound,
    const std::optional<double> &upper_bound) {
  if (!this->hasVertex(start) || !this->hasVertex(end)) {
    throw std::invalid_argument("Bound cannot be updated for non-existent vertices");
  }

  if (upper_bound.has_value()) {
    if (this->hasEdge(start, end)) {
      this->getEdge(start, end).removeLastWeight();
      this->getEdge(start, end).addWeight(upper_bound.value());
    } else {
      throw std::invalid_argument("Bound cannot be updated if no edge exists");
    }
  }

  if (lower_bound.has_value()) {
    if (this->hasEdge(end, start)) {
      this->getEdge(end, start).removeLastWeight();
      this->getEdge(end, start).addWeight(-lower_bound.value());
    } else {
      throw std::invalid_argument("Bound cannot be updated if no edge exists");
    }
  }
}

template <typename Vertex, typename Edge>
void SimpleTemporalNetwork<Vertex, Edge>::addUnaryConstraint(
    const Vertex &vertex, const std::optional<double> &lower_bound,
    const std::optional<double> &upper_bound) {
  addBinaryConstraint(getOrigin(), vertex, lower_bound, upper_bound);
}

template <typename Vertex, typename Edge> bool SimpleTemporalNetwork<Vertex, Edge>::solve() {
  d_graph_ = this->floydWarshall();

  for (size_t i = 0; i < this->vertices_.size(); i++) {
    if (d_graph_[i][i] != 0) {
      return false;
    }
  }

  return true;
}

template <typename Vertex, typename Edge>
std::vector<std::pair<Vertex, double>> SimpleTemporalNetwork<Vertex, Edge>::getEarliestSolution() {
  std::vector<std::pair<Vertex, double>> solution;
  for (size_t i = 0; i < this->vertices_.size(); i++) {
    solution.push_back({this->vertices_[i], -d_graph_[i][0]});
  }
  return solution;
}

template <typename Vertex, typename Edge>
std::vector<std::pair<Vertex, double>> SimpleTemporalNetwork<Vertex, Edge>::getLatestSolution() {
  std::vector<std::pair<Vertex, double>> solution;
  for (size_t i = 0; i < this->vertices_.size(); i++) {
    solution.push_back({this->vertices_[i], d_graph_[0][i]});
  }
  return solution;
}

}  // namespace daisi::datastructure

#endif
