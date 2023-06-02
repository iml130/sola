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

#ifndef DAISI_NEGOTIATION_ITERATED_AUCTION_INITIATOR_PUBSUB_TEPSSI_NS3_H_
#define DAISI_NEGOTIATION_ITERATED_AUCTION_INITIATOR_PUBSUB_TEPSSI_NS3_H_

#include <memory>
#include <queue>

#include "cpps/model/agv_fleet.h"
#include "cpps/negotiation/initiator/iterated_auction_initiator.h"

namespace daisi::cpps {

class IteratedAuctionInitiatorPubsubTePSSI : public IteratedAuctionInitiator {
public:
  IteratedAuctionInitiatorPubsubTePSSI(
      const std::shared_ptr<sola_ns3::SOLAWrapperNs3> &sola,
      const std::shared_ptr<daisi::cpps::CppsLoggerNs3> &logger,
      const std::shared_ptr<MaterialFlowModel> &material_flow_model);
  virtual ~IteratedAuctionInitiatorPubsubTePSSI() = default;

private:
  virtual void prepareInteraction() final;

  virtual void taskAnnoucement() final;

  virtual std::vector<IteratedAuctionInitiator::Winner> selectWinners() final;

  virtual void iterationNotification(
      const std::vector<std::tuple<std::string, Task>> &iteration_info) final;

  virtual void renotifyAboutOpenTasks() final;

  const std::string interaction_topic_;
};
}  // namespace daisi::cpps

#endif
