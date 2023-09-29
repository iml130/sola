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

#ifndef DAISI_MINHTON_NS3_MINHTON_SCENARIO_STEPS_H_
#define DAISI_MINHTON_NS3_MINHTON_SCENARIO_STEPS_H_

#include <cstdint>
#include <optional>
#include <string>

#include "manager/scenariofile_component.h"
#include "ns3/core-module.h"

namespace daisi::minhton_ns3 {

struct JoinOne {
  static std::string typeName() { return "join-one"; }

  std::optional<ns3::Time> delay;
  std::optional<uint32_t> level;
  std::optional<uint32_t> number;
  std::optional<uint32_t> index;

  void parse(const YAML::Node &node) {
    SERIALIZE_NS3_TIME_OPTIONAL(delay);
    SERIALIZE_VAR(level);
    SERIALIZE_VAR(number);
    SERIALIZE_VAR(index);
  }
};

struct JoinMany {
  static std::string typeName() { return "join-many"; }

  std::optional<ns3::Time> delay;
  uint32_t number;
  std::string mode;

  void parse(const YAML::Node &node) {
    SERIALIZE_NS3_TIME_OPTIONAL(delay);
    SERIALIZE_VAR(number);
    SERIALIZE_VAR(mode);
  }
};

struct LeaveOne {
  static std::string typeName() { return "leave-one"; }

  std::optional<ns3::Time> delay;
  std::optional<uint32_t> level;
  std::optional<uint32_t> number;
  std::optional<uint32_t> index;

  void parse(const YAML::Node &node) {
    SERIALIZE_NS3_TIME_OPTIONAL(delay);
    SERIALIZE_VAR(level);
    SERIALIZE_VAR(number);
    SERIALIZE_VAR(index);
  }
};

struct LeaveMany {
  static std::string typeName() { return "leave-many"; }

  std::optional<ns3::Time> delay;
  uint32_t number;
  std::string mode;

  void parse(const YAML::Node &node) {
    SERIALIZE_NS3_TIME_OPTIONAL(delay);
    SERIALIZE_VAR(number);
    SERIALIZE_VAR(mode);
  }
};

struct SearchMany {
  static std::string typeName() { return "search-many"; }

  std::optional<ns3::Time> delay;
  uint32_t number;

  void parse(const YAML::Node &node) {
    SERIALIZE_NS3_TIME_OPTIONAL(delay);
    SERIALIZE_VAR(number);
  }
};

struct SearchAll {
  static std::string typeName() { return "search-all"; }

  std::optional<ns3::Time> delay;

  void parse(const YAML::Node &node) { SERIALIZE_NS3_TIME_OPTIONAL(delay); }
};

struct FailOne {
  static std::string typeName() { return "fail-one"; }

  std::optional<ns3::Time> delay;
  std::optional<uint32_t> level;
  std::optional<uint32_t> number;
  std::optional<uint32_t> index;

  void parse(const YAML::Node &node) {
    SERIALIZE_NS3_TIME_OPTIONAL(delay);
    SERIALIZE_VAR(level);
    SERIALIZE_VAR(number);
    SERIALIZE_VAR(index);
  }
};

struct FailMany {
  static std::string typeName() { return "fail-many"; }

  std::optional<ns3::Time> delay;
  uint32_t number;

  void parse(const YAML::Node &node) {
    SERIALIZE_NS3_TIME_OPTIONAL(delay);
    SERIALIZE_VAR(number);
  }
};

struct MixedExecution {
  static std::string typeName() { return "mixed-execution"; }

  std::optional<ns3::Time> delay;
  uint32_t join_number;
  uint32_t leave_number;
  uint32_t search_number;

  void parse(const YAML::Node &node) {
    SERIALIZE_NS3_TIME_OPTIONAL(delay);
    SERIALIZE_VAR(join_number);
    SERIALIZE_VAR(leave_number);
    SERIALIZE_VAR(search_number);
  }
};

struct ValidateLeave {
  static std::string typeName() { return "validate-leave"; }

  std::optional<ns3::Time> delay;

  void parse(const YAML::Node &node) { SERIALIZE_NS3_TIME_OPTIONAL(delay); }
};

struct FindQuery {
  static std::string typeName() { return "find-query"; }

  uint32_t level;
  uint32_t number;
  std::string scope;
  std::string query;
  uint32_t validity_threshold;

  void parse(const YAML::Node &node) {
    SERIALIZE_VAR(level);
    SERIALIZE_VAR(number);
    SERIALIZE_VAR(scope);
    SERIALIZE_VAR(query);
    SERIALIZE_VAR(validity_threshold);
  }
};

struct Time {
  static std::string typeName() { return "time"; }

  ns3::Time time;

  void parse(const YAML::Node &node) { SERIALIZE_NS3_TIME(time); }
};

struct StaticBuild {
  static std::string typeName() { return "static-build"; }

  uint32_t number;

  void parse(const YAML::Node &node) { SERIALIZE_VAR(number); }
};

struct RequestCountdown {
  static std::string typeName() { return "request-countdown"; }

  uint32_t number;

  void parse(const YAML::Node &node) { SERIALIZE_VAR(number); }
};

}  // namespace daisi::minhton_ns3

#endif
