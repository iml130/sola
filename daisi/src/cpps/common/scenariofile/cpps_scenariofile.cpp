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

#include "cpps_scenariofile.h"

namespace daisi::cpps {

std::unordered_map<std::string, AmrDescription> CppsScenariofile::getAmrDescriptions() const {
  std::unordered_map<std::string, AmrDescription> descriptions;
  for (const auto &amr : autonomous_mobile_robots) {
    descriptions[amr.properties.friendly_name] = amr.getAmrDescription();
  }
  return descriptions;
}

std::unordered_map<std::string, MaterialFlowDescriptionScenario>
CppsScenariofile::getMaterialFlowDescriptions() const {
  std::unordered_map<std::string, MaterialFlowDescriptionScenario> descriptions;
  for (const auto &mf : material_flows) {
    descriptions[mf.friendly_name] = mf;
  }
  return descriptions;
}

void CppsScenariofile::verifyScenarioSequenceOfMaterialFlows() const {
  std::vector<SpawnInfoScenario> mfs;
  std::copy_if(scenario_sequence.begin(), scenario_sequence.end(), std::back_inserter(mfs),
               [](const auto &info) { return info.isMaterialFlow(); });

  if (mfs.empty()) {
    throw std::runtime_error("At least one Material Flow must be given in the scenario sequence.");
  }

  // all mfs must be gaussian
  bool all_mfs_have_gaussian = std::all_of(mfs.begin(), mfs.end(), [](const auto &info) {
    return info.spawn_distribution.isGaussian();
  });
  if (!all_mfs_have_gaussian) {
    throw std::runtime_error("Scenario sequence of MFs must be all gaussian.");
  }
}

void CppsScenariofile::verifyScenarioSequenceOfAmrs() const {
  std::vector<SpawnInfoScenario> amrs;
  std::copy_if(scenario_sequence.begin(), scenario_sequence.end(), std::back_inserter(amrs),
               [](const auto &info) { return info.isAmr(); });

  if (amrs.empty()) {
    throw std::runtime_error("At least one AMR must be given in the scenario sequence.");
  }

  // not allowing gaussian distribution for amrs
  bool at_least_one_amrs_has_gaussian = std::any_of(amrs.begin(), amrs.end(), [](const auto &info) {
    return info.spawn_distribution.isGaussian();
  });

  if (at_least_one_amrs_has_gaussian) {
    throw std::runtime_error("Gaussian distribution not allowed for AMRs in scenario sequence.");
  }

  // checking if all are either absolute or relative
  bool all_amrs_have_absolute = std::all_of(amrs.begin(), amrs.end(), [](const auto &info) {
    return info.spawn_distribution.isAbsolute();
  });

  bool all_amrs_have_relative = std::all_of(amrs.begin(), amrs.end(), [](const auto &info) {
    return info.spawn_distribution.isRelative();
  });

  if (!all_amrs_have_absolute && !all_amrs_have_relative) {
    throw std::runtime_error(
        "Scenario sequence of AMRs must be either all absolute or all relative.");
  }

  if (all_amrs_have_absolute) {
    // if absolute, numbers must add up to initial_number_of_amrs

    uint16_t sum = 0;
    for (const auto &info : amrs) {
      sum += info.spawn_distribution.number;
    }

    if (sum != initial_number_of_amrs) {
      throw std::runtime_error(
          "Absolute numbers of AMRs in scenario sequence must add up to initial_number_of_amrs.");
    }
  } else if (all_amrs_have_relative) {
    // if relative, percentages must add up to 1

    double sum = 0;
    for (const auto &info : amrs) {
      sum += info.spawn_distribution.percentage;
    }

    if (abs(sum - 1) > std::numeric_limits<double>::epsilon()) {
      throw std::runtime_error("Percentages of AMRs in scenario sequence must add up to 1.");
    }
  }
}

void CppsScenariofile::calcNumbersOfRelativeAmrDistribution() {
  std::vector<SpawnInfoScenario> amrs;
  std::copy_if(scenario_sequence.begin(), scenario_sequence.end(), std::back_inserter(amrs),
               [](const auto &info) { return info.isAmr(); });

  bool all_amrs_have_relative = std::all_of(amrs.begin(), amrs.end(), [](const auto &info) {
    return info.spawn_distribution.isRelative();
  });

  if (!all_amrs_have_relative) {
    return;
  }

  std::vector<double> percentages;
  std::vector<uint16_t> absolute;
  std::vector<std::pair<size_t, double>> gaps;

  std::transform(amrs.begin(), amrs.end(), std::back_inserter(percentages),
                 [](const auto &info) { return info.spawn_distribution.percentage; });

  // total number * percentage
  for (auto i = 0U; i < amrs.size(); i++) {
    auto percentage = amrs[i].spawn_distribution.percentage;
    absolute.push_back(std::floor(percentage * initial_number_of_amrs));
    gaps.push_back({i, percentage * initial_number_of_amrs - absolute[i]});
  }

  uint16_t missing = initial_number_of_amrs - std::accumulate(absolute.begin(), absolute.end(), 0);

  if (missing > 0) {
    // += 1 for the amr types where there is the most deviation
    std::sort(gaps.begin(), gaps.end(),
              [](const auto &p1, const auto &p2) { return p1.second > p2.second; });
    uint16_t count = 0;

    for (const auto &gap : gaps) {
      absolute[gap.first] += 1;
      count += 1;

      // until the gap is closed
      if (count >= missing) {
        break;
      }
    }
  }

  size_t amr_counter = 0;
  for (auto &info : scenario_sequence) {
    if (info.isAmr()) {
      info.spawn_distribution.number = absolute[amr_counter++];
    }
  }
}

}  // namespace daisi::cpps
