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

#ifndef DAISI_CPPS_AMR_MESSAGE_SERIALIZER_H_
#define DAISI_CPPS_AMR_MESSAGE_SERIALIZER_H_

#include <variant>

#include "cpps/amr/amr_description.h"
#include "cpps/amr/amr_topology.h"
#include "cpps/amr/message/amr_order_info.h"
#include "cpps/amr/message/amr_order_update.h"
#include "cpps/amr/message/amr_status_update.h"

namespace daisi::cpps::amr {
using Message = std::variant<AmrDescription,   // physical -> logical
                             AmrStatusUpdate,  // physical -> logical
                             AmrOrderInfo,     // logical -> physical
                             AmrOrderUpdate,   // physical -> logical
                             Topology          // logical -> physical
                             >;

std::string serialize(const Message &msg);

Message deserialize(const std::string &msg);

}  // namespace daisi::cpps::amr

#endif
