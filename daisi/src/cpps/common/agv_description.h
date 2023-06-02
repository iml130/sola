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

#ifndef DAISI_CPPS_COMMON_AGV_DESCRIPTION_H_
#define DAISI_CPPS_COMMON_AGV_DESCRIPTION_H_

#include <string>

#include "cpps/amr/physical/amr_mobility_model_ns3.h"
#include "cpps/model/ability.h"
#include "cpps/model/kinematics.h"
#include "ns3/ptr.h"

namespace daisi::cpps {
struct AgvDeviceProperties {
  std::string manufacturer;
  std::string model_name;
  uint32_t model_number = 0;
  std::string device_type;
  std::string friendly_name;  // + number for individual AGV

  Kinematics kinematic;
  mrta::model::Ability ability;
};

struct AgvDeviceDescription {
  uint32_t serial_number = 0;
  ns3::Ptr<AmrMobilityModelNs3> mobility;
};

struct AgvDataModel {
  AgvDeviceDescription agv_device_descr;
  AgvDeviceProperties agv_properties;
};

inline std::ostream &operator<<(std::ostream &os, const AgvDataModel &agv_data_model) {
  os << agv_data_model.agv_properties.manufacturer << ";";
  os << agv_data_model.agv_properties.model_name << ";";
  os << agv_data_model.agv_properties.model_number << ";";
  os << agv_data_model.agv_properties.device_type << ";";
  os << agv_data_model.agv_properties.friendly_name << ";";
  os << agv_data_model.agv_properties.kinematic.getMaxVelocity() << ";";
  os << agv_data_model.agv_properties.kinematic.getMinVelocity() << ";";
  os << agv_data_model.agv_properties.kinematic.getMaxAcceleration() << ";";
  os << agv_data_model.agv_properties.kinematic.getMinAcceleration() << ";";
  os << agv_data_model.agv_properties.kinematic.getLoadTime() << ";";
  os << agv_data_model.agv_properties.kinematic.getUnloadTime() << ";";
  os << std::get<1>(agv_data_model.agv_properties.ability) << ";";
  os << std::get<0>(agv_data_model.agv_properties.ability) << ";";
  os << agv_data_model.agv_device_descr.serial_number << ";";

  return os;
}

inline std::istream &operator>>(std::istream &is, AgvDataModel &agv_data_model) {
  std::string payload;
  is >> payload;
  std::string manufacturer = payload.substr(0, payload.find(";"));
  payload = payload.substr(payload.find(";") + 1, payload.size());
  std::string model_name = payload.substr(0, payload.find(";"));
  payload = payload.substr(payload.find(";") + 1, payload.size());
  std::string model_number = payload.substr(0, payload.find(";"));
  payload = payload.substr(payload.find(";") + 1, payload.size());
  std::string device_type = payload.substr(0, payload.find(";"));
  payload = payload.substr(payload.find(";") + 1, payload.size());
  std::string friendly_name = payload.substr(0, payload.find(";"));
  payload = payload.substr(payload.find(";") + 1, payload.size());
  std::string kinematic_max_velo = payload.substr(0, payload.find(";"));
  payload = payload.substr(payload.find(";") + 1, payload.size());
  std::string kinematic_min_velo = payload.substr(0, payload.find(";"));
  payload = payload.substr(payload.find(";") + 1, payload.size());
  std::string kinematic_max_acc = payload.substr(0, payload.find(";"));
  payload = payload.substr(payload.find(";") + 1, payload.size());
  std::string kinematic_min_acc = payload.substr(0, payload.find(";"));
  payload = payload.substr(payload.find(";") + 1, payload.size());
  std::string load_time = payload.substr(0, payload.find(";"));
  payload = payload.substr(payload.find(";") + 1, payload.size());
  std::string unload_time = payload.substr(0, payload.find(";"));
  payload = payload.substr(payload.find(";") + 1, payload.size());
  std::string load_carrier_type = payload.substr(0, payload.find(";"));
  payload = payload.substr(payload.find(";") + 1, payload.size());
  std::string max_payload = payload.substr(0, payload.find(";"));
  payload = payload.substr(payload.find(";") + 1, payload.size());
  std::string serial_number = payload.substr(0, payload.find(";"));

  agv_data_model.agv_properties.manufacturer = manufacturer;
  agv_data_model.agv_properties.model_name = model_name;
  agv_data_model.agv_properties.model_number = std::stoi(model_number);
  agv_data_model.agv_properties.device_type = device_type;
  agv_data_model.agv_properties.friendly_name = friendly_name;
  Kinematics kinematic(std::stod(kinematic_max_velo), std::stod(kinematic_min_velo),
                       std::stod(kinematic_max_acc), std::stod(kinematic_min_acc),
                       std::stod(load_time), std::stod(unload_time));
  agv_data_model.agv_properties.kinematic = kinematic;

  mrta::model::LoadCarrier load_carrier(load_carrier_type);
  float max_payload_number = std::stod(max_payload);
  mrta::model::Ability ability{max_payload_number, load_carrier};
  agv_data_model.agv_properties.ability = ability;

  agv_data_model.agv_device_descr.serial_number = std::stoi(serial_number);

  return is;
}

}  // namespace daisi::cpps

#endif
