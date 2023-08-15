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

#ifndef DAISI_CPPS_AMR_MESSAGE_AMR_STATUS_UPDATE_H_
#define DAISI_CPPS_AMR_MESSAGE_AMR_STATUS_UPDATE_H_

#include "cpps/amr/message/amr_state.h"
#include "solanet/serializer/serialize.h"
#include "utils/structure_helpers.h"

namespace daisi::cpps {

class AmrStatusUpdate {
public:
  AmrStatusUpdate() = default;
  AmrStatusUpdate(const util::Position &position, AmrState state)
      : position_(position), state_(std::move(state)) {}
  util::Position getPosition() const { return position_; }
  AmrState getState() const { return state_; }

  SERIALIZE(position_, state_)

private:
  util::Position position_;
  AmrState state_ = AmrState::kError;
};
}  // namespace daisi::cpps

#endif
