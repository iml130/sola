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

#ifndef DAISI_CPPS_AGV_AGV_STATES_NS3_H_
#define DAISI_CPPS_AGV_AGV_STATES_NS3_H_

enum AgvStates { Idle, Working, Charging, AgvError };

enum PhyLogPacketType { PublishService = 0, PosUpdate = 1, OrderUpdate = 2 };

#endif
