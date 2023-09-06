// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#ifndef MINHTON_CORE_NODE_H_
#define MINHTON_CORE_NODE_H_

#include <memory.h>

#include <chrono>
#include <future>
#include <thread>
#include <vector>

#include "minhton/algorithms/esearch/find_query.h"
#include "minhton/core/connection_info.h"
#include "minhton/core/definitions.h"
#include "minhton/core/logic_container.h"
#include "minhton/core/node_info.h"
#include "minhton/core/routing_information.h"
#include "minhton/core/watchdog.h"
#include "minhton/logging/logging.h"
#include "minhton/message/message.h"
#include "minhton/message/types_all.h"
#include "minhton/network/network_facade.h"
#include "minhton/utils/config_node.h"
#include "minhton/utils/fsm.h"
#include "minhton/utils/procedure_info.h"

namespace minhton {

// Low level
class MinhtonNode {
public:
  /// Constructor
  explicit MinhtonNode(const ConfigNode &config_node,
                       NeighborCallbackFct fct = NeighborCallbackFct{}, bool auto_start = true);

  /// Giving the node every necessary information about itself to start.
  ///
  /// We simply set the given address, port and name.
  ///
  /// If we are root, \p config.isRoot() is true and we will only initialize
  /// our \p self_node_info with the position 0:0 and initialize our routing information.
  ///
  /// If we are not root, we only initialize \p self_node_info without further information.
  ///
  /// Typical usage:
  /// \code
  ///   node->setConfig(config);
  /// \endcode
  ///
  /// \param config with all necessary information
  ///
  /// \returns void
  void setConfig(const minhton::ConfigNode &config);

  void stop();

  /// Destructor
  virtual ~MinhtonNode() = default;

  /// \returns the node information of this node
  minhton::NodeInfo getNodeInfo() const;

  void processSignal(Signal signal);

  void initFSM(minhton::FSMState &init_state);

  FSMState getFsmState();

  std::shared_ptr<minhton::RoutingInformation> getRoutingInformation();

  void insert(const std::vector<Entry> &data);
  void update(const std::vector<Entry> &data);
  void remove(const std::vector<std::string> &keys);

  std::future<FindResult> find(const std::string &query);  // for high level api
  std::future<FindResult> find(const FindQuery &query);    // for testing

  // if we expose this, we dont need to expose the logic container!
  void performSearchExactTest(const minhton::NodeInfo &destination,
                              std::shared_ptr<minhton::MessageSEVariant> query);

  // For debugging and testing purposes
  std::shared_ptr<AccessContainer> getAccessContainer() const { return access_; };

private:
  /// Sending a message via the network interface.
  ///
  /// We simply call the sendMessage method of the network interface.
  ///
  /// Typical usage:
  /// \code
  ///   this->send(outgoing_message);
  /// \endcode
  ///
  /// \param msg message which we want to send
  ///
  void send(const MessageVariant &msg);

  /// Before the network facade can send the message, we call this method to check if the message is
  /// valid at the current state. This requires an std::visit call since we need to extract the
  /// header of the message for a further analysis. The inner function does a validation of the
  /// message header and the necessary transition of the FSM. Additionally, this methods informs the
  /// logger about the outgoing message.
  ///
  /// \param msg_variant Message to check
  void prepareSending(const MessageVariant &msg_variant);

  /// This method is called if a timeout expires.
  ///
  /// The FSM is making a transition here.
  ///
  /// \param timeout_type
  void triggerTimeout(const TimeoutType &timeout_type);

  /// When we receive a message via the network interface,
  /// this method will be called.
  ///
  /// We simply choose the right process-method for the corresponding message type.
  /// E.g. a join message will get processed by the processJoin method.
  ///
  /// The network interface has access to this method via a callback.
  ///
  /// Therefore this method will also be called if we receive a search exact message
  /// and we are the destination node.
  ///
  /// Typical usage:
  /// \code
  ///   this->recv(incoming_message);
  /// \endcode
  ///
  /// \param msg message which we want to process
  ///
  void recv(const MessageVariant &msg);

  /// After we receive a message from the network facade and before the message is processed, we
  /// call this method to check if the message is valid at the current state. This requires an
  /// std::visit call since we need to extract the header of the message for a further analysis. The
  /// majority of the inner function are workarounds, since in some cases messages need to be
  /// handled differently, i.e., redirected to another node, dropped or they must trigger a
  /// divergent FSM event.
  ///
  /// \param msg_variant Message to check
  ///
  /// \returns true if the message should be processed, false if not
  bool prepareReceiving(const MessageVariant &msg_variant);

  /// After all relevant information have been set, we can start the node.
  ///
  /// If we are the root node or if we do not want to \p auto_connect to the network
  /// immediately, we will only start the server via the network interface.
  ///
  /// If we are not the root node and want to \p auto_connect to the network,
  /// then a join message will be sent immeditately to the remote address.
  ///
  /// Typical usage:
  /// \code
  ///   node->start(config.getRemoteAddress(), config.getRemotePort(), true);
  /// \endcode
  ///
  /// \param info info how we should join the network
  /// \param auto_connect if we want to connect to the network immediately
  ///
  /// \returns void
  void start(const JoinInfo &info, bool auto_connect = false);

  /// Container holding functions to access different parts of MINHTON
  std::shared_ptr<AccessContainer> access_;

  /// To send or receive data via the network layer.
  NetworkFacade network_facade_;

  /// Class to save information about procedures which have not been completed yet
  std::shared_ptr<minhton::ProcedureInfo> procedure_info_;

  /// For logging.
  bool verbose_ = true;

  /// timeout lengths in milliseconds used by FSM
  TimeoutLengthsContainer timeout_lengths_container_{};

  /// Helper method to get the timeout length depending on the type
  uint16_t getTimeoutLength(const TimeoutType &timeout_type) const;

  /// Setting a timeout with the given type.
  ///
  /// Only one timeout can be set at any given time.
  ///
  /// \param timeout_type to indicate length and the workflow in triggerTimeout
  void setTimeout(TimeoutType timeout_type);

  minhton::core::WatchDog watchdog_;

  /// Contains all routing information about nodes we have connections to,
  /// including our own node information.
  std::shared_ptr<minhton::RoutingInformation> routing_info_;

  /// Finite State Machine to control workflow
  ///
  /// e.g. not letting a node which is in the process of accepting a new child
  /// accept another child simultaneously.
  minhton::FiniteStateMachine fsm_;

  std::unique_ptr<LogicContainer> logic_;

  NeighborCallbackFct neighbor_update_fct_;

  NodeInfo replacing_node_;
};
}  // namespace minhton

#endif
