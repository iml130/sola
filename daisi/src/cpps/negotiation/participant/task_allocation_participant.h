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

#ifndef DAISI_TASK_ALLOCATION_PARTICIPANT_NS3_H_
#define DAISI_TASK_ALLOCATION_PARTICIPANT_NS3_H_

#include <memory.h>

#include "cpps/message/serializer.h"
#include "cpps/negotiation/task_management/task_management.h"
#include "sola-ns3/sola_ns3_wrapper.h"

namespace daisi::cpps {

class TaskAllocationParticipant {
public:
  TaskAllocationParticipant(const std::shared_ptr<sola_ns3::SOLAWrapperNs3> &sola,
                            const std::unique_ptr<TaskManagement> &task_management)
      : sola_(sola), task_management_(task_management) {}

  virtual void init() = 0;

  virtual void receiveMessage(const Message &msg) = 0;

  virtual ~TaskAllocationParticipant() = default;

protected:
  const std::shared_ptr<sola_ns3::SOLAWrapperNs3> sola_;
  const std::unique_ptr<TaskManagement> &task_management_;
};

}  // namespace daisi::cpps

#endif
