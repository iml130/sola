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

#include "amr_physical_asset_application.h"

namespace daisi::cpps {
ns3::TypeId AmrPhysicalAssetApplication::GetTypeId() {
  static ns3::TypeId tid = ns3::TypeId("AmrPhysicalAssetApplication")
                               .SetParent<Application>()
                               .AddConstructor<AmrPhysicalAssetApplication>();
  return tid;
}
}  // namespace daisi::cpps
