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

#ifndef DAISI_CPPS_LOGICAL_ALGORITHMS_ALGORITHM_INTERFACE_H_
#define DAISI_CPPS_LOGICAL_ALGORITHMS_ALGORITHM_INTERFACE_H_

#include <memory>

#include "sola-ns3/sola_ns3_wrapper.h"

namespace daisi::cpps::logical {

class AlgorithmInterface {
public:
  AlgorithmInterface(std::shared_ptr<sola_ns3::SOLAWrapperNs3> sola) : sola_(std::move(sola)){};

  virtual ~AlgorithmInterface() = 0;

protected:
  std::shared_ptr<sola_ns3::SOLAWrapperNs3> sola_;
};

}  // namespace daisi::cpps::logical

#endif
