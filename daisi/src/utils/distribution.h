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

#ifndef DAISI_UTILS_DISTRIBUTION_H_
#define DAISI_UTILS_DISTRIBUTION_H_

#include <random>
#include <variant>

namespace daisi {

/*
 * Container for different random distributions
 * with unified API to get the next random value
 */
template <typename T> struct Dist {
  /**
   * Get next value for this distribution
   */
  template <typename Generator> T operator()(Generator &gen) {
    static_assert(std::is_integral_v<T>, "T needs to be an integral type");
    if (auto fixed = std::get_if<T>(&dist)) {
      return *fixed;
    }

    if (auto normal = std::get_if<std::normal_distribution<double>>(&dist)) {
      double number = normal->operator()(gen);
      return static_cast<uint64_t>(std::clamp(number, 0.0, std::numeric_limits<double>::max()));
    }

    if (auto uniform = std::get_if<std::uniform_int_distribution<T>>(&dist)) {
      return uniform->operator()(gen);
    }

    if (auto discrete = std::get_if<std::discrete_distribution<T>>(&dist)) {
      return discrete->operator()(gen);
    }
    throw std::runtime_error("invalid distribution");
  }

  std::variant<T, std::normal_distribution<double>, std::uniform_int_distribution<T>,
               std::discrete_distribution<T>>
      dist;
};

}  // namespace daisi
#endif
