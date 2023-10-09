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

#include "datastructure/simple_temporal_network.tpp"

#include <catch2/catch_test_macros.hpp>
#include <string>

using namespace daisi::datastructure;

struct TestVertex {
  explicit TestVertex(const std::string &name) : name_(name){};

  static TestVertex createOrigin() {
    auto vertex = TestVertex("origin");
    vertex.is_origin_ = true;
    return vertex;
  }

  friend bool operator==(const TestVertex &v1, const TestVertex &v2) {
    return v1.name_ == v2.name_;
  }

  std::string getName() const { return name_; }

  void setValue(const double &value) { value_ = value; }
  double getValue() const { return value_; }

  void setOrigin() { is_origin_ = true; }
  bool isOrigin() const { return is_origin_; }

private:
  std::string name_;
  double value_ = 0.0;
  bool is_origin_ = false;
};

struct TestEdge {
  explicit TestEdge(const bool all_positive) : all_positive_(all_positive){};

  void addWeight(double weight) {
    if (weight > 0 && all_positive_) {
      weights_.push_back(weight);
    } else if (weight < 0 && !all_positive_) {
      weights_.push_back(weight);
    } else {
      throw std::invalid_argument("Weight does not fit to the edge type");
    }
  }

  double getWeight() const {
    if (all_positive_) {
      return *std::max_element(weights_.begin(), weights_.end());
    } else {
      return *std::min_element(weights_.begin(), weights_.end());
    }
  }

  void removeLastWeight() { weights_.pop_back(); }

private:
  std::vector<double> weights_;
  bool all_positive_;
};

namespace std {

template <> struct hash<TestVertex> {
  std::size_t operator()(const TestVertex &v) const {
    string repr = v.getName();
    return hash<string>()(repr);
  }
};

}  // namespace std

using TestSTN = SimpleTemporalNetwork<TestVertex, TestEdge>;

TEST_CASE("Simple graph with four vertices", "[weight matrix, floyd warshall]") {
  // Example 1.1 from Dechter et al.

  TestSTN stn;

  REQUIRE(stn.getVertices().size() == 1);

  TestVertex &v0 = stn.getOrigin();
  REQUIRE(v0.isOrigin());
  TestVertex v1{"v1"};
  TestVertex v2{"v2"};
  TestVertex v3{"v3"};
  TestVertex v4{"v4"};

  v1.setValue(1);
  v2.setValue(2);
  v3.setValue(3);
  v4.setValue(4);

  stn.addVertex(v1);
  stn.addVertex(v2);
  stn.addVertex(v3);
  stn.addVertex(v4);

  REQUIRE(stn.getVertices().size() == 5);

  REQUIRE_NOTHROW(stn.addBinaryConstraint(stn.getOrigin(), v1, 10, 20));
  REQUIRE_NOTHROW(stn.addBinaryConstraint(v1, v2, 30, 40));
  REQUIRE_NOTHROW(stn.addBinaryConstraint(v3, v2, 10, 20));
  REQUIRE_NOTHROW(stn.addBinaryConstraint(v3, v4, 40, 50));
  REQUIRE_NOTHROW(stn.addBinaryConstraint(stn.getOrigin(), v4, 60, 70));

  REQUIRE(stn.hasEdge(stn.getOrigin(), v1));
  REQUIRE(stn.hasEdge(v1, v2));
  REQUIRE(stn.hasEdge(v3, v2));
  REQUIRE(stn.hasEdge(v3, v4));
  REQUIRE(stn.hasEdge(stn.getOrigin(), v4));

  REQUIRE(stn.getEdge(stn.getOrigin(), v1).getWeight() == 20);
  REQUIRE(stn.getEdge(v1, v2).getWeight() == 40);
  REQUIRE(stn.getEdge(v3, v2).getWeight() == 20);
  REQUIRE(stn.getEdge(v3, v4).getWeight() == 50);
  REQUIRE(stn.getEdge(stn.getOrigin(), v4).getWeight() == 70);

  REQUIRE(stn.getEdge(v1, stn.getOrigin()).getWeight() == -10);
  REQUIRE(stn.getEdge(v2, v1).getWeight() == -30);
  REQUIRE(stn.getEdge(v2, v3).getWeight() == -10);
  REQUIRE(stn.getEdge(v4, v3).getWeight() == -40);
  REQUIRE(stn.getEdge(v4, stn.getOrigin()).getWeight() == -60);

  REQUIRE(stn.solve());

  auto earliest_solution = stn.getEarliestSolution();

  auto it_v0 = std::find_if(earliest_solution.begin(), earliest_solution.end(),
                            [&](auto const &pair) { return std::get<0>(pair) == stn.getOrigin(); });
  REQUIRE(it_v0 != earliest_solution.end());
  REQUIRE(it_v0->second == 0);

  auto it_v1 = std::find_if(earliest_solution.begin(), earliest_solution.end(),
                            [&](auto const &pair) { return std::get<0>(pair) == v1; });
  REQUIRE(it_v1 != earliest_solution.end());
  REQUIRE(it_v1->second == 10);

  auto it_v2 = std::find_if(earliest_solution.begin(), earliest_solution.end(),
                            [&](auto const &pair) { return std::get<0>(pair) == v2; });
  REQUIRE(it_v2 != earliest_solution.end());
  REQUIRE(it_v2->second == 40);

  auto it_v3 = std::find_if(earliest_solution.begin(), earliest_solution.end(),
                            [&](auto const &pair) { return std::get<0>(pair) == v3; });
  REQUIRE(it_v3 != earliest_solution.end());
  REQUIRE(it_v3->second == 20);

  auto it_v4 = std::find_if(earliest_solution.begin(), earliest_solution.end(),
                            [&](auto const &pair) { return std::get<0>(pair) == v4; });
  REQUIRE(it_v4 != earliest_solution.end());
  REQUIRE(it_v4->second == 60);

  auto latest_solution = stn.getLatestSolution();

  auto it_v1l = std::find_if(latest_solution.begin(), latest_solution.end(),
                             [&](auto const &pair) { return std::get<0>(pair) == v1; });
  REQUIRE(it_v1l != latest_solution.end());
  REQUIRE(it_v1l->second == 20);

  auto it_v2l = std::find_if(latest_solution.begin(), latest_solution.end(),
                             [&](auto const &pair) { return std::get<0>(pair) == v2; });
  REQUIRE(it_v2l != latest_solution.end());
  REQUIRE(it_v2l->second == 50);

  auto it_v3l = std::find_if(latest_solution.begin(), latest_solution.end(),
                             [&](auto const &pair) { return std::get<0>(pair) == v3; });
  REQUIRE(it_v3l != latest_solution.end());
  REQUIRE(it_v3l->second == 30);

  auto it_v4l = std::find_if(latest_solution.begin(), latest_solution.end(),
                             [&](auto const &pair) { return std::get<0>(pair) == v4; });
  REQUIRE(it_v4l != latest_solution.end());
  REQUIRE(it_v4l->second == 70);
}
