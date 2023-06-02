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

#include "cpps/negotiation/utils/directed_graph.h"

#include <limits>

#include "cpps/model/task.h"
#include "cpps/negotiation/utils/precedence_graph_components.h"
#include "cpps/negotiation/utils/simple_temporal_network_components.h"

namespace daisi::cpps {

template <typename Vertex, typename Edge>
std::vector<Vertex> &DirectedGraph<Vertex, Edge>::getVertices() {
  return vertices_;
}

template <typename Vertex, typename Edge>
std::unordered_map<Vertex, std::unordered_map<Vertex, std::vector<Edge>>>
    &DirectedGraph<Vertex, Edge>::getAdjacencyLists() {
  return adjacency_lists_;
}

template <typename Vertex, typename Edge>
size_t DirectedGraph<Vertex, Edge>::getNumberOfvertices() const {
  return vertices_.size();
}

template <typename Vertex, typename Edge>
void DirectedGraph<Vertex, Edge>::addVertex(const Vertex &vertex) {
  if (std::find(vertices_.begin(), vertices_.end(), vertex) == vertices_.end()) {
    vertices_.push_back(vertex);
  } else {
    throw std::logic_error("Vertex already inserted");
  }

  adjacency_lists_[vertex] = std::unordered_map<Vertex, std::vector<Edge>>();
}

template <typename Vertex, typename Edge>
void DirectedGraph<Vertex, Edge>::removeVertex(const Vertex &vertex) {
  if (std::find(vertices_.begin(), vertices_.end(), vertex) != vertices_.end()) {
    for (auto &it : adjacency_lists_) {
      it.second.erase(vertex);
    }

    adjacency_lists_.erase(vertex);

    vertices_.erase(std::remove(vertices_.begin(), vertices_.end(), vertex), vertices_.end());
  }
}

template <typename Vertex, typename Edge>
void DirectedGraph<Vertex, Edge>::addEdge(const Vertex &start, const Vertex &end, Edge edge) {
  bool start_exists = std::find(vertices_.begin(), vertices_.end(), start) != vertices_.end();
  bool end_exists = std::find(vertices_.begin(), vertices_.end(), end) != vertices_.end();

  if (!start_exists || !end_exists) {
    throw std::logic_error("");
  }

  adjacency_lists_[start][end].push_back(edge);
}

template <typename Vertex, typename Edge>
void DirectedGraph<Vertex, Edge>::removeEdges(const Vertex &start, const Vertex &end) {
  auto it = adjacency_lists_.find(start);
  if (it != adjacency_lists_.end()) {
    it->second.clear();
  }
}

template <typename Vertex, typename Edge>
std::vector<Edge> DirectedGraph<Vertex, Edge>::getEdges(const Vertex &start, const Vertex &end) {
  auto it = adjacency_lists_.find(start);
  if (it != adjacency_lists_.end()) {
    auto it2 = it->second.find(end);
    if (it2 != it->second.end()) {
      return it2->second;
    }
  }
  throw std::invalid_argument("Edge does not exist.");
}

template <typename Vertex, typename Edge>
double DirectedGraph<Vertex, Edge>::getEdgeValue(const Vertex &start, const Vertex &end) {
  auto edges = getEdges(start, end);
  return getEdgeValue(edges);
}

template <typename Vertex, typename Edge>
double DirectedGraph<Vertex, Edge>::getEdgeValue(const std::vector<Edge> &edges) {
  std::vector<double> weights;

  std::transform(edges.begin(), edges.end(), std::back_inserter(weights),
                 [](const Edge &e) -> double { return e.getWeight(); });

  bool all_negative =
      std::all_of(weights.cbegin(), weights.cend(), [](const double &w) { return w <= 0; });
  bool all_positive =
      std::all_of(weights.cbegin(), weights.cend(), [](const double &w) { return w >= 0; });

  if (all_negative) {
    return *std::min_element(weights.begin(), weights.end());
  }

  if (all_positive) {
    return *std::max_element(weights.begin(), weights.end());
  }

  throw std::logic_error("Edges either have to be all positive or all negative.");
}

template <typename Vertex, typename Edge>
bool DirectedGraph<Vertex, Edge>::hasEdge(const Vertex &start, const Vertex &end) {
  typename std::unordered_map<Vertex, std::unordered_map<Vertex, std::vector<Edge>>>::const_iterator
      it = adjacency_lists_.find(start);
  if (it != adjacency_lists_.end()) {
    typename std::unordered_map<Vertex, std::vector<Edge>>::const_iterator entry =
        it->second.find(end);
    if (entry != it->second.end()) {
      return !entry->second.empty();
    }
  }
  return false;
}

template <typename Vertex, typename Edge>
std::vector<std::vector<double>> DirectedGraph<Vertex, Edge>::floydWarshall() {
  size_t dim = vertices_.size();
  double inf = std::numeric_limits<double>::max();
  std::vector<std::vector<double>> dist(dim, std::vector<double>(dim, inf));

  for (size_t i = 0; i < vertices_.size(); i++) {
    for (size_t j = 0; j < vertices_.size(); j++) {
      const Vertex &v1 = vertices_[i];
      const Vertex &v2 = vertices_[j];
      if (hasEdge(v1, v2)) {
        dist[i][j] = getEdgeValue(v1, v2);
      }
    }
  }

  for (size_t i = 0; i < vertices_.size(); i++) {
    dist[i][i] = 0.0;
  }

  for (size_t k = 0; k < vertices_.size(); k++) {
    for (size_t i = 0; i < vertices_.size(); i++) {
      for (size_t j = 0; j < vertices_.size(); j++) {
        double alternative = dist[i][k] + dist[k][j];
        if (dist[i][j] > alternative) {
          dist[i][j] = alternative;
        }
      }
    }
  }

  return dist;
}

template <typename Vertex, typename Edge> bool DirectedGraph<Vertex, Edge>::isZero(double d) {
  return std::abs(d) < 0.001;
}

template class DirectedGraph<STN_Vertex, STN_Edge>;
template class DirectedGraph<Task, GEdge>;

}  // namespace daisi::cpps
