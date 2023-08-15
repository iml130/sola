// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#ifndef SOLA_MANAGEMENT_OVERLAY_MINHTON_H_
#define SOLA_MANAGEMENT_OVERLAY_MINHTON_H_

#include <future>
#include <memory>
#include <vector>

#include "minhton/core/definitions.h"
#include "minhton/core/minhton.h"
#include "minhton/logging/logger.h"
#include "minhton/utils/config_node.h"
#include "storage/storage.h"

namespace sola {
class ManagementOverlayMinhton final : public Storage {
public:
  using Logger = minhton::Logger::LoggerPtr;
  using Config = minhton::ConfigNode;

  explicit ManagementOverlayMinhton(const minhton::ConfigNode &config);

  void insert(std::vector<Entry> entries) override;
  std::future<minhton::FindResult> find(Request r) override;
  void stop() override;
  bool canStop() const override;

  bool isRunning() const { return node_->getState() == minhton::State::kConnected; }

private:
  std::unique_ptr<minhton::Minhton> node_;
};

}  // namespace sola

#endif
