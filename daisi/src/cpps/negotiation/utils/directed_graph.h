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

#ifndef DAISI_NEGOTIATION_UTILS_DIRECTED_GRAPH_NS3_H_
#define DAISI_NEGOTIATION_UTILS_DIRECTED_GRAPH_NS3_H_

#include <algorithm>
#include <ostream>
#include <unordered_map>
#include <vector>

namespace daisi::cpps {

template <typename Vertex, typename Edge> class DirectedGraph {
public:
  DirectedGraph() = default;
  ~DirectedGraph() = default;

  void addVertex(const Vertex &vertex);
  void removeVertex(const Vertex &vertex);

  void addEdge(const Vertex &start, const Vertex &end, Edge edge);
  void removeEdges(const Vertex &start, const Vertex &end);
  std::vector<Edge> getEdges(const Vertex &start, const Vertex &end);
  double getEdgeValue(const Vertex &start, const Vertex &end);
  double getEdgeValue(const std::vector<Edge> &edges);
  bool hasEdge(const Vertex &start, const Vertex &end);

  std::vector<Vertex> &getVertices();
  std::unordered_map<Vertex, std::unordered_map<Vertex, std::vector<Edge>>> &getAdjacencyLists();
  size_t getNumberOfvertices() const;

  std::vector<std::vector<double>> floydWarshall();
  static bool isZero(double d);

  static double calcEdgeValue(std::vector<Edge> edges);

  friend std::ostream &operator<<(std::ostream &stream, const DirectedGraph &graph) {
    stream << "vertices: ";
    for (const Vertex &v : graph.getVertices()) {
      stream << v << ", ";
    }

    stream << std::endl << "edges: " << std::endl;
    for (const auto &start_map : graph.getAdjacencyLists()) {
      for (const auto &end_entry : start_map.second) {
        stream << start_map.first << " -> " << end_entry.first << " : " << end_entry.second
               << std::endl;
      }
    }

    return stream;
  }

protected:
  std::vector<Vertex> vertices_;

  std::unordered_map<Vertex, std::unordered_map<Vertex, std::vector<Edge>>> adjacency_lists_;
};

}  // namespace daisi::cpps

#endif
