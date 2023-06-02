// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#ifndef MINHTON_ALGORITHMS_ALGORITHM_INTERFACE_H_
#define MINHTON_ALGORITHMS_ALGORITHM_INTERFACE_H_

#include <memory>

#include "minhton/core/access_container.h"
#include "minhton/message/message.h"
#include "minhton/message/types_all.h"

namespace minhton {

class AlgorithmInterface {
public:
  explicit AlgorithmInterface(std::shared_ptr<AccessContainer> access) : access_(access){};

  virtual ~AlgorithmInterface(){};

  virtual void process(const MessageVariant &msg) = 0;

protected:
  std::shared_ptr<AccessContainer> access_;
  void send(const MessageVariant &msg) { access_->send(msg); }

  std::shared_ptr<RoutingInformation> getRoutingInfo() const { return access_->routing_info; }

  NodeInfo getSelfNodeInfo() const { return getRoutingInfo()->getSelfNodeInfo(); }
};

}  // namespace minhton

#endif
