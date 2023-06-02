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

#include "cpps/amr/amr_mobility_helper.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <memory>

using namespace daisi::cpps;

TEST_CASE("EstimamteTimeConsumption single function execution (neglect rotation cost, neglect "
          "payload weight, stop at functionality endpoint)",
          "[mobility_helper]") {
  // preparation
  auto topology = Topology({15, 15, 15});
  daisi::util::Pose start_pose({0, 0}, 0);
  daisi::util::Pose impossible_start_pose1({-1, 0}, 0);
  daisi::util::Pose impossible_start_pose2({20, 0}, 0);

  auto kinematics1 = AmrKinematics(1, 0, 1, 1);
  auto kinematics2 = AmrKinematics(10, 0, 10, 10);
  auto kinematics3 = AmrKinematics(10, 0, 2.5, 5);

  auto properties = AmrProperties();
  AmrProperties impossible_properties("manufacturer", "model_name", 0, "device_type",
                                      "friendly_name", {});
  auto physical_properties = AmrPhysicalProperties();
  auto load_handling1 = AmrLoadHandlingUnit(1, 1, daisi::cpps::mrta::model::Ability());
  auto load_handling2 = AmrLoadHandlingUnit(10, 10, daisi::cpps::mrta::model::Ability());
  auto load_handling3 = AmrLoadHandlingUnit(2.5, 5, daisi::cpps::mrta::model::Ability());

  auto move_to11 = MoveTo({10, 0});
  auto move_to12 = MoveTo({0, 10});
  auto move_to21 = MoveTo({1, 0});
  auto move_to22 = MoveTo({0, 1});
  auto move_to31 = MoveTo({3, 4});
  auto move_to32 = MoveTo({4, 3});

  auto unload11 = Unload(start_pose.position);

  auto load11 = Load(start_pose.position);

  auto navigate11 = Navigate({{10, 0}, {0, 0}});
  auto navigate12 = Navigate({{0, 10}, {0, 0}});
  auto navigate21 = Navigate({{1, 0}, {0, 0}});
  auto navigate22 = Navigate({{0, 1}, {0, 0}});
  auto navigate31 = Navigate({{3, 4}, {0, 0}});
  auto navigate32 = Navigate({{4, 3}, {0, 0}});
  auto navigate41 = Navigate({{0, 0}, {10, 0}});
  auto navigate42 = Navigate({{0, 0}, {0, 10}});
  auto navigate51 = Navigate({{0, 0}, {1, 0}});
  auto navigate52 = Navigate({{0, 0}, {0, 1}});
  auto navigate61 = Navigate({{0, 0}, {3, 4}});
  auto navigate62 = Navigate({{0, 0}, {4, 3}});

  std::vector<FunctionalityVariant> functionalities;

  AmrDescription impossible_description =
      AmrDescription(1, kinematics1, impossible_properties, physical_properties, load_handling1);

  functionalities = {move_to11};
  REQUIRE_THROWS_AS(AmrMobilityHelper::estimateDuration(start_pose, functionalities,
                                                        impossible_description, topology),
                    std::invalid_argument);
  functionalities = {unload11};
  REQUIRE_THROWS_AS(AmrMobilityHelper::estimateDuration(start_pose, functionalities,
                                                        impossible_description, topology),
                    std::invalid_argument);
  functionalities = {load11};
  REQUIRE_THROWS_AS(AmrMobilityHelper::estimateDuration(start_pose, functionalities,
                                                        impossible_description, topology),
                    std::invalid_argument);
  functionalities = {navigate11};
  REQUIRE_THROWS_AS(AmrMobilityHelper::estimateDuration(start_pose, functionalities,
                                                        impossible_description, topology),
                    std::invalid_argument);

  AmrDescription description1 =
      AmrDescription(1, kinematics1, properties, physical_properties, load_handling1);
  functionalities = {MoveTo({1, 0})};
  REQUIRE_THROWS_AS(AmrMobilityHelper::estimateDuration(impossible_start_pose1, functionalities,
                                                        description1, topology),
                    std::invalid_argument);
  REQUIRE_THROWS_AS(AmrMobilityHelper::estimateDuration(impossible_start_pose2, functionalities,
                                                        description1, topology),
                    std::invalid_argument);
  functionalities = {MoveTo({-1, 0})};
  REQUIRE_THROWS_AS(
      AmrMobilityHelper::estimateDuration(start_pose, functionalities, description1, topology),
      std::invalid_argument);
  functionalities = {MoveTo({20, 0})};
  REQUIRE_THROWS_AS(
      AmrMobilityHelper::estimateDuration(start_pose, functionalities, description1, topology),
      std::invalid_argument);
  functionalities = {Unload({-1, 0})};
  REQUIRE_THROWS_AS(
      AmrMobilityHelper::estimateDuration(start_pose, functionalities, description1, topology),
      std::invalid_argument);
  functionalities = {Unload({20, 0})};
  REQUIRE_THROWS_AS(
      AmrMobilityHelper::estimateDuration(start_pose, functionalities, description1, topology),
      std::invalid_argument);
  functionalities = {Load({-1, 0})};
  REQUIRE_THROWS_AS(
      AmrMobilityHelper::estimateDuration(start_pose, functionalities, description1, topology),
      std::invalid_argument);
  functionalities = {Load({20, 0})};
  REQUIRE_THROWS_AS(
      AmrMobilityHelper::estimateDuration(start_pose, functionalities, description1, topology),
      std::invalid_argument);
  functionalities = {Navigate({{-1, 0}})};
  REQUIRE_THROWS_AS(
      AmrMobilityHelper::estimateDuration(start_pose, functionalities, description1, topology),
      std::invalid_argument);
  functionalities = {Navigate({{0, 20}})};
  REQUIRE_THROWS_AS(
      AmrMobilityHelper::estimateDuration(start_pose, functionalities, description1, topology),
      std::invalid_argument);

  AmrDescription description2 =
      AmrDescription(1, kinematics1, properties, physical_properties, load_handling1);
  // kinematics1, load_handling1, MoveTo
  functionalities = {move_to11};
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionalities, description2,
                                              topology) == Catch::Approx(11));
  functionalities = {move_to12};
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionalities, description2,
                                              topology) == Catch::Approx(11));
  functionalities = {move_to21};
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionalities, description2,
                                              topology) == Catch::Approx(2));
  functionalities = {move_to22};
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionalities, description2,
                                              topology) == Catch::Approx(2));
  functionalities = {move_to31};
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionalities, description2,
                                              topology) == Catch::Approx(6));
  functionalities = {move_to32};
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionalities, description2,
                                              topology) == Catch::Approx(6));
  // kinematics1, load_handling1, Unload
  functionalities = {unload11};
  REQUIRE(
      AmrMobilityHelper::estimateDuration(start_pose, functionalities, description2, topology) ==
      Catch::Approx(description2.getLoadHandling().getUnloadTime()));
  // kinematics1, load_handling1, Load
  functionalities = {load11};
  REQUIRE(
      AmrMobilityHelper::estimateDuration(start_pose, functionalities, description2, topology) ==
      Catch::Approx(description2.getLoadHandling().getLoadTime()));
  // kinematics1, load_handling1, Navigate
  functionalities = {navigate11};
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionalities, description2,
                                              topology) == Catch::Approx(21));
  functionalities = {navigate12};
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionalities, description2,
                                              topology) == Catch::Approx(21));
  functionalities = {navigate21};
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionalities, description2,
                                              topology) == Catch::Approx(3));
  functionalities = {navigate22};
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionalities, description2,
                                              topology) == Catch::Approx(3));
  functionalities = {navigate31};
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionalities, description2,
                                              topology) == Catch::Approx(11));
  functionalities = {navigate32};
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionalities, description2,
                                              topology) == Catch::Approx(11));
  functionalities = {navigate41};
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionalities, description2,
                                              topology) == Catch::Approx(11));
  functionalities = {navigate42};
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionalities, description2,
                                              topology) == Catch::Approx(11));
  functionalities = {navigate51};
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionalities, description2,
                                              topology) == Catch::Approx(2));
  functionalities = {navigate52};
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionalities, description2,
                                              topology) == Catch::Approx(2));
  functionalities = {navigate61};
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionalities, description2,
                                              topology) == Catch::Approx(6));
  functionalities = {navigate62};
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionalities, description2,
                                              topology) == Catch::Approx(6));

  AmrDescription description3 =
      AmrDescription(1, kinematics1, properties, physical_properties, load_handling2);
  // kinematics1, load_handling2, MoveTo
  functionalities = {move_to11};
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionalities, description3,
                                              topology) == Catch::Approx(11));
  functionalities = {move_to12};
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionalities, description3,
                                              topology) == Catch::Approx(11));
  functionalities = {move_to21};
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionalities, description3,
                                              topology) == Catch::Approx(2));
  functionalities = {move_to22};
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionalities, description3,
                                              topology) == Catch::Approx(2));
  functionalities = {move_to31};
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionalities, description3,
                                              topology) == Catch::Approx(6));
  functionalities = {move_to32};
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionalities, description3,
                                              topology) == Catch::Approx(6));
  // kinematics1, load_handling2, Unload
  functionalities = {unload11};
  REQUIRE(
      AmrMobilityHelper::estimateDuration(start_pose, functionalities, description3, topology) ==
      Catch::Approx(description3.getLoadHandling().getUnloadTime()));
  // kinematics1, load_handling2, Load
  functionalities = {load11};
  REQUIRE(
      AmrMobilityHelper::estimateDuration(start_pose, functionalities, description3, topology) ==
      Catch::Approx(description3.getLoadHandling().getLoadTime()));
  // kinematics1, load_handling2, Navigate
  functionalities = {navigate11};
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionalities, description3,
                                              topology) == Catch::Approx(21));
  functionalities = {navigate12};
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionalities, description3,
                                              topology) == Catch::Approx(21));
  functionalities = {navigate21};
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionalities, description3,
                                              topology) == Catch::Approx(3));
  functionalities = {navigate22};
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionalities, description3,
                                              topology) == Catch::Approx(3));
  functionalities = {navigate31};
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionalities, description3,
                                              topology) == Catch::Approx(11));
  functionalities = {navigate32};
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionalities, description3,
                                              topology) == Catch::Approx(11));
  functionalities = {navigate41};
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionalities, description3,
                                              topology) == Catch::Approx(11));
  functionalities = {navigate42};
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionalities, description3,
                                              topology) == Catch::Approx(11));
  functionalities = {navigate51};
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionalities, description3,
                                              topology) == Catch::Approx(2));
  functionalities = {navigate52};
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionalities, description3,
                                              topology) == Catch::Approx(2));
  functionalities = {navigate61};
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionalities, description3,
                                              topology) == Catch::Approx(6));
  functionalities = {navigate62};
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionalities, description3,
                                              topology) == Catch::Approx(6));

  AmrDescription description4 =
      AmrDescription(1, kinematics1, properties, physical_properties, load_handling3);
  // kinematics1, load_handling3, MoveTo
  functionalities = {move_to11};
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionalities, description4,
                                              topology) == Catch::Approx(11));
  functionalities = {move_to12};
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionalities, description4,
                                              topology) == Catch::Approx(11));
  functionalities = {move_to21};
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionalities, description4,
                                              topology) == Catch::Approx(2));
  functionalities = {move_to22};
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionalities, description4,
                                              topology) == Catch::Approx(2));
  functionalities = {move_to31};
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionalities, description4,
                                              topology) == Catch::Approx(6));
  functionalities = {move_to32};
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionalities, description4,
                                              topology) == Catch::Approx(6));
  // kinematics1, load_handling3, Unload
  functionalities = {unload11};
  REQUIRE(
      AmrMobilityHelper::estimateDuration(start_pose, functionalities, description4, topology) ==
      Catch::Approx(description4.getLoadHandling().getUnloadTime()));
  // kinematics1, load_handling3, Load//
  functionalities = {load11};
  REQUIRE(
      AmrMobilityHelper::estimateDuration(start_pose, functionalities, description4, topology) ==
      Catch::Approx(description4.getLoadHandling().getLoadTime()));
  // kinematics1, load_handling3, Navigate
  functionalities = {navigate11};
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionalities, description4,
                                              topology) == Catch::Approx(21));
  functionalities = {navigate12};
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionalities, description4,
                                              topology) == Catch::Approx(21));
  functionalities = {navigate21};
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionalities, description4,
                                              topology) == Catch::Approx(3));
  functionalities = {navigate22};
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionalities, description4,
                                              topology) == Catch::Approx(3));
  functionalities = {navigate31};
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionalities, description4,
                                              topology) == Catch::Approx(11));
  functionalities = {navigate32};
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionalities, description4,
                                              topology) == Catch::Approx(11));
  functionalities = {navigate41};
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionalities, description4,
                                              topology) == Catch::Approx(11));
  functionalities = {navigate42};
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionalities, description4,
                                              topology) == Catch::Approx(11));
  functionalities = {navigate51};
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionalities, description4,
                                              topology) == Catch::Approx(2));
  functionalities = {navigate52};
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionalities, description4,
                                              topology) == Catch::Approx(2));
  functionalities = {navigate61};
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionalities, description4,
                                              topology) == Catch::Approx(6));
  functionalities = {navigate62};
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionalities, description4,
                                              topology) == Catch::Approx(6));

  AmrDescription description5 =
      AmrDescription(1, kinematics2, properties, physical_properties, load_handling1);
  // kinematics2, load_handling1, MoveTo
  functionalities = {move_to11};
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionalities, description5,
                                              topology) == Catch::Approx(2));
  functionalities = {move_to12};
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionalities, description5,
                                              topology) == Catch::Approx(2));
  functionalities = {move_to21};
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionalities, description5,
                                              topology) == Catch::Approx(0.632455532));
  functionalities = {move_to22};
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionalities, description5,
                                              topology) == Catch::Approx(0.632455532));
  functionalities = {move_to31};
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionalities, description5,
                                              topology) == Catch::Approx(1.414213562));
  functionalities = {move_to32};
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionalities, description5,
                                              topology) == Catch::Approx(1.414213562));
  // kinematics2, load_handling1, Unload
  functionalities = {unload11};
  REQUIRE(
      AmrMobilityHelper::estimateDuration(start_pose, functionalities, description5, topology) ==
      Catch::Approx(description5.getLoadHandling().getUnloadTime()));
  // kinematics2, load_handling1, Load
  functionalities = {load11};
  REQUIRE(
      AmrMobilityHelper::estimateDuration(start_pose, functionalities, description5, topology) ==
      Catch::Approx(description5.getLoadHandling().getLoadTime()));
  // kinematics2, load_handling1, Navigate
  functionalities = {navigate11};
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionalities, description5,
                                              topology) == Catch::Approx(3));
  functionalities = {navigate12};
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionalities, description5,
                                              topology) == Catch::Approx(3));
  functionalities = {navigate21};
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionalities, description5,
                                              topology) == Catch::Approx(0.894427191));
  functionalities = {navigate22};
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionalities, description5,
                                              topology) == Catch::Approx(0.894427191));
  functionalities = {navigate31};
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionalities, description5,
                                              topology) == Catch::Approx(2));
  functionalities = {navigate32};
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionalities, description5,
                                              topology) == Catch::Approx(2));
  functionalities = {navigate41};
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionalities, description5,
                                              topology) == Catch::Approx(2));
  functionalities = {navigate42};
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionalities, description5,
                                              topology) == Catch::Approx(2));
  functionalities = {navigate51};
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionalities, description5,
                                              topology) == Catch::Approx(0.632455532));
  functionalities = {navigate52};
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionalities, description5,
                                              topology) == Catch::Approx(0.632455532));
  functionalities = {navigate61};
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionalities, description5,
                                              topology) == Catch::Approx(1.414213562));
  functionalities = {navigate62};
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionalities, description5,
                                              topology) == Catch::Approx(1.414213562));

  AmrDescription description6 =
      AmrDescription(1, kinematics3, properties, physical_properties, load_handling1);
  // kinematics3, load_handling1, MoveTo
  functionalities = {move_to11};
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionalities, description6,
                                              topology) == Catch::Approx(3.464101615));
  functionalities = {move_to12};
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionalities, description6,
                                              topology) == Catch::Approx(3.464101615));
  functionalities = {move_to21};
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionalities, description6,
                                              topology) == Catch::Approx(1.095445115));
  functionalities = {move_to22};
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionalities, description6,
                                              topology) == Catch::Approx(1.095445115));
  functionalities = {move_to31};
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionalities, description6,
                                              topology) == Catch::Approx(2.449489743));
  functionalities = {move_to32};
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionalities, description6,
                                              topology) == Catch::Approx(2.449489743));
  // kinematics3, load_handling1, Unload
  functionalities = {unload11};
  REQUIRE(
      AmrMobilityHelper::estimateDuration(start_pose, functionalities, description6, topology) ==
      Catch::Approx(description6.getLoadHandling().getUnloadTime()));
  // kinematics3, load_handling1, Load
  functionalities = {load11};
  REQUIRE(
      AmrMobilityHelper::estimateDuration(start_pose, functionalities, description6, topology) ==
      Catch::Approx(description6.getLoadHandling().getLoadTime()));
  // kinematics3, load_handling1, Navigate
  functionalities = {navigate11};
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionalities, description6,
                                              topology) == Catch::Approx(4.898979486));
  functionalities = {navigate12};
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionalities, description6,
                                              topology) == Catch::Approx(4.898979486));
  functionalities = {navigate21};
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionalities, description6,
                                              topology) == Catch::Approx(1.549193338));
  functionalities = {navigate22};
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionalities, description6,
                                              topology) == Catch::Approx(1.549193338));
  functionalities = {navigate31};
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionalities, description6,
                                              topology) == Catch::Approx(3.464101615));
  functionalities = {navigate32};
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionalities, description6,
                                              topology) == Catch::Approx(3.464101615));
  functionalities = {navigate41};
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionalities, description6,
                                              topology) == Catch::Approx(3.464101615));
  functionalities = {navigate42};
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionalities, description6,
                                              topology) == Catch::Approx(3.464101615));
  functionalities = {navigate51};
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionalities, description6,
                                              topology) == Catch::Approx(1.095445115));
  functionalities = {navigate52};
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionalities, description6,
                                              topology) == Catch::Approx(1.095445115));
  functionalities = {navigate61};
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionalities, description6,
                                              topology) == Catch::Approx(2.449489743));
  functionalities = {navigate62};
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionalities, description6,
                                              topology) == Catch::Approx(2.449489743));
}

TEST_CASE("estimateDuration single functionality (neglect rotation cost, neglect "
          "payload weight, stop at functionality endpoint)",
          "[mobility_helper]") {
  // preparation
  auto topology = Topology({15, 15, 15});
  daisi::util::Pose start_pose({0, 0}, 0);
  daisi::util::Pose impossible_start_pose1({-1, 0}, 0);
  daisi::util::Pose impossible_start_pose2({20, 0}, 0);

  auto kinematics1 = AmrKinematics(1, 0, 1, 1);
  auto kinematics2 = AmrKinematics(10, 0, 10, 10);
  auto kinematics3 = AmrKinematics(10, 0, 2.5, 5);

  auto properties = AmrProperties();
  AmrProperties impossible_properties("manufacturer", "model_name", 0, "device_type",
                                      "friendly_name", {});
  auto physical_properties = AmrPhysicalProperties();
  auto load_handling1 = AmrLoadHandlingUnit(1, 1, daisi::cpps::mrta::model::Ability());
  auto load_handling2 = AmrLoadHandlingUnit(10, 10, daisi::cpps::mrta::model::Ability());
  auto load_handling3 = AmrLoadHandlingUnit(2.5, 5, daisi::cpps::mrta::model::Ability());

  auto move_to11 = MoveTo({10, 0});
  auto move_to12 = MoveTo({0, 10});
  auto move_to21 = MoveTo({1, 0});
  auto move_to22 = MoveTo({0, 1});
  auto move_to31 = MoveTo({3, 4});
  auto move_to32 = MoveTo({4, 3});

  auto unload11 = Unload(start_pose.position);

  auto load11 = Load(start_pose.position);

  auto navigate11 = Navigate({{10, 0}, {0, 0}});
  auto navigate12 = Navigate({{0, 10}, {0, 0}});
  auto navigate21 = Navigate({{1, 0}, {0, 0}});
  auto navigate22 = Navigate({{0, 1}, {0, 0}});
  auto navigate31 = Navigate({{3, 4}, {0, 0}});
  auto navigate32 = Navigate({{4, 3}, {0, 0}});
  auto navigate41 = Navigate({{0, 0}, {10, 0}});
  auto navigate42 = Navigate({{0, 0}, {0, 10}});
  auto navigate51 = Navigate({{0, 0}, {1, 0}});
  auto navigate52 = Navigate({{0, 0}, {0, 1}});
  auto navigate61 = Navigate({{0, 0}, {3, 4}});
  auto navigate62 = Navigate({{0, 0}, {4, 3}});

  FunctionalityVariant functionality;

  AmrDescription impossible_description =
      AmrDescription(1, kinematics1, impossible_properties, physical_properties, load_handling1);

  functionality = move_to11;
  REQUIRE_THROWS_AS(AmrMobilityHelper::estimateDuration(start_pose, functionality,
                                                        impossible_description, topology),
                    std::invalid_argument);
  functionality = unload11;
  REQUIRE_THROWS_AS(AmrMobilityHelper::estimateDuration(start_pose, functionality,
                                                        impossible_description, topology),
                    std::invalid_argument);
  functionality = load11;
  REQUIRE_THROWS_AS(AmrMobilityHelper::estimateDuration(start_pose, functionality,
                                                        impossible_description, topology),
                    std::invalid_argument);
  functionality = navigate11;
  REQUIRE_THROWS_AS(AmrMobilityHelper::estimateDuration(start_pose, functionality,
                                                        impossible_description, topology),
                    std::invalid_argument);

  AmrDescription description1 =
      AmrDescription(1, kinematics1, properties, physical_properties, load_handling1);
  functionality = MoveTo({1, 0});
  REQUIRE_THROWS_AS(AmrMobilityHelper::estimateDuration(impossible_start_pose1, functionality,
                                                        description1, topology),
                    std::invalid_argument);
  REQUIRE_THROWS_AS(AmrMobilityHelper::estimateDuration(impossible_start_pose2, functionality,
                                                        description1, topology),
                    std::invalid_argument);
  functionality = MoveTo({-1, 0});
  REQUIRE_THROWS_AS(
      AmrMobilityHelper::estimateDuration(start_pose, functionality, description1, topology),
      std::invalid_argument);
  functionality = MoveTo({20, 0});
  REQUIRE_THROWS_AS(
      AmrMobilityHelper::estimateDuration(start_pose, functionality, description1, topology),
      std::invalid_argument);
  functionality = Unload({-1, 0});
  REQUIRE_THROWS_AS(
      AmrMobilityHelper::estimateDuration(start_pose, functionality, description1, topology),
      std::invalid_argument);
  functionality = Unload({20, 0});
  REQUIRE_THROWS_AS(
      AmrMobilityHelper::estimateDuration(start_pose, functionality, description1, topology),
      std::invalid_argument);
  functionality = Load({-1, 0});
  REQUIRE_THROWS_AS(
      AmrMobilityHelper::estimateDuration(start_pose, functionality, description1, topology),
      std::invalid_argument);
  functionality = Load({20, 0});
  REQUIRE_THROWS_AS(
      AmrMobilityHelper::estimateDuration(start_pose, functionality, description1, topology),
      std::invalid_argument);
  functionality = Navigate({{-1, 0}});
  REQUIRE_THROWS_AS(
      AmrMobilityHelper::estimateDuration(start_pose, functionality, description1, topology),
      std::invalid_argument);
  functionality = Navigate({{0, 20}});
  REQUIRE_THROWS_AS(
      AmrMobilityHelper::estimateDuration(start_pose, functionality, description1, topology),
      std::invalid_argument);

  AmrDescription description2 =
      AmrDescription(1, kinematics1, properties, physical_properties, load_handling1);
  // kinematics1, load_handling1, MoveTo
  functionality = move_to11;
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionality, description2, topology) ==
          Catch::Approx(11));
  functionality = move_to12;
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionality, description2, topology) ==
          Catch::Approx(11));
  functionality = move_to21;
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionality, description2, topology) ==
          Catch::Approx(2));
  functionality = move_to22;
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionality, description2, topology) ==
          Catch::Approx(2));
  functionality = move_to31;
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionality, description2, topology) ==
          Catch::Approx(6));
  functionality = move_to32;
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionality, description2, topology) ==
          Catch::Approx(6));
  // kinematics1, load_handling1, Unload
  functionality = unload11;
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionality, description2, topology) ==
          Catch::Approx(description2.getLoadHandling().getUnloadTime()));
  // kinematics1, load_handling1, Load
  functionality = load11;
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionality, description2, topology) ==
          Catch::Approx(description2.getLoadHandling().getLoadTime()));
  // kinematics1, load_handling1, Navigate
  functionality = navigate11;
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionality, description2, topology) ==
          Catch::Approx(21));
  functionality = navigate12;
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionality, description2, topology) ==
          Catch::Approx(21));
  functionality = navigate21;
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionality, description2, topology) ==
          Catch::Approx(3));
  functionality = navigate22;
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionality, description2, topology) ==
          Catch::Approx(3));
  functionality = navigate31;
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionality, description2, topology) ==
          Catch::Approx(11));
  functionality = navigate32;
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionality, description2, topology) ==
          Catch::Approx(11));
  functionality = navigate41;
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionality, description2, topology) ==
          Catch::Approx(11));
  functionality = navigate42;
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionality, description2, topology) ==
          Catch::Approx(11));
  functionality = navigate51;
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionality, description2, topology) ==
          Catch::Approx(2));
  functionality = navigate52;
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionality, description2, topology) ==
          Catch::Approx(2));
  functionality = navigate61;
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionality, description2, topology) ==
          Catch::Approx(6));
  functionality = navigate62;
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionality, description2, topology) ==
          Catch::Approx(6));

  AmrDescription description3 =
      AmrDescription(1, kinematics1, properties, physical_properties, load_handling2);
  // kinematics1, load_handling2, MoveTo
  functionality = move_to11;
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionality, description3, topology) ==
          Catch::Approx(11));
  functionality = move_to12;
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionality, description3, topology) ==
          Catch::Approx(11));
  functionality = move_to21;
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionality, description3, topology) ==
          Catch::Approx(2));
  functionality = move_to22;
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionality, description3, topology) ==
          Catch::Approx(2));
  functionality = move_to31;
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionality, description3, topology) ==
          Catch::Approx(6));
  functionality = move_to32;
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionality, description3, topology) ==
          Catch::Approx(6));
  // kinematics1, load_handling2, Unload
  functionality = unload11;
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionality, description3, topology) ==
          Catch::Approx(description3.getLoadHandling().getUnloadTime()));
  // kinematics1, load_handling2, Load
  functionality = load11;
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionality, description3, topology) ==
          Catch::Approx(description3.getLoadHandling().getLoadTime()));
  // kinematics1, load_handling2, Navigate
  functionality = navigate11;
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionality, description3, topology) ==
          Catch::Approx(21));
  functionality = navigate12;
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionality, description3, topology) ==
          Catch::Approx(21));
  functionality = navigate21;
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionality, description3, topology) ==
          Catch::Approx(3));
  functionality = navigate22;
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionality, description3, topology) ==
          Catch::Approx(3));
  functionality = navigate31;
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionality, description3, topology) ==
          Catch::Approx(11));
  functionality = navigate32;
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionality, description3, topology) ==
          Catch::Approx(11));
  functionality = navigate41;
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionality, description3, topology) ==
          Catch::Approx(11));
  functionality = navigate42;
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionality, description3, topology) ==
          Catch::Approx(11));
  functionality = navigate51;
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionality, description3, topology) ==
          Catch::Approx(2));
  functionality = navigate52;
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionality, description3, topology) ==
          Catch::Approx(2));
  functionality = navigate61;
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionality, description3, topology) ==
          Catch::Approx(6));
  functionality = navigate62;
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionality, description3, topology) ==
          Catch::Approx(6));

  AmrDescription description4 =
      AmrDescription(1, kinematics1, properties, physical_properties, load_handling3);
  // kinematics1, load_handling3, MoveTo
  functionality = move_to11;
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionality, description4, topology) ==
          Catch::Approx(11));
  functionality = move_to12;
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionality, description4, topology) ==
          Catch::Approx(11));
  functionality = move_to21;
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionality, description4, topology) ==
          Catch::Approx(2));
  functionality = move_to22;
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionality, description4, topology) ==
          Catch::Approx(2));
  functionality = move_to31;
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionality, description4, topology) ==
          Catch::Approx(6));
  functionality = move_to32;
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionality, description4, topology) ==
          Catch::Approx(6));
  // kinematics1, load_handling3, Unload
  functionality = unload11;
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionality, description4, topology) ==
          Catch::Approx(description4.getLoadHandling().getUnloadTime()));
  // kinematics1, load_handling3, Load
  functionality = load11;
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionality, description4, topology) ==
          Catch::Approx(description4.getLoadHandling().getLoadTime()));
  // kinematics1, load_handling3, Navigate
  functionality = navigate11;
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionality, description4, topology) ==
          Catch::Approx(21));
  functionality = navigate12;
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionality, description4, topology) ==
          Catch::Approx(21));
  functionality = navigate21;
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionality, description4, topology) ==
          Catch::Approx(3));
  functionality = navigate22;
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionality, description4, topology) ==
          Catch::Approx(3));
  functionality = navigate31;
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionality, description4, topology) ==
          Catch::Approx(11));
  functionality = navigate32;
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionality, description4, topology) ==
          Catch::Approx(11));
  functionality = navigate41;
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionality, description4, topology) ==
          Catch::Approx(11));
  functionality = navigate42;
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionality, description4, topology) ==
          Catch::Approx(11));
  functionality = navigate51;
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionality, description4, topology) ==
          Catch::Approx(2));
  functionality = navigate52;
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionality, description4, topology) ==
          Catch::Approx(2));
  functionality = navigate61;
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionality, description4, topology) ==
          Catch::Approx(6));
  functionality = navigate62;
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionality, description4, topology) ==
          Catch::Approx(6));

  AmrDescription description5 =
      AmrDescription(1, kinematics2, properties, physical_properties, load_handling1);
  // kinematics2, load_handling1, MoveTo
  functionality = move_to11;
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionality, description5, topology) ==
          Catch::Approx(2));
  functionality = move_to12;
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionality, description5, topology) ==
          Catch::Approx(2));
  functionality = move_to21;
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionality, description5, topology) ==
          Catch::Approx(0.632455532));
  functionality = move_to22;
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionality, description5, topology) ==
          Catch::Approx(0.632455532));
  functionality = move_to31;
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionality, description5, topology) ==
          Catch::Approx(1.414213562));
  functionality = move_to32;
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionality, description5, topology) ==
          Catch::Approx(1.414213562));
  // kinematics2, load_handling1, Unload
  functionality = unload11;
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionality, description5, topology) ==
          Catch::Approx(description5.getLoadHandling().getUnloadTime()));
  // kinematics2, load_handling1, Load
  functionality = load11;
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionality, description5, topology) ==
          Catch::Approx(description5.getLoadHandling().getLoadTime()));
  // kinematics2, load_handling1, Navigate
  functionality = navigate11;
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionality, description5, topology) ==
          Catch::Approx(3));
  functionality = navigate12;
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionality, description5, topology) ==
          Catch::Approx(3));
  functionality = navigate21;
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionality, description5, topology) ==
          Catch::Approx(0.894427191));
  functionality = navigate22;
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionality, description5, topology) ==
          Catch::Approx(0.894427191));
  functionality = navigate31;
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionality, description5, topology) ==
          Catch::Approx(2));
  functionality = navigate32;
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionality, description5, topology) ==
          Catch::Approx(2));
  functionality = navigate41;
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionality, description5, topology) ==
          Catch::Approx(2));
  functionality = navigate42;
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionality, description5, topology) ==
          Catch::Approx(2));
  functionality = navigate51;
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionality, description5, topology) ==
          Catch::Approx(0.632455532));
  functionality = navigate52;
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionality, description5, topology) ==
          Catch::Approx(0.632455532));
  functionality = navigate61;
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionality, description5, topology) ==
          Catch::Approx(1.414213562));
  functionality = navigate62;
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionality, description5, topology) ==
          Catch::Approx(1.414213562));

  AmrDescription description6 =
      AmrDescription(1, kinematics3, properties, physical_properties, load_handling1);
  // kinematics3, load_handling1, MoveTo
  functionality = move_to11;
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionality, description6, topology) ==
          Catch::Approx(3.464101615));
  functionality = move_to12;
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionality, description6, topology) ==
          Catch::Approx(3.464101615));
  functionality = move_to21;
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionality, description6, topology) ==
          Catch::Approx(1.095445115));
  functionality = move_to22;
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionality, description6, topology) ==
          Catch::Approx(1.095445115));
  functionality = move_to31;
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionality, description6, topology) ==
          Catch::Approx(2.449489743));
  functionality = move_to32;
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionality, description6, topology) ==
          Catch::Approx(2.449489743));
  // kinematics3, load_handling1, Unload
  functionality = unload11;
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionality, description6, topology) ==
          Catch::Approx(description6.getLoadHandling().getUnloadTime()));
  // kinematics3, load_handling1, Load
  functionality = load11;
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionality, description6, topology) ==
          Catch::Approx(description6.getLoadHandling().getLoadTime()));
  // kinematics3, load_handling1, Navigate
  functionality = navigate11;
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionality, description6, topology) ==
          Catch::Approx(4.898979486));
  functionality = navigate12;
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionality, description6, topology) ==
          Catch::Approx(4.898979486));
  functionality = navigate21;
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionality, description6, topology) ==
          Catch::Approx(1.549193338));
  functionality = navigate22;
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionality, description6, topology) ==
          Catch::Approx(1.549193338));
  functionality = navigate31;
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionality, description6, topology) ==
          Catch::Approx(3.464101615));
  functionality = navigate32;
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionality, description6, topology) ==
          Catch::Approx(3.464101615));
  functionality = navigate41;
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionality, description6, topology) ==
          Catch::Approx(3.464101615));
  functionality = navigate42;
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionality, description6, topology) ==
          Catch::Approx(3.464101615));
  functionality = navigate51;
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionality, description6, topology) ==
          Catch::Approx(1.095445115));
  functionality = navigate52;
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionality, description6, topology) ==
          Catch::Approx(1.095445115));
  functionality = navigate61;
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionality, description6, topology) ==
          Catch::Approx(2.449489743));
  functionality = navigate62;
  REQUIRE(AmrMobilityHelper::estimateDuration(start_pose, functionality, description6, topology) ==
          Catch::Approx(2.449489743));
}

TEST_CASE("calculatePhases single functionality (neglect rotation cost, neglect "
          "payload weight, stop at functionality endpoint)",
          "[mobility_helper]") {
  // preparation
  auto topology = Topology({15, 15, 15});
  daisi::util::Pose start_pose({0, 0}, 0);
  daisi::util::Pose impossible_start_pose1({-1, 0}, 0);
  daisi::util::Pose impossible_start_pose2({20, 0}, 0);

  auto kinematics1 = AmrKinematics(1, 0, 1, 1);
  auto kinematics2 = AmrKinematics(10, 0, 10, 10);
  auto kinematics3 = AmrKinematics(10, 0, 2.5, 5);
  auto kinematics_list = {kinematics1, kinematics2, kinematics3};

  auto properties = AmrProperties();
  AmrProperties impossible_properties("manufacturer", "model_name", 0, "device_type",
                                      "friendly_name", {});
  auto physical_properties = AmrPhysicalProperties();
  auto load_handling1 = AmrLoadHandlingUnit(1, 1, daisi::cpps::mrta::model::Ability());
  auto load_handling2 = AmrLoadHandlingUnit(10, 10, daisi::cpps::mrta::model::Ability());
  auto load_handling3 = AmrLoadHandlingUnit(2.5, 5, daisi::cpps::mrta::model::Ability());
  auto load_handling_list = {load_handling1, load_handling2, load_handling3};

  auto move_to11 = MoveTo({10, 0});
  auto move_to12 = MoveTo({0, 10});
  auto move_to21 = MoveTo({1, 0});
  auto move_to22 = MoveTo({0, 1});
  auto move_to31 = MoveTo({3, 4});
  auto move_to32 = MoveTo({4, 3});

  auto unload11 = Unload(start_pose.position);

  auto load11 = Load(start_pose.position);

  auto navigate11 = Navigate({{10, 0}, {0, 0}});
  auto navigate12 = Navigate({{0, 10}, {0, 0}});
  auto navigate21 = Navigate({{1, 0}, {0, 0}});
  auto navigate22 = Navigate({{0, 1}, {0, 0}});
  auto navigate31 = Navigate({{3, 4}, {0, 0}});
  auto navigate32 = Navigate({{4, 3}, {0, 0}});
  auto navigate41 = Navigate({{0, 0}, {10, 0}});
  auto navigate42 = Navigate({{0, 0}, {0, 10}});
  auto navigate51 = Navigate({{0, 0}, {1, 0}});
  auto navigate52 = Navigate({{0, 0}, {0, 1}});
  auto navigate61 = Navigate({{0, 0}, {3, 4}});
  auto navigate62 = Navigate({{0, 0}, {4, 3}});

  const FunctionalityVariant functionality_list[] = {
      move_to11,  move_to12,  move_to21,  move_to22,  move_to31,  move_to32,  unload11,
      load11,     navigate11, navigate12, navigate21, navigate22, navigate31, navigate32,
      navigate41, navigate42, navigate51, navigate52, navigate61, navigate62};

  FunctionalityVariant fail_functionality;

  AmrDescription impossible_description =
      AmrDescription(1, kinematics1, impossible_properties, physical_properties, load_handling1);

  fail_functionality = move_to11;
  REQUIRE_THROWS_AS(AmrMobilityHelper::calculatePhases(0, start_pose, fail_functionality,
                                                       impossible_description, topology),
                    std::invalid_argument);
  fail_functionality = unload11;
  REQUIRE_THROWS_AS(AmrMobilityHelper::calculatePhases(0, start_pose, fail_functionality,
                                                       impossible_description, topology),
                    std::invalid_argument);
  fail_functionality = load11;
  REQUIRE_THROWS_AS(AmrMobilityHelper::calculatePhases(0, start_pose, fail_functionality,
                                                       impossible_description, topology),
                    std::invalid_argument);
  fail_functionality = navigate11;
  REQUIRE_THROWS_AS(AmrMobilityHelper::calculatePhases(0, start_pose, fail_functionality,
                                                       impossible_description, topology),
                    std::invalid_argument);

  AmrDescription description1 =
      AmrDescription(1, kinematics1, properties, physical_properties, load_handling1);
  fail_functionality = MoveTo({1, 0});
  REQUIRE_THROWS_AS(AmrMobilityHelper::calculatePhases(0, impossible_start_pose1,
                                                       fail_functionality, description1, topology),
                    std::invalid_argument);
  REQUIRE_THROWS_AS(AmrMobilityHelper::calculatePhases(0, impossible_start_pose2,
                                                       fail_functionality, description1, topology),
                    std::invalid_argument);
  fail_functionality = MoveTo({-1, 0});
  REQUIRE_THROWS_AS(
      AmrMobilityHelper::calculatePhases(0, start_pose, fail_functionality, description1, topology),
      std::invalid_argument);
  fail_functionality = MoveTo({20, 0});
  REQUIRE_THROWS_AS(
      AmrMobilityHelper::calculatePhases(0, start_pose, fail_functionality, description1, topology),
      std::invalid_argument);
  fail_functionality = Unload({-1, 0});
  REQUIRE_THROWS_AS(
      AmrMobilityHelper::calculatePhases(0, start_pose, fail_functionality, description1, topology),
      std::invalid_argument);
  fail_functionality = Unload({20, 0});
  REQUIRE_THROWS_AS(
      AmrMobilityHelper::calculatePhases(0, start_pose, fail_functionality, description1, topology),
      std::invalid_argument);
  fail_functionality = Load({-1, 0});
  REQUIRE_THROWS_AS(
      AmrMobilityHelper::estimateDuration(start_pose, fail_functionality, description1, topology),
      std::invalid_argument);
  fail_functionality = Load({20, 0});
  REQUIRE_THROWS_AS(
      AmrMobilityHelper::calculatePhases(0, start_pose, fail_functionality, description1, topology),
      std::invalid_argument);
  fail_functionality = Navigate({{-1, 0}});
  REQUIRE_THROWS_AS(
      AmrMobilityHelper::calculatePhases(0, start_pose, fail_functionality, description1, topology),
      std::invalid_argument);
  fail_functionality = Navigate({{0, 20}});
  REQUIRE_THROWS_AS(
      AmrMobilityHelper::calculatePhases(0, start_pose, fail_functionality, description1, topology),
      std::invalid_argument);

  for (auto kinematics : kinematics_list) {
    for (auto load_handling : load_handling_list) {
      AmrDescription description(1, kinematics, properties, physical_properties, load_handling);
      for (auto functionality : functionality_list) {
        auto phases =
            AmrMobilityHelper::calculatePhases(0, start_pose, functionality, description, topology);
        REQUIRE(phases.back().timestamp == AmrMobilityHelper::estimateDuration(
                                               start_pose, functionality, description, topology));
        REQUIRE(phases.back().state == AmrMobilityState::kIdle);
        auto prev_phase = phases[0];
        for (unsigned long i = 1; i < phases.size(); i++) {
          REQUIRE(prev_phase.state <= phases[i].state);
          REQUIRE(prev_phase.timestamp <= phases[i].timestamp);
          prev_phase = phases[i];
        }
        REQUIRE(phases.size() >= 1);
        double total_phases_distance = 0;
        daisi::util::Position start_pos = start_pose.position;
        daisi::util::Position last_pos = start_pos;
        for (auto phase : phases) {
          total_phases_distance += (phase.position - last_pos).GetLength();
          last_pos = phase.position;
        }
        if (std::holds_alternative<Navigate>(functionality)) {
          auto navigate = std::get<Navigate>(functionality);
          REQUIRE(phases.size() <= 4 + navigate.waypoints.size());
          unsigned long i = 0;
          for (auto wp : navigate.waypoints) {
            for (; i < phases.size(); i++) {
              if (wp == phases[i].position) break;
            }
            REQUIRE(wp.x == Catch::Approx(phases[i].position.x).margin(0.000001));
            REQUIRE(wp.y == Catch::Approx(phases[i].position.y).margin(0.000001));
          }
          double total_wp_distance = 0;
          daisi::util::Position last_wp = start_pose.position;
          for (auto wp : navigate.waypoints) {
            total_wp_distance += (wp - last_wp).GetLength();
            last_wp = wp;
          }
          REQUIRE(total_phases_distance == total_wp_distance);
        }
        if (std::holds_alternative<Unload>(functionality)) {
          double total_wp_distance =
              (std::get<Unload>(functionality).destination - start_pos).GetLength();
          REQUIRE(total_phases_distance == total_wp_distance);
        }
        if (std::holds_alternative<MoveTo>(functionality)) {
          double total_wp_distance =
              (std::get<MoveTo>(functionality).destination - start_pos).GetLength();
          REQUIRE(total_phases_distance == total_wp_distance);
        }
        if (std::holds_alternative<Load>(functionality)) {
          double total_wp_distance =
              (std::get<Load>(functionality).destination - start_pos).GetLength();
          REQUIRE(total_phases_distance == total_wp_distance);
        }
        for (auto phase : phases) {
          switch (phase.state) {
            case AmrMobilityState::kAccelerating:
              REQUIRE(phase.acceleration != daisi::util::Acceleration{0, 0});
              REQUIRE((phase.velocity == daisi::util::Velocity{0, 0} ||
                       (phase.velocity.x * phase.acceleration.x +
                        phase.velocity.y * phase.acceleration.y) /
                               phase.velocity.GetLength() / phase.acceleration.GetLength() ==
                           1));
              break;
            case AmrMobilityState::kConstSpeedTraveling:
              REQUIRE(phase.velocity != daisi::util::Velocity{0, 0});
              REQUIRE(phase.acceleration == daisi::util::Acceleration{0, 0});
              break;
            case AmrMobilityState::kDecelerating:
              REQUIRE(phase.acceleration != daisi::util::Acceleration{0, 0});
              REQUIRE((phase.velocity == daisi::util::Velocity{0, 0} ||
                       (phase.velocity.x * phase.acceleration.x +
                        phase.velocity.y * phase.acceleration.y) /
                               phase.velocity.GetLength() / phase.acceleration.GetLength() ==
                           Catch::Approx(-1).margin(0.000001)));
              break;
            case AmrMobilityState::kStationary:
              REQUIRE(phase.acceleration == daisi::util::Acceleration{0, 0});
              REQUIRE(phase.velocity == daisi::util::Velocity{0, 0});
              break;
            case AmrMobilityState::kIdle:
              REQUIRE(phase.acceleration == daisi::util::Acceleration{0, 0});
              REQUIRE(phase.velocity == daisi::util::Velocity{0, 0});
              break;
            default:
              REQUIRE(false);
          }
        }
      }
    }
  }
  daisi::util::Duration start_timestamp(10);
  FunctionalityVariant functionality = Unload({0, 0});
  AmrMobilityStatus prev_phase;
  std::vector<AmrMobilityStatus> phases;
  AmrMobilityStatus start;
  start.timestamp = start_timestamp;
  phases = AmrMobilityHelper::calculatePhases(start_timestamp, start_pose, functionality,
                                              description1, topology);
  REQUIRE(phases.back().timestamp ==
          start.timestamp + AmrMobilityHelper::estimateDuration(start_pose, functionality,
                                                                description1, topology));
  REQUIRE(phases.back().state == AmrMobilityState::kIdle);
  prev_phase = phases[0];
  for (unsigned long i = 1; i < phases.size(); i++) {
    REQUIRE(prev_phase.state <= phases[i].state);
    REQUIRE(prev_phase.timestamp <= phases[i].timestamp);
    prev_phase = phases[i];
  }
  REQUIRE(phases.size() == 2);

  functionality = MoveTo({0, 0});
  phases = AmrMobilityHelper::calculatePhases(start_timestamp, start_pose, functionality,
                                              description1, topology);
  REQUIRE(phases.back().timestamp ==
          start.timestamp + AmrMobilityHelper::estimateDuration(start_pose, functionality,
                                                                description1, topology));
  REQUIRE(phases.back().state == AmrMobilityState::kIdle);
  prev_phase = phases[0];
  for (unsigned long i = 1; i < phases.size(); i++) {
    REQUIRE(prev_phase.state <= phases[i].state);
    REQUIRE(prev_phase.timestamp <= phases[i].timestamp);
    prev_phase = phases[i];
  }
  REQUIRE(phases.size() == 1);

  functionality = Load({0, 0});
  phases = AmrMobilityHelper::calculatePhases(start_timestamp, start_pose, functionality,
                                              description1, topology);
  REQUIRE(phases.back().timestamp ==
          start.timestamp + AmrMobilityHelper::estimateDuration(start_pose, functionality,
                                                                description1, topology));
  REQUIRE(phases.back().state == AmrMobilityState::kIdle);
  prev_phase = phases[0];
  for (unsigned long i = 1; i < phases.size(); i++) {
    REQUIRE(prev_phase.state <= phases[i].state);
    REQUIRE(prev_phase.timestamp <= phases[i].timestamp);
    prev_phase = phases[i];
  }
  REQUIRE(phases.size() == 2);

  functionality = Navigate({{0, 0}});
  phases = AmrMobilityHelper::calculatePhases(start_timestamp, start_pose, functionality,
                                              description1, topology);
  REQUIRE(phases.back().timestamp ==
          start.timestamp + AmrMobilityHelper::estimateDuration(start_pose, functionality,
                                                                description1, topology));
  REQUIRE(phases.back().state == AmrMobilityState::kIdle);
  prev_phase = phases[0];
  for (unsigned long i = 1; i < phases.size(); i++) {
    REQUIRE(prev_phase.state <= phases[i].state);
    REQUIRE(prev_phase.timestamp <= phases[i].timestamp);
    prev_phase = phases[i];
  }
  REQUIRE(phases.size() == 1);
  if (std::holds_alternative<Navigate>(functionality)) {
    REQUIRE(phases.size() <= 4 + std::get<Navigate>(functionality).waypoints.size());
    unsigned long i = 0;
    for (auto wp : std::get<Navigate>(functionality).waypoints) {
      for (; i < phases.size(); i++) {
        if (wp == phases[i].position) break;
      }
      REQUIRE(wp == phases[i].position);
    }
  }
}

TEST_CASE("calculateMobilityStatus )", "[mobility_helper]") {
  AmrMobilityStatus acc1, acc2, const1, const2, dec1, dec2, stat, idle;
  acc1.acceleration = {1, 0};
  acc1.position = {0, 0};
  acc1.state = AmrMobilityState::kAccelerating;
  acc1.timestamp = 0;
  acc1.velocity = {0, 0};

  acc2.acceleration = {2, 1};
  acc2.position = {1, 1};
  acc2.state = AmrMobilityState::kAccelerating;
  acc2.timestamp = 1000;
  acc2.velocity = {2, 1};

  const1.acceleration = {0, 0};
  const1.position = {0, 0};
  const1.state = AmrMobilityState::kConstSpeedTraveling;
  const1.timestamp = 0;
  const1.velocity = {1, 0};

  const2.acceleration = {0, 0};
  const2.position = {1, 1};
  const2.state = AmrMobilityState::kConstSpeedTraveling;
  const2.timestamp = 1000;
  const2.velocity = {2, 1};

  dec1.acceleration = {-1, 0};
  dec1.position = {0, 0};
  dec1.state = AmrMobilityState::kDecelerating;
  dec1.timestamp = 0;
  dec1.velocity = {0, 0};

  dec2.acceleration = {-2, -1};
  dec2.position = {1, 1};
  dec2.state = AmrMobilityState::kDecelerating;
  dec2.timestamp = 1000;
  dec2.velocity = {2, 1};

  stat.acceleration = {0, 0};
  stat.position = {1, 0};
  stat.state = AmrMobilityState::kStationary;
  stat.timestamp = 1000;
  stat.velocity = {0, 0};

  idle.acceleration = {0, 0};
  idle.position = {1, 0};
  idle.state = AmrMobilityState::kStationary;
  idle.timestamp = 1000;
  idle.velocity = {0, 0};

  // invalid
  REQUIRE_THROWS_AS(AmrMobilityHelper::calculateMobilityStatus(acc1, -1), std::invalid_argument);
  REQUIRE_THROWS_AS(AmrMobilityHelper::calculateMobilityStatus(acc2, 1), std::invalid_argument);
  REQUIRE_THROWS_AS(AmrMobilityHelper::calculateMobilityStatus(const1, -1), std::invalid_argument);
  REQUIRE_THROWS_AS(AmrMobilityHelper::calculateMobilityStatus(const2, 1), std::invalid_argument);
  REQUIRE_THROWS_AS(AmrMobilityHelper::calculateMobilityStatus(dec1, -1), std::invalid_argument);
  REQUIRE_THROWS_AS(AmrMobilityHelper::calculateMobilityStatus(dec2, 1), std::invalid_argument);
  REQUIRE_THROWS_AS(AmrMobilityHelper::calculateMobilityStatus(stat, 1), std::invalid_argument);
  REQUIRE_THROWS_AS(AmrMobilityHelper::calculateMobilityStatus(idle, 1), std::invalid_argument);

  auto all = {acc1, acc2, const1, const2, dec1, dec2, stat, idle};
  for (auto status : all) {
    // trivial
    auto result = AmrMobilityHelper::calculateMobilityStatus(status, status.timestamp);
    REQUIRE(result.acceleration == status.acceleration);
    REQUIRE(result.position == status.position);
    REQUIRE(result.state == status.state);
    REQUIRE(result.timestamp == status.timestamp);
    REQUIRE(result.velocity == status.velocity);

    // t+1
    result = AmrMobilityHelper::calculateMobilityStatus(status, status.timestamp + 1);
    REQUIRE(result.acceleration == status.acceleration);

    if (status.state == AmrMobilityState::kDecelerating) {
      REQUIRE(result.position == status.position + status.velocity - 1 / 2.0 * status.acceleration);
    } else {
      REQUIRE(result.position == status.position + status.velocity + 1 / 2.0 * status.acceleration);
    }

    REQUIRE(result.state == status.state);
    REQUIRE(result.timestamp == status.timestamp + 1);
    REQUIRE(result.velocity == status.velocity + status.acceleration);
  }
}
