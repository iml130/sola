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

#ifndef DAISI_NATTER_MODE_H_
#define DAISI_NATTER_MODE_H_

namespace daisi::natter_ns3 {
enum class NatterMode {
  kNone,
  kMinhcast,
};

inline NatterMode natterModeFromString(const std::string &mode) {
  if (mode == "minhcast") {
    return NatterMode::kMinhcast;
  }
  throw std::runtime_error("invalid natter mode");
}

}  // namespace daisi::natter_ns3

#endif  // DAISI_NATTER_MODE_H_
