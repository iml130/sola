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

#ifndef DAISI_CPPS_AMR_AMR_DESCRIPTION_H_
#define DAISI_CPPS_AMR_AMR_DESCRIPTION_H_

#include "cpps/amr/amr_kinematics.h"
#include "cpps/amr/amr_load_handling_unit.h"
#include "cpps/amr/amr_physical_properties.h"
#include "cpps/amr/amr_properties.h"
#include "solanet/serializer/serialize.h"

namespace daisi::cpps {
class AmrDescription {
public:
  AmrDescription() = default;
  AmrDescription(uint32_t serial_number, const AmrKinematics &kinematics, AmrProperties properties,
                 AmrPhysicalProperties physical_properties,
                 const AmrLoadHandlingUnit &load_handling)
      : serial_number_(serial_number),
        kinematics_(kinematics),
        load_handling_(load_handling),
        properties_(std::move(properties)),
        physical_properties_(std::move(physical_properties)) {}
  uint32_t getSerialNumber() const { return serial_number_; }
  AmrKinematics getKinematics() const { return kinematics_; }
  AmrProperties getProperties() const { return properties_; }
  AmrPhysicalProperties getPhysicalProperties() const { return physical_properties_; }
  AmrLoadHandlingUnit getLoadHandling() const { return load_handling_; }

  SERIALIZE(kinematics_, load_handling_, properties_, physical_properties_, serial_number_);

private:
  uint32_t serial_number_ = 0;
  AmrKinematics kinematics_;
  AmrLoadHandlingUnit load_handling_;
  AmrProperties properties_;
  AmrPhysicalProperties physical_properties_;
};
}  // namespace daisi::cpps
#endif
