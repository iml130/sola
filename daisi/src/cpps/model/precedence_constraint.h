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

#ifndef DAISI_MODEL_PRECEDENCE_CONSTRAINT_NS3_H_
#define DAISI_MODEL_PRECEDENCE_CONSTRAINT_NS3_H_

#include <optional>

#include "solanet/serializer/serialize.h"

namespace daisi::cpps {

struct PrecedenceConstraints {
public:
  PrecedenceConstraints() : earliest_valid_start_time_(0) {}

  PrecedenceConstraints(const double &earliest_valid_start_time,
                        const std::vector<std::string> &constraint_uuids)
      : earliest_valid_start_time_(earliest_valid_start_time),
        constraint_uuids_(constraint_uuids) {}

  void setEarliestValidStartTime(const double &time) { earliest_valid_start_time_ = time; }
  double getEarliestValidStartTime() const { return earliest_valid_start_time_; }

  std::vector<std::string> getConstraintUUIDs() const { return constraint_uuids_; }
  void addConstraintUUID(const std::string &constraint) { constraint_uuids_.push_back(constraint); }

  SERIALIZE(earliest_valid_start_time_, constraint_uuids_);

private:
  double earliest_valid_start_time_;
  std::vector<std::string> constraint_uuids_;
};

}  // namespace daisi::cpps

#endif
