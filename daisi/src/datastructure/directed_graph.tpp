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

#ifndef DAISI_DATASTRUCTURE_DIRECTED_GRAPH_IMPL_H_
#define DAISI_DATASTRUCTURE_DIRECTED_GRAPH_IMPL_H_

#include <algorithm>
#include <stdexcept>

#include "directed_graph.h"

namespace daisi::datastructure {

template <typename Vertex, typename Edge>
DirectedGraph<Vertex, Edge>::DirectedGraph(const DirectedGraph &other)
    : vertices_(), adjacency_matrix_(other.adjacency_matrix_) {
  for (const auto &vertex : other.vertices_) {
    Vertex new_vertex(vertex);
    vertices_.push_back(new_vertex);
  }

  for (size_t i = 0; i < adjacency_matrix_.size(); ++i) {
    for (size_t j = 0; j < adjacency_matrix_[i].size(); ++j) {
      if (other.adjacency_matrix_[i][j].has_value()) {
        Edge new_edge(*other.adjacency_matrix_[i][j]);
        adjacency_matrix_[i][j] = std::make_optional(new_edge);
      }
    }
  }
}

template <typename Vertex, typename Edge>
void DirectedGraph<Vertex, Edge>::addVertex(const Vertex &vertex) {
  if (!hasVertex(vertex)) {
    vertices_.push_back(vertex);

    const int n = vertices_.size();
    adjacency_matrix_.push_back({});

    for (int i = 0; i < n; i++) {
      adjacency_matrix_[n - 1].push_back(std::nullopt);
    }

    for (int i = 0; i < n - 1; i++) {
      adjacency_matrix_[i].push_back(std::nullopt);
    }
  }
}

template <typename Vertex, typename Edge>
void DirectedGraph<Vertex, Edge>::removeVertex(const Vertex &vertex) {
  auto it = std::find(vertices_.begin(), vertices_.end(), vertex);
  if (it != vertices_.end()) {
    const int index = it - vertices_.begin();

    // removing from vertices
    vertices_.erase(it);

    // removing outgoing
    auto it_outgoing = adjacency_matrix_.begin() + index;
    adjacency_matrix_.erase(it_outgoing);

    // removing incoming
    for (auto &others_outgoing : adjacency_matrix_) {
      auto it_other = others_outgoing.begin() + index;
      others_outgoing.erase(it_other + index);
    }
  }
}

template <typename Vertex, typename Edge>
bool DirectedGraph<Vertex, Edge>::hasVertex(const Vertex &vertex) const {
  auto it = std::find(vertices_.begin(), vertices_.end(), vertex);
  return it != vertices_.end();
}

template <typename Vertex, typename Edge>
void DirectedGraph<Vertex, Edge>::addEdge(const Vertex &start, const Vertex &end,
                                          const Edge &edge) {
  if (!hasVertex(start)) {
    throw std::logic_error("Start Vertex does not exist");
  }

  if (!hasVertex(end)) {
    throw std::logic_error("End Vertex does not exist");
  }

  const int start_index = std::find(vertices_.begin(), vertices_.end(), start) - vertices_.begin();
  const int end_index = std::find(vertices_.begin(), vertices_.end(), end) - vertices_.begin();

  adjacency_matrix_[start_index][end_index] = edge;
}

template <typename Vertex, typename Edge>
void DirectedGraph<Vertex, Edge>::removeEdge(const Vertex &start, const Vertex &end) {
  if (hasVertex(start) && hasVertex(end)) {
    const int start_index =
        std::find(vertices_.begin(), vertices_.end(), start) - vertices_.begin();
    const int end_index = std::find(vertices_.begin(), vertices_.end(), end) - vertices_.begin();

    adjacency_matrix_[start_index][end_index] = std::nullopt;
  }
}

template <typename Vertex, typename Edge>
bool DirectedGraph<Vertex, Edge>::hasEdge(const Vertex &start, const Vertex &end) const {
  if (!hasVertex(start) || !hasVertex(end)) {
    return false;
  }

  const int start_index = std::find(vertices_.begin(), vertices_.end(), start) - vertices_.begin();
  const int end_index = std::find(vertices_.begin(), vertices_.end(), end) - vertices_.begin();

  return adjacency_matrix_[start_index][end_index].has_value();
}

template <typename Vertex, typename Edge>
const std::vector<Vertex> &DirectedGraph<Vertex, Edge>::getVertices() const {
  return vertices_;
}

template <typename Vertex, typename Edge>
Edge &DirectedGraph<Vertex, Edge>::getEdge(const Vertex &start, const Vertex &end) {
  const auto it_start = std::find(vertices_.begin(), vertices_.end(), start);
  const auto it_end = std::find(vertices_.begin(), vertices_.end(), end);

  if (it_start == vertices_.end()) {
    throw std::logic_error("Start Vertex does not exist");
  }

  if (it_end == vertices_.end()) {
    throw std::logic_error("End Vertex does not exist");
  }

  return adjacency_matrix_[it_start - vertices_.begin()][it_end - vertices_.begin()].value();
}

template <typename Vertex, typename Edge>
std::vector<std::pair<Vertex, Edge>> DirectedGraph<Vertex, Edge>::getOutgoingEdges(
    const Vertex &start) const {
  const auto it_start = std::find(vertices_.begin(), vertices_.end(), start);
  if (it_start == vertices_.end()) {
    throw std::logic_error("Start Vertex does not exist");
  }

  const int start_index = it_start - vertices_.begin();

  std::vector<std::pair<Vertex, Edge>> outgoing;

  for (size_t i = 0; i < vertices_.size(); i++) {
    auto opt_edge = adjacency_matrix_[start_index][i];

    if (opt_edge.has_value()) {
      outgoing.push_back({vertices_[i], opt_edge.value()});
    }
  }

  return outgoing;
}

template <typename Vertex, typename Edge>
std::vector<std::pair<Vertex, Edge>> DirectedGraph<Vertex, Edge>::getIncomingEdges(
    const Vertex &end) const {
  const auto it_end = std::find(vertices_.begin(), vertices_.end(), end);
  if (it_end == vertices_.end()) {
    throw std::logic_error("End Vertex does not exist");
  }

  const int end_index = it_end - vertices_.begin();

  std::vector<std::pair<Vertex, Edge>> incoming;

  for (size_t i = 0; i < vertices_.size(); i++) {
    auto opt_edge = adjacency_matrix_[i][end_index];

    if (opt_edge.has_value()) {
      incoming.push_back({vertices_[i], opt_edge.value()});
    }
  }

  return incoming;
}

}  // namespace daisi::datastructure

#endif
