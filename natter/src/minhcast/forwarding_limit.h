// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#ifndef NATTER_FORWARDING_LIMIT_H_
#define NATTER_FORWARDING_LIMIT_H_

#include <cstdint>
#include <limits>
#include <tuple>

#include "core/natter_check.h"
#include "natter/minhcast_level_number.h"
#include "solanet/serializer/serialize.h"

namespace natter::minhcast {
class ForwardingLimit {
public:
  ForwardingLimit(uint32_t forwarding_up, uint32_t forwarding_down)
      : forwarding_limits_(forwarding_up, forwarding_down) {
    NATTER_CHECK(forwarding_up <= forwarding_down, "Forwarding up is greater than forwarding down");
  }

  ForwardingLimit() : forwarding_limits_(0, std::numeric_limits<Level>::max()) {}

  uint32_t up() const { return std::get<0>(forwarding_limits_); }
  uint32_t down() const { return std::get<1>(forwarding_limits_); }

  SERIALIZE(forwarding_limits_);

private:
  std::tuple<Level, Level> forwarding_limits_;
};
}  // namespace natter::minhcast

#endif  // DAISI_FORWARDING_LIMIT_H_
