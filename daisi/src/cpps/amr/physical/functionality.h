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

#ifndef DAISI_CPPS_AMR_PHYSICAL_FUNCTIONALITY_H_
#define DAISI_CPPS_AMR_PHYSICAL_FUNCTIONALITY_H_

#include <functional>
#include <variant>

#include "cpps/amr/physical/load.h"
#include "cpps/amr/physical/move_to.h"
#include "cpps/amr/physical/navigate.h"
#include "cpps/amr/physical/unload.h"

namespace daisi::cpps {
enum class FunctionalityType { kLoad, kMoveTo, kNavigate, kUnload };

using FunctionalityVariant = std::variant<std::monostate, Load, MoveTo, Navigate, Unload>;
using FunctionalityDoneCallback = std::function<void(const FunctionalityVariant &)>;

}  // namespace daisi::cpps
#endif
