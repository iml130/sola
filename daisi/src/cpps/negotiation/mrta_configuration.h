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

#ifndef DAISI_MRTA_CONFIGURATION_NS3_H_
#define DAISI_MRTA_CONFIGURATION_NS3_H_

#include <vector>

namespace daisi::cpps {

enum InteractionProtocolType {
  kIteratedAuctionPubSubModified,
  kIteratedAuctionPubSubTePSSI,
};

enum TaskManagementType {
  kBasicList,
  kGreedyTSPList,
  kSimpleTemporalNetwork,
};

enum UtilityEvaluationComponents {
  kMinTravelTime,
  kMinTravelDistance,
  kMinMakespan,
  kMinDelay,
};

struct UtilityEvaluationType {
  std::vector<UtilityEvaluationComponents> components;
  std::vector<float> factors;
};

struct MRTAConfig {
  InteractionProtocolType interaction_protocol_type;
  TaskManagementType task_management_type;
  UtilityEvaluationType utility_evaluation_type;
};

}  // namespace daisi::cpps

#endif
