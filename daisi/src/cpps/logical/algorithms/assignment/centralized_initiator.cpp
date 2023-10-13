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

#include "centralized_initiator.h"

namespace daisi::cpps::logical {
CentralizedInitiator::CentralizedInitiator(daisi::cpps::common::CppsCommunicatorPtr communicator,
                                           std::shared_ptr<CppsLoggerNs3> logger)
    : AssignmentInitiator(communicator, logger) {
  // create request to gain information about all active AMR's
  sola::Request amr_request;
  amr_request.all = true;
  amr_request.permissive = true;
  amr_request.request = "(servicetype == transport)";

  // send request
  amr_find_result_ = communicator->sola.findService(amr_request);
};

void CentralizedInitiator::addMaterialFlow(
    std::shared_ptr<material_flow::MFDLScheduler> scheduler) {
  if (!preparation_finished_) {
    // first material flow, finish preparation first
    if (amr_find_result_.valid()) {
      readAmrRequestFuture();
    } else {
      throw std::runtime_error("Future is not ready yet but required.");
    }
  }

  material_flows_.push_back(*scheduler);
  distributeMFTasks(material_flows_.size() - 1, false);
}

void CentralizedInitiator::readAmrRequestFuture() {
  // iterate through all AMRs found
  for (const auto &result : amr_find_result_.get()) {
    ParticipantInfo info;
    float max_payload = -1;
    amr::LoadCarrier loadcarrier = amr::LoadCarrier(amr::LoadCarrier::kNoLoadCarrierType);

    for (const auto &entry : result) {
      // retreive all relevant information for ParticipantInfo
      std::string key = std::get<0>(entry);
      auto value = std::get<1>(entry);
      if (key == "endpoint") {
        info.connection_string = std::get<std::string>(value);
      } else if (key == "loadcarriertype") {
        loadcarrier = amr::LoadCarrier(std::get<std::string>(value));
      } else if (key == "maxpayload") {
        max_payload = std::get<float>(value);
      }

      if (max_payload != -1 && loadcarrier.isValid()) {
        // all information to initialize ability were obtained
        amr::AmrStaticAbility ability = amr::AmrStaticAbility(loadcarrier, max_payload);
        info.ability = ability;
      }
    }
    if (!info.isValid()) {
      // something is still uninitialized
      throw std::runtime_error("The AMR's ability and connection string should be known.");
    }

    // algorithm-specific storage of info
    storeParticipant(info);
  }

  preparation_finished_ = true;
}

}  // namespace daisi::cpps::logical
