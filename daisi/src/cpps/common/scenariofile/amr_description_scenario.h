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

#ifndef DAISI_CPPS_MANAGER_AMR_DESCRIPTION_SCENARIO_H_
#define DAISI_CPPS_MANAGER_AMR_DESCRIPTION_SCENARIO_H_

#include "cpps/amr/amr_description.h"
#include "manager/scenariofile_component.h"

namespace daisi::cpps {

struct AmrKinematicsScenario {
  double max_velocity;
  double min_velocity;
  double max_acceleration;
  double max_deceleration;

  void parse(const YAML::Node &node) {
    SERIALIZE_VAR(max_velocity);
    SERIALIZE_VAR(min_velocity);
    SERIALIZE_VAR(max_acceleration);
    SERIALIZE_VAR(max_deceleration);
  }

  AmrKinematics getAmrKinematics() const {
    return {max_velocity, min_velocity, max_acceleration, max_deceleration};
  }
};

struct AmrLoadHandlingScenario {
  double load_time;
  double unload_time;
  std::string load_carrier;
  float max_payload;

  void parse(const YAML::Node &node) {
    SERIALIZE_VAR(load_time);
    SERIALIZE_VAR(unload_time);
    SERIALIZE_VAR(load_carrier);
    SERIALIZE_VAR(max_payload);
  }

  AmrLoadHandlingUnit getLoadHandlingUnit() const {
    return {load_time, unload_time,
            amr::AmrStaticAbility(amr::LoadCarrier(load_carrier), max_payload)};
  }
};

struct AmrPropertiesScenario {
  std::string device_type;
  std::string manufacturer;
  std::string model_name;
  std::string friendly_name;
  uint32_t model_number;

  void parse(const YAML::Node &node) {
    SERIALIZE_VAR(device_type);
    SERIALIZE_VAR(manufacturer);
    SERIALIZE_VAR(model_name);
    SERIALIZE_VAR(friendly_name);
    SERIALIZE_VAR(model_number);
  }

  AmrProperties getAmrProperties() const {
    return {manufacturer,
            model_name,
            model_number,
            device_type,
            friendly_name,
            {FunctionalityType::kLoad, FunctionalityType::kMoveTo, FunctionalityType::kUnload,
             FunctionalityType::kNavigate}};
  }
};

struct AmrDescriptionScenario {
  AmrKinematicsScenario kinematics;
  AmrLoadHandlingScenario load_handling;
  AmrPropertiesScenario properties;

  void parse(const YAML::Node &node) {
    SERIALIZE_VAR(kinematics);
    SERIALIZE_VAR(load_handling);
    SERIALIZE_VAR(properties);
  }

  AmrDescription getAmrDescription() const {
    AmrPhysicalProperties physical_props;  // TODO define in future?
    return {1, kinematics.getAmrKinematics(), properties.getAmrProperties(), physical_props,
            load_handling.getLoadHandlingUnit()};
  }
};

}  // namespace daisi::cpps

#endif
