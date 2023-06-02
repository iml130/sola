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

#include "cpps/negotiation/utility/utility_evaluator.h"

#include <cassert>
#include <numeric>

namespace daisi::cpps {

UtilityEvaluator::UtilityEvaluator(const UtilityEvaluationType &type) : type_(type) {
  if (type_.components.size() != type_.factors.size()) {
    throw std::invalid_argument("Invalid Utility Evaluation Type");
  }

  float sum_of_factors = std::accumulate(type_.factors.begin(), type_.factors.end(), 0.0);
  if (sum_of_factors > 1.001 or sum_of_factors < 0.999) {
    throw std::invalid_argument("Sum of factors of Utility Evaluation Type should be close to 1");
  }
}

bool UtilityEvaluator::compare(const UtilityDimensions &udim1, const UtilityDimensions &udim2) {
  return udim1.getUtility() > udim2.getUtility();
}

void UtilityEvaluator::evaluate(UtilityDimensions &udim) const {
  if (!(udim.start_time != 0)) throw std::runtime_error("Invalid Start Time");
  if (!(udim.earliest_start_time > 0)) throw std::runtime_error("Invalid Earliest Start Time");
  if (!(udim.latest_start_time > 0)) throw std::runtime_error("Invalid Latest Start Time");
  if (!(udim.execution_duration != 0)) throw std::runtime_error("Invalid Execution Duration");
  if (!(udim.execution_distance != 0)) throw std::runtime_error("Invalid Execution Distance");
  if (!(udim.makespan > 1)) throw std::runtime_error("Invalid Makespan");
  if (!(udim.getQueueSize() >= 0)) throw std::runtime_error("Invalid Queue Size");

  udim.setQuality(0);
  setCosts(udim);
}

void UtilityEvaluator::reevaluate(UtilityDimensions &udim) const {
  if (!(udim.start_time != 0)) throw std::runtime_error("Invalid Start Time");
  if (!(udim.earliest_start_time > 0)) throw std::runtime_error("Invalid Earliest Start Time");
  if (!(udim.latest_start_time > 0)) throw std::runtime_error("Invalid Latest Start Time");
  if (!(udim.execution_duration != 0)) throw std::runtime_error("Invalid Execution Duration");
  if (!(udim.execution_distance != 0)) throw std::runtime_error("Invalid Execution Distance");
  if (!(udim.makespan > 1)) throw std::runtime_error("Invalid Makespan");
  if (!(udim.getQueueSize() >= 0)) throw std::runtime_error("Invalid Queue Size");

  setCosts(udim);
}

void UtilityEvaluator::setCosts(UtilityDimensions &udim) const {
  double costs = 0.0;

  for (size_t i = 0; i < type_.components.size(); i++) {
    UtilityEvaluationComponents component = type_.components[i];
    float factor = type_.factors[i];

    switch (component) {
      case UtilityEvaluationComponents::kMinTravelTime:
        costs += udim.travel_to_pickup_duration / 1000 * factor;
        break;

      case UtilityEvaluationComponents::kMinTravelDistance:
        costs += udim.travel_to_pickup_distance * factor;
        break;

      case UtilityEvaluationComponents::kMinMakespan:
        costs += (udim.makespan - udim.spawn_time) / 1000 * factor;
        break;

      case UtilityEvaluationComponents::kMinDelay:
        costs += udim.getDelay() / 1000 * factor;
        break;

      default:
        throw std::runtime_error("Not implemented yet");
    }
  }

  udim.setCosts(costs);
}

double UtilityEvaluator::linearCombination(double const &first, double const &second,
                                           double const &alpha) {
  return alpha * first + (1 - alpha) * second;
}

}  // namespace daisi::cpps
