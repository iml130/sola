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

#include "datastructure/weighted_directed_graph.tpp"

#include <catch2/catch_test_macros.hpp>
#include <string>

struct TestVertex {
  explicit TestVertex(const std::string &name) : name_(name){};

  static TestVertex createOrigin() { return TestVertex("origin"); }

  friend bool operator==(const TestVertex &v1, const TestVertex &v2) {
    return v1.name_ == v2.name_;
  }

  std::string getName() const { return name_; }

  int test_member_ = 0;

private:
  std::string name_;
};

struct TestEdge {
  TestEdge(const std::string &name, const double &weight) : name_(name), weight_(weight){};

  friend bool operator==(const TestEdge &e1, const TestEdge &e2) { return e1.name_ == e2.name_; }

  std::string getName() const { return name_; }
  double getWeight() const { return weight_; }

private:
  std::string name_;
  double weight_;
};

namespace std {

template <> struct hash<TestVertex> {
  std::size_t operator()(const TestVertex &v) const {
    string repr = v.getName();
    return hash<string>()(repr);
  }
};

template <> struct hash<TestEdge> {
  std::size_t operator()(const TestEdge &e) const {
    string repr = e.getName();
    return hash<string>()(repr);
  }
};

}  // namespace std

using namespace daisi::datastructure;
using TestGraph = WeightedDirectedGraph<TestVertex, TestEdge>;

TEST_CASE("Simple graph with four vertices", "[weight matrix, floyd warshall]") {
  // using example from
  // https://www.gatevidyalay.com/floyd-warshall-algorithm-shortest-path-algorithm/

  TestGraph g1;

  TestVertex v1{"v1"};
  TestVertex v2{"v2"};
  TestVertex v3{"v3"};
  TestVertex v4{"v4"};

  g1.addVertex(v1);
  g1.addVertex(v2);
  g1.addVertex(v3);
  g1.addVertex(v4);

  g1.addEdge(v1, v2, TestEdge{"e12", 8});
  g1.addEdge(v1, v4, TestEdge{"e14", 1});
  g1.addEdge(v2, v3, TestEdge{"e23", 1});
  g1.addEdge(v4, v2, TestEdge{"e42", 2});
  g1.addEdge(v4, v3, TestEdge{"e43", 9});
  g1.addEdge(v3, v1, TestEdge{"e31", 4});

  auto matrix = g1.getWeightMatrix();
  REQUIRE(matrix.size() == 4);
  REQUIRE(matrix[0].size() == 4);

  for (int i = 0; i < 4; i++) {
    REQUIRE(matrix[i][i] == 0);
  }

  // v1->v4 = 4
  REQUIRE(matrix[0][3] == 1);

  // v1->v2 = 8
  REQUIRE(matrix[0][1] == 8);

  // v2->v3 = 1
  REQUIRE(matrix[1][2] == 1);

  // v4->v2 = 2
  REQUIRE(matrix[3][1] == 2);

  // v4->v3 = 9
  REQUIRE(matrix[3][2] == 9);

  // v3->v1 = 4
  REQUIRE(matrix[2][0] == 4);

  // infinity values
  double inf = std::numeric_limits<double>::infinity();
  REQUIRE(matrix[0][2] == inf);
  REQUIRE(matrix[1][0] == inf);
  REQUIRE(matrix[1][3] == inf);
  REQUIRE(matrix[2][1] == inf);
  REQUIRE(matrix[2][3] == inf);
  REQUIRE(matrix[3][0] == inf);

  auto dist = g1.floydWarshall();
  REQUIRE(dist[0][0] == 0);
  REQUIRE(dist[0][1] == 3);
  REQUIRE(dist[0][2] == 4);
  REQUIRE(dist[0][3] == 1);

  REQUIRE(dist[1][0] == 5);
  REQUIRE(dist[1][1] == 0);
  REQUIRE(dist[1][2] == 1);
  REQUIRE(dist[1][3] == 6);

  REQUIRE(dist[2][0] == 4);
  REQUIRE(dist[2][1] == 7);
  REQUIRE(dist[2][2] == 0);
  REQUIRE(dist[2][3] == 5);

  REQUIRE(dist[3][0] == 7);
  REQUIRE(dist[3][1] == 2);
  REQUIRE(dist[3][2] == 3);
  REQUIRE(dist[3][3] == 0);
}
