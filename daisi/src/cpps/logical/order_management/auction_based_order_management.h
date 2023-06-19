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

#ifndef DAISI_CPPS_LOGICAL_ORDER_MANAGEMENT_AUCTION_BASED_ORDER_MANAGEMENT_H_
#define DAISI_CPPS_LOGICAL_ORDER_MANAGEMENT_AUCTION_BASED_ORDER_MANAGEMENT_H_

#include "metrics_composition.h"
#include "order_management.h"

namespace daisi::cpps::logical {

class AuctionBasedOrderManagement : public OrderManagement {
public:
  struct InsertionPoint {};

  AuctionBasedOrderManagement(const AmrDescription &amr_description,
                                       const Topology &topology, const daisi::util::Pose &pose)
      : OrderManagement(amr_description, topology, pose) {}

  virtual ~AuctionBasedOrderManagement() = default;

  virtual bool addTask(const daisi::material_flow::Task &task,
                       std::shared_ptr<InsertionPoint> insertion_point = nullptr) = 0;

  virtual bool addTask(const daisi::material_flow::Task &task) final {
    return addTask(task, nullptr);
  }

  virtual std::pair<MetricsComposition, std::shared_ptr<InsertionPoint>>
  getLatestCalculatedInsertionInfo() const = 0;
};

}  // namespace daisi::cpps::logical

#endif
