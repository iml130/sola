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

#include "cpps/logical/order_management/metrics_composition.h"
#include "cpps/model/ability.h"
#include "solanet/serializer/serialize.h"

namespace daisi::cpps::logical {

class BidSubmission {
public:
  BidSubmission() = default;

  BidSubmission(std::string task_uuid, std::string participant_connection,
                const daisi::cpps::mrta::model::Ability participant_ability,
                order_management::MetricsComposition metrics_composition)
      : task_uuid_(std::move(task_uuid)),
        participant_connection_(std::move(participant_connection)),
        participant_ability_(participant_ability),
        metrics_composition_(metrics_composition) {}

  const std::string &getTaskUuid() const { return task_uuid_; }

  const std::string &getParticipantConnection() const { return participant_connection_; }

  const daisi::cpps::mrta::model::Ability &getParticipantAbility() const {
    return participant_ability_;
  }

  const order_management::MetricsComposition &getMetricsComposition() {
    return metrics_composition_;
  }

  bool operator>(const BidSubmission &other) const {
    // if (metrics_composition_ != other.metrics_composition_) {
    //   return metrics_composition_ > other.metrics_composition_;
    // }

    // if metrics are the same, it does not matter whether which one is selected
    // but for comparability a unique ordering is necessary

    // if abilities are unequal, ability can be used for ordering
    if (!equalAbility(participant_ability_, other.participant_ability_)) {
      return lessAbility(participant_ability_, other.participant_ability_);
    }

    // if abilit is equal, at least the connection strings are different
    return participant_connection_ > other.participant_connection_;
  }

  SERIALIZE(task_uuid_, participant_connection_, participant_ability_, metrics_composition_);

private:
  std::string task_uuid_;

  std::string participant_connection_;

  daisi::cpps::mrta::model::Ability participant_ability_;

  order_management::MetricsComposition metrics_composition_;
};

}  // namespace daisi::cpps::logical

#endif
