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

#ifndef DAISI_MATERIAL_FLOW_TIME_WINDOW_H_
#define DAISI_MATERIAL_FLOW_TIME_WINDOW_H_

#include <optional>

#include "solanet/serializer/serialize.h"
#include "utils/structure_helpers.h"

namespace daisi::material_flow {

class TimeWindow {
public:
  TimeWindow() = default;
  TimeWindow(const util::Duration &earliest_start, const util::Duration &latest_finish);
  TimeWindow(const util::Duration &earliest_start, const util::Duration &latest_finish,
             const util::Duration &spawn_time);

  const util::Duration &getRelativeEarliestStart() const;
  const util::Duration &getRelativeLatestFinish() const;

  const util::Duration getAbsoluteEarliestStart() const;
  const util::Duration getAbsoluteLatestFinish() const;

  bool hasSpawnTime() const;
  void setSpawnTime(const util::Duration &spawn_time);

  SERIALIZE(earliest_start_, latest_finish_, spawn_time_);

private:
  util::Duration earliest_start_ = 0.0;
  util::Duration latest_finish_ = 0.0;

  std::optional<util::Duration> spawn_time_ = std::nullopt;
};

}  // namespace daisi::material_flow

#endif
