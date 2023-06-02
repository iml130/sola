// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#ifndef NATTER_NATTER_MINHCAST_H_
#define NATTER_NATTER_MINHCAST_H_

#include <set>

#include "natter/logger_interface.h"
#include "natter/minhcast_level_number.h"
#include "natter/natter.h"
#include "natter/network_info_ipv4.h"

namespace natter::minhcast {

struct MinhcastNodeInfo {
  LevelNumber position;
  NetworkInfoIPv4 network_info = {};
  UUID uuid = {};

  bool operator<(const MinhcastNodeInfo &other) const {
    // Only comparing position. All information for Minhcast is based on node position
    return position < other.position;
  }
};

// CRTP pattern
class NatterMinhcast : public Natter<NatterMinhcast, MinhcastNodeInfo> {
public:
  /**
   * Create natter instance
   * @param recv_callback function will be called when message arrives for this instance
   * @param missing_callback NOT YET USED FOR MINHCAST
   */
  NatterMinhcast(MsgReceiveFct recv_callback, MsgMissingFct missing_callback);

  /**
   * Create natter instance
   * @param recv_callback function will be called when message arrives for this instance
   * @param missing_callback NOT YET USED FOR MINHCAST
   * @param logger logger
   * @param node_uuid uuid for created instance
   */
  NatterMinhcast(MsgReceiveFct recv_callback, MsgMissingFct missing_callback,
                 const std::vector<logging::LoggerPtr> &logger, UUID node_uuid);

  /**
   * Create natter instance with random uuid
   * @param recv_callback function will be called when message arrives for this instance
   * @param missing_callback NOT YET USED FOR MINHCAST
   * @param logger logger
   */
  NatterMinhcast(MsgReceiveFct recv_callback, MsgMissingFct missing_callback,
                 const std::vector<logging::LoggerPtr> &logger);

  // Disallow copy/move
  NatterMinhcast(const NatterMinhcast &) = delete;
  NatterMinhcast &operator=(const NatterMinhcast &) = delete;
  NatterMinhcast(NatterMinhcast &&) = delete;
  NatterMinhcast &operator=(NatterMinhcast &&) = delete;

  ~NatterMinhcast();

private:
  DECLARE_CRTP_METHODS
};

}  // namespace natter::minhcast

#endif  // NATTER_NATTER_MINHCAST_H_
