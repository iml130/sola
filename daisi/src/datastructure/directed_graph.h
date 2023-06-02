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

#ifndef DAISI_DATASTRUCTURE_DIRECTED_GRAPH_H_
#define DAISI_DATASTRUCTURE_DIRECTED_GRAPH_H_

#include <optional>
#include <unordered_map>
#include <utility>
#include <vector>

namespace daisi::datastructure {

template <typename Vertex, typename Edge> class DirectedGraph {
public:
  DirectedGraph() = default;
  DirectedGraph(const DirectedGraph &other);

  void addVertex(const Vertex &vertex);
  void removeVertex(const Vertex &vertex);
  bool hasVertex(const Vertex &vertex) const;

  void addEdge(const Vertex &start, const Vertex &end, const Edge &edge);
  void removeEdge(const Vertex &start, const Vertex &end);
  bool hasEdge(const Vertex &start, const Vertex &end) const;

  const std::vector<Vertex> &getVertices() const;
  Edge &getEdge(const Vertex &start, const Vertex &end);

  std::vector<std::pair<Vertex, Edge>> getOutgoingEdges(const Vertex &start) const;
  std::vector<std::pair<Vertex, Edge>> getIncomingEdges(const Vertex &end) const;

protected:
  std::vector<Vertex> vertices_;
  std::vector<std::vector<std::optional<Edge>>> adjacency_matrix_;
};

}  // namespace daisi::datastructure

#endif
