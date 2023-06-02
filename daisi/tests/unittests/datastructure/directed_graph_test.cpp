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

#include <catch2/catch_test_macros.hpp>
#include <string>

#include "datastructure/directed_graph_impl.h"

struct TestVertex {
  explicit TestVertex(const std::string &name) : test_member_(0), name_(name){};

  static TestVertex createOrigin() { return TestVertex("origin"); }

  friend bool operator==(const TestVertex &v1, const TestVertex &v2) {
    return v1.name_ == v2.name_;
  }

  std::string getName() const { return name_; }

  int test_member_;

private:
  std::string name_;
};

struct TestEdge {
  explicit TestEdge(const std::string &name) : test_member_(0), name_(name){};

  friend bool operator==(const TestEdge &e1, const TestEdge &e2) { return e1.name_ == e2.name_; }

  std::string getName() const { return name_; }

  int test_member_;

private:
  std::string name_;
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
using TestGraph = DirectedGraph<TestVertex, TestEdge>;

TEST_CASE("Simple graph with three vertices", "[adding and removing vertices]") {
  TestGraph g1;

  TestVertex v1{"v1"};
  g1.addVertex(v1);
  REQUIRE(g1.getVertices().size() == 1);
  REQUIRE(g1.hasVertex(v1));

  g1.addVertex(v1);
  REQUIRE(g1.getVertices().size() == 1);
  REQUIRE(g1.hasVertex(v1));

  TestVertex v1_duplicate{"v1"};
  REQUIRE(v1 == v1_duplicate);
  g1.addVertex(v1_duplicate);
  REQUIRE(g1.getVertices().size() == 1);
  REQUIRE(g1.hasVertex(v1_duplicate));
  REQUIRE(g1.hasVertex(v1));

  TestVertex v2{"v2"};
  g1.addVertex(v2);
  auto vertices_tmp1 = g1.getVertices();
  REQUIRE(vertices_tmp1.size() == 2);
  REQUIRE(std::find(vertices_tmp1.begin(), vertices_tmp1.end(), v1) != vertices_tmp1.end());
  REQUIRE(std::find(vertices_tmp1.begin(), vertices_tmp1.end(), v2) != vertices_tmp1.end());
  REQUIRE(g1.hasVertex(v1));
  REQUIRE(g1.hasVertex(v2));

  TestVertex v3{"v3"};
  g1.addVertex(v3);
  auto vertices_tmp2 = g1.getVertices();
  REQUIRE(vertices_tmp2.size() == 3);
  REQUIRE(std::find(vertices_tmp2.begin(), vertices_tmp2.end(), v1) != vertices_tmp2.end());
  REQUIRE(std::find(vertices_tmp2.begin(), vertices_tmp2.end(), v2) != vertices_tmp2.end());
  REQUIRE(std::find(vertices_tmp2.begin(), vertices_tmp2.end(), v3) != vertices_tmp2.end());
  REQUIRE(g1.hasVertex(v1));
  REQUIRE(g1.hasVertex(v2));
  REQUIRE(g1.hasVertex(v3));

  g1.removeVertex(v2);
  auto vertices_tmp3 = g1.getVertices();
  REQUIRE(vertices_tmp3.size() == 2);
  REQUIRE(std::find(vertices_tmp3.begin(), vertices_tmp3.end(), v1) != vertices_tmp3.end());
  REQUIRE(std::find(vertices_tmp3.begin(), vertices_tmp3.end(), v2) == vertices_tmp3.end());
  REQUIRE(std::find(vertices_tmp3.begin(), vertices_tmp3.end(), v3) != vertices_tmp3.end());
  REQUIRE(g1.hasVertex(v1));
  REQUIRE(!g1.hasVertex(v2));
  REQUIRE(g1.hasVertex(v3));

  g1.removeVertex(v1);
  auto vertices_tmp4 = g1.getVertices();
  REQUIRE(vertices_tmp4.size() == 1);
  REQUIRE(std::find(vertices_tmp4.begin(), vertices_tmp4.end(), v1) == vertices_tmp4.end());
  REQUIRE(std::find(vertices_tmp4.begin(), vertices_tmp4.end(), v2) == vertices_tmp4.end());
  REQUIRE(std::find(vertices_tmp4.begin(), vertices_tmp4.end(), v3) != vertices_tmp4.end());
  REQUIRE(!g1.hasVertex(v1));
  REQUIRE(!g1.hasVertex(v2));
  REQUIRE(g1.hasVertex(v3));

  g1.removeVertex(v1);
  REQUIRE(g1.getVertices().size() == 1);
  REQUIRE(!g1.hasVertex(v1));

  g1.removeVertex(v3);
  REQUIRE(g1.getVertices().empty());
  REQUIRE(!g1.hasVertex(v3));
}

TEST_CASE("Simple graph with three vertices and edges", "[adding and removing edges]") {
  TestGraph g1;

  TestVertex v1{"v1"};
  TestVertex v2{"v2"};

  TestEdge e12{"e12"};
  TestEdge e21{"e22"};

  REQUIRE(!g1.hasEdge(v1, v2));
  REQUIRE(!g1.hasEdge(v2, v1));
  REQUIRE(!g1.hasEdge(v1, v1));
  REQUIRE(!g1.hasEdge(v2, v2));

  g1.addVertex(v1);
  g1.addVertex(v2);
  REQUIRE(!g1.hasEdge(v1, v2));
  REQUIRE(!g1.hasEdge(v2, v1));
  REQUIRE(!g1.hasEdge(v1, v1));
  REQUIRE(!g1.hasEdge(v2, v2));

  g1.addEdge(v1, v2, e12);
  REQUIRE(g1.hasEdge(v1, v2));
  REQUIRE(!g1.hasEdge(v2, v1));
  REQUIRE(!g1.hasEdge(v1, v1));
  REQUIRE(!g1.hasEdge(v2, v2));

  g1.addEdge(v1, v2, e12);
  REQUIRE(g1.hasEdge(v1, v2));
  REQUIRE(!g1.hasEdge(v2, v1));
  REQUIRE(!g1.hasEdge(v1, v1));
  REQUIRE(!g1.hasEdge(v2, v2));

  g1.addEdge(v2, v1, e21);
  REQUIRE(g1.hasEdge(v1, v2));
  REQUIRE(g1.hasEdge(v2, v1));
  REQUIRE(!g1.hasEdge(v1, v1));
  REQUIRE(!g1.hasEdge(v2, v2));

  g1.removeEdge(v2, v1);
  REQUIRE(g1.hasEdge(v1, v2));
  REQUIRE(!g1.hasEdge(v2, v1));
  REQUIRE(!g1.hasEdge(v1, v1));
  REQUIRE(!g1.hasEdge(v2, v2));

  // no throw if there is nothing
  REQUIRE_NOTHROW(g1.removeEdge(v2, v2));
  REQUIRE_NOTHROW(g1.removeEdge(v1, v1));

  // two edges between two vertices
  g1.addEdge(v2, v1, e21);
  TestEdge e21_2{"e21_2"};
  g1.addEdge(v2, v1, e21_2);

  REQUIRE(g1.hasEdge(v2, v1));

  // edge between with start=end
  TestEdge e11{"e11"};
  TestEdge e11_2{"e11_2"};
  g1.addEdge(v1, v1, e11);
  REQUIRE(g1.hasEdge(v1, v1));
  g1.addEdge(v1, v1, e11_2);
  REQUIRE(g1.hasEdge(v1, v1));
  REQUIRE(g1.hasEdge(v1, v1));
  g1.addEdge(v1, v1, e11);
  g1.removeEdge(v1, v1);
  REQUIRE(!g1.hasEdge(v1, v1));

  // adding/removing edges between non-existing vertices
  TestVertex v3{"v3"};
  REQUIRE_NOTHROW(g1.removeVertex(v3));
  REQUIRE_NOTHROW(g1.removeEdge(v3, v1));
  REQUIRE_NOTHROW(g1.removeEdge(v1, v3));
  REQUIRE(!g1.hasVertex(v3));

  TestEdge e31{"e31"};
  REQUIRE_THROWS(g1.addEdge(v1, v3, e31));
  REQUIRE_THROWS(g1.addEdge(v3, v1, e31));
  REQUIRE_THROWS(g1.addEdge(v3, v3, e31));
  REQUIRE(!g1.hasEdge(v3, v3));
  REQUIRE(!g1.hasVertex(v3));

  // removing vertices -> no edges
  g1.addVertex(v3);
  REQUIRE_NOTHROW(g1.addEdge(v3, v1, e31));
  REQUIRE(g1.hasVertex(v3));
  REQUIRE(!g1.hasEdge(v1, v3));
  REQUIRE(g1.hasEdge(v3, v1));

  g1.addEdge(v1, v2, e12);
  g1.addEdge(v2, v1, e21);
  REQUIRE(g1.hasEdge(v1, v2));
  REQUIRE(g1.hasEdge(v2, v1));
  REQUIRE(g1.hasEdge(v3, v1));

  g1.removeVertex(v2);
  REQUIRE(!g1.hasVertex(v2));
  REQUIRE(!g1.hasEdge(v1, v2));
  REQUIRE(!g1.hasEdge(v2, v1));

  REQUIRE(g1.hasEdge(v3, v1));
  g1.removeVertex(v1);
  REQUIRE(!g1.hasEdge(v3, v1));
}

TEST_CASE("Simple graph with two vertices and edges", "[getter]") {
  TestGraph g1;

  TestVertex v1{"v1"};
  TestVertex v2{"v2"};

  TestEdge e12{"e12"};
  TestEdge e21{"e22"};

  auto t1 = g1.getVertices();
  REQUIRE(t1.empty());

  g1.addVertex(v1);
  g1.addVertex(v2);
  g1.addEdge(v1, v2, e12);

  REQUIRE(!g1.hasEdge(v2, v1));
  REQUIRE_THROWS(g1.getEdge(v2, v1));

  REQUIRE(e12 == g1.getEdge(v1, v2));

  REQUIRE(g1.getVertices().size() == 2);
}

TEST_CASE("Simple graph with four vertices and edges", "[getter outgoing/incoming edges]") {
  TestGraph g1;

  TestVertex v1{"v1"};
  TestVertex v2{"v2"};

  TestEdge e12{"e12"};
  TestEdge e21{"e22"};

  auto t1 = g1.getVertices();
  REQUIRE(t1.empty());

  g1.addVertex(v1);
  g1.addVertex(v2);
  g1.addEdge(v1, v2, e12);

  REQUIRE(!g1.hasEdge(v2, v1));
  REQUIRE_THROWS(g1.getEdge(v2, v1));

  REQUIRE(e12 == g1.getEdge(v1, v2));

  REQUIRE(g1.getVertices().size() == 2);
}

TEST_CASE("Simple graph with four vertices", "[getter outgoing/incoming edges]") {
  TestGraph g1;

  TestVertex v1{"v1"};
  TestVertex v2{"v2"};
  TestVertex v3{"v3"};
  TestVertex v4{"v4"};

  TestEdge e12{"e12"};
  TestEdge e14{"e14"};
  TestEdge e41{"e41"};
  TestEdge e23{"e23"};
  TestEdge e24{"e24"};

  g1.addVertex(v1);
  g1.addVertex(v2);
  g1.addVertex(v3);
  g1.addVertex(v4);

  g1.addEdge(v1, v2, e12);
  g1.addEdge(v1, v4, e14);
  g1.addEdge(v4, v1, e41);
  g1.addEdge(v2, v3, e23);
  g1.addEdge(v2, v4, e24);

  REQUIRE(g1.getVertices().size() == 4);
  REQUIRE(g1.hasVertex(v1));
  REQUIRE(g1.hasVertex(v2));
  REQUIRE(g1.hasVertex(v3));
  REQUIRE(g1.hasVertex(v4));

  REQUIRE(g1.getEdge(v1, v2) == e12);
  REQUIRE(g1.getEdge(v1, v4) == e14);
  REQUIRE(g1.getEdge(v4, v1) == e41);
  REQUIRE(g1.getEdge(v2, v3) == e23);
  REQUIRE(g1.getEdge(v2, v4) == e24);

  auto outgoing_v1 = g1.getOutgoingEdges(v1);
  auto outgoing_v2 = g1.getOutgoingEdges(v2);
  auto outgoing_v3 = g1.getOutgoingEdges(v3);
  auto outgoing_v4 = g1.getOutgoingEdges(v4);

  auto incoming_v1 = g1.getIncomingEdges(v1);
  auto incoming_v2 = g1.getIncomingEdges(v2);
  auto incoming_v3 = g1.getIncomingEdges(v3);
  auto incoming_v4 = g1.getIncomingEdges(v4);

  REQUIRE(outgoing_v1.size() == 2);
  REQUIRE((std::find_if(outgoing_v1.begin(), outgoing_v1.end(), [&](auto const &pair) {
             return std::get<0>(pair) == v2;
           }) != outgoing_v1.end()));
  REQUIRE((std::find_if(outgoing_v1.begin(), outgoing_v1.end(), [&](auto const &pair) {
             return std::get<0>(pair) == v4;
           }) != outgoing_v1.end()));
  REQUIRE(incoming_v1.size() == 1);
  REQUIRE(incoming_v1[0].first == v4);
  REQUIRE(incoming_v1[0].second == e41);

  REQUIRE(outgoing_v2.size() == 2);
  REQUIRE((std::find_if(outgoing_v2.begin(), outgoing_v2.end(), [&](auto const &pair) {
             return std::get<0>(pair) == v3;
           }) != outgoing_v2.end()));
  REQUIRE((std::find_if(outgoing_v2.begin(), outgoing_v2.end(), [&](auto const &pair) {
             return std::get<0>(pair) == v4;
           }) != outgoing_v2.end()));
  REQUIRE(incoming_v2.size() == 1);
  REQUIRE(incoming_v2[0].first == v1);
  REQUIRE(incoming_v2[0].second == e12);

  REQUIRE(incoming_v3.size() == 1);
  REQUIRE(outgoing_v3.empty());

  REQUIRE(incoming_v4.size() == 2);
  REQUIRE(outgoing_v4.size() == 1);
}
