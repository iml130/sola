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

#ifndef DAISI_PATH_PLANNING_CONSENSUS_CONSENSUS_BASE_H_
#define DAISI_PATH_PLANNING_CONSENSUS_CONSENSUS_BASE_H_

#include <functional>
#include <memory>

#include "path_planning/constants.h"
#include "sola-ns3/sola_ns3_wrapper.h"

namespace daisi::path_planning::consensus {

//! CRTP base class for a consensus algorithm
template <typename T, typename DataContainer> class ConsensusBase {
public:
  explicit ConsensusBase(std::shared_ptr<DataContainer> container)
      : container_(std::move(container)) {}

  ~ConsensusBase() = default;
  void findConsensus(const PointTimePairs &points, double seconds_earliest_start,
                     std::function<void(uint32_t id, double start_time)> success_cb,
                     std::function<void(uint32_t id)> fail_cb) {
    static_cast<T *>(this)->findConsensusImpl(points, seconds_earliest_start, success_cb, fail_cb);
  }

  void recvTopicMessage(const std::string &topic, const std::string &msg) {
    return static_cast<T *>(this)->recvTopicMessageImpl(topic, msg);
  }

protected:
  std::shared_ptr<DataContainer> container_;  //!< Data container which contains data that is needed
                                              //!< in multiple consensus algorithm classes
};
}  // namespace daisi::path_planning::consensus

#endif  // DAISI_PATH_PLANNING_CONSENSUS_CONSENSUS_BASE_H_
