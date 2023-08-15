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

#ifndef DAISI_CPPS_AMR_AMR_LOAD_HANDLING_UNIT_H_
#define DAISI_CPPS_AMR_AMR_LOAD_HANDLING_UNIT_H_

#include <string>

#include "cpps/amr/model/amr_static_ability.h"
#include "solanet/serializer/serialize.h"

namespace daisi::cpps {
class AmrLoadHandlingUnit {
public:
  AmrLoadHandlingUnit() = default;
  AmrLoadHandlingUnit(double load_time_s, double unload_time_s,
                      const amr::AmrStaticAbility &ability)
      : load_time_s_(load_time_s), unload_time_s_(unload_time_s), ability_(ability) {}

  /// @{
  /// Return load/unload time in seconds
  double getLoadTime() const { return load_time_s_; }
  double getUnloadTime() const { return unload_time_s_; }
  /// @}

  amr::AmrStaticAbility getAbility() const { return ability_; }

  SERIALIZE(ability_, load_time_s_, unload_time_s_)

private:
  double load_time_s_ = -1;
  double unload_time_s_ = -1;
  amr::AmrStaticAbility ability_;
};
}  // namespace daisi::cpps
#endif
