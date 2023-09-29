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

#ifndef DAISI_CPPS_AMR_AMR_PROPERTIES_H_
#define DAISI_CPPS_AMR_AMR_PROPERTIES_H_

#include <set>
#include <string>

#include "cpps/amr/model/amr_static_ability.h"
#include "cpps/amr/physical/functionality.h"
#include "solanet/serializer/serialize.h"

namespace daisi::cpps {
class AmrProperties {
public:
  AmrProperties() = default;

  AmrProperties(std::string manufacturer, std::string model_name, uint32_t model_number,
                std::string device_type, std::string friendly_name,
                std::set<FunctionalityType> functionalities = {})
      : device_type_(std::move(device_type)),
        friendly_name_(std::move(friendly_name)),
        functionalities_(std::move(functionalities)),
        manufacturer_(std::move(manufacturer)),
        model_name_(std::move(model_name)),
        model_number_(model_number) {}

  explicit AmrProperties(std::string friendly_name) : friendly_name_(std::move(friendly_name)) {}

  std::string getManufacturer() const { return manufacturer_; }
  std::string getModelName() const { return model_name_; }
  uint32_t getModelNumber() const { return model_number_; }
  std::string getDeviceType() const { return device_type_; }
  std::string getFriendlyName() const { return friendly_name_; }
  std::set<FunctionalityType> getFunctionalities() const { return functionalities_; }

  SERIALIZE(device_type_, friendly_name_, functionalities_, manufacturer_, model_name_,
            model_number_);

private:
  std::string device_type_ = "none";
  std::string friendly_name_ = "none";
  std::set<FunctionalityType> functionalities_ = {
      FunctionalityType::kLoad, FunctionalityType::kMoveTo, FunctionalityType::kUnload,
      FunctionalityType::kNavigate};
  std::string manufacturer_ = "none";
  std::string model_name_ = "none";
  uint32_t model_number_ = 0;
};
}  // namespace daisi::cpps
#endif
