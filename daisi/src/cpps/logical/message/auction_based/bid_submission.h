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

#ifndef DAISI_CPPS_LOGICAL_MESSAGE_AUCTION_BASED_BID_SUBMISSION_H_
#define DAISI_CPPS_LOGICAL_MESSAGE_AUCTION_BASED_BID_SUBMISSION_H_

#include <string>

#include "cpps/amr/model/amr_static_ability.h"
#include "cpps/logical/task_management/metrics_composition.h"
#include "solanet/serializer/serialize.h"
#include "solanet/uuid.h"
#include "solanet/uuid_generator.h"

namespace daisi::cpps::logical {

class BidSubmission {
public:
  BidSubmission() = default;

  BidSubmission(std::string task_uuid, std::string participant_connection,
                const amr::AmrStaticAbility &participant_ability,
                const MetricsComposition &metrics_composition)
      : task_uuid_(std::move(task_uuid)),
        participant_connection_(std::move(participant_connection)),
        participant_ability_(participant_ability),
        metrics_composition_(metrics_composition) {}

  const std::string &getTaskUuid() const { return task_uuid_; }

  const std::string &getParticipantConnection() const { return participant_connection_; }

  const amr::AmrStaticAbility &getParticipantAbility() const { return participant_ability_; }

  const MetricsComposition &getMetricsComposition() { return metrics_composition_; }

  bool operator>(const BidSubmission &other) const {
    if (metrics_composition_ != other.metrics_composition_) {
      return metrics_composition_ > other.metrics_composition_;
    }

    // if metrics are the same, it does not matter whether which one is selected
    // but for comparability a unique ordering is necessary

    // if abilities are unequal, ability can be used for ordering
    if (participant_ability_ != other.participant_ability_) {
      return participant_ability_ < other.participant_ability_;
    }

    // if ability is equal, at least the connection strings are different
    return participant_connection_ > other.participant_connection_;
  }

  solanet::UUID getUUID() const { return uuid_; }

  SERIALIZE(uuid_, task_uuid_, participant_connection_, participant_ability_, metrics_composition_);

private:
  solanet::UUID uuid_ = solanet::generateUUID();

  std::string task_uuid_;

  std::string participant_connection_;

  amr::AmrStaticAbility participant_ability_;

  MetricsComposition metrics_composition_;
};

}  // namespace daisi::cpps::logical

#endif
