// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#include "minhton/core/node.h"

#include <algorithm>
#include <cassert>
#include <condition_variable>
#include <mutex>

#include "minhton/core/access_container.h"
#include "minhton/core/node_info.h"
#include "minhton/exception/fsm_exception.h"
#include "minhton/exception/invalid_message_exception.h"
#include "minhton/utils/serializer_cereal.h"
#include "minhton/utils/timestamp_helper.h"
#include "minhton/utils/uuid.h"

namespace minhton {

MinhtonNode::MinhtonNode(const ConfigNode &config_node, NeighborCallbackFct fct, bool auto_start)
    : network_facade_([&](const MessageVariant &msg) { recv(msg); }, config_node.getOwnIP()),
      neighbor_update_fct_(fct) {
  setConfig(config_node);

  if (fct) {
    addCallback(fct);
  }

  start(config_node.getJoinInfo(), auto_start);
}

void MinhtonNode::addCallback(NeighborCallbackFct fct) {
  routing_info_->addNeighborChangeSubscription(
      [&](const minhton::NodeInfo &new_node, NeighborRelationship relationship,
          const minhton::NodeInfo &old_node, uint16_t position) {
        neighbor_update_fct_({new_node, relationship, old_node.getAddress(), old_node.getPort(),
                              position, getNodeInfo()});
      });
}

void MinhtonNode::setConfig(const minhton::ConfigNode &config) {
  // TODO Should only be called from constructor
  minhton::NodeInfo self_node_info;

  self_node_info.setAddress(network_facade_.getIP());
  self_node_info.setPort(network_facade_.getPort());
  verbose_ = config.getVerbose();

  auto acc = std::make_shared<AccessContainer>();

  Logger log;

  // TODO Temporary UUID generator. Should go to Node/L/P/NodeInfo
  minhton::UUID uuid = minhton::generateUUID();
  for (const Logger::LoggerPtr &logger : config.getLogger()) {
    logger->setApplicationUUID(uuid);
    log.addLogger(logger);
  }

  log.setLogLevel(logLevelFromString(config.getLogLevel()));

  acc->logger = std::move(log);
  access_ = acc;

  acc->send = [this](const MessageVariant &msg) { send(msg); };
  acc->recv = [this](const MessageVariant &msg) { recv(msg); };

  acc->set_timeout = [this](TimeoutType type) -> void { this->setTimeout(type); };

  acc->cancel_timeout = [this](TimeoutType timeoutType) { watchdog_.cancelJob(timeoutType); };

  acc->get_fsm_state = [this]() -> FSMState { return this->getFsmState(); };
  acc->set_new_fsm = [this](minhton::FiniteStateMachine new_fsm) { fsm_ = new_fsm; };

  acc->get_timestamp = []() { return minhton::getCurrentTime(); };

  acc->get_timeout_length = [this](minhton::TimeoutType timeout_type) {
    return getTimeoutLength(timeout_type);
  };

  // this is the receiver handler; messages needs to be forwarded to "recv"-Method
  if (config.isRoot()) {
    // if root, basically wait for a package
    self_node_info.setPosition(minhton::LogicalNodeInfo(0, 0, config.getFanout()));

    // the fsm of root initially always needs to be connected
    // otherwise no other node could connect
    fsm_ = FiniteStateMachine(FSMState::kConnected);

  } else {
    fsm_ = FiniteStateMachine(FSMState::kIdle);
  }

  routing_info_ = std::make_shared<RoutingInformation>(self_node_info, access_->logger);

  acc->routing_info = routing_info_;
  procedure_info_ = std::make_shared<minhton::ProcedureInfo>();
  acc->procedure_info = procedure_info_;

  timeout_lengths_container_ = config.getTimeoutLengthsContainer();
  logic_ = std::make_unique<LogicContainer>(acc, config.getAlgorithmTypesContainer());
}

void MinhtonNode::start(const JoinInfo &info, bool auto_connect) {
  access_->logger.logPhysicalNodeInfo({network_facade_.getIP(), network_facade_.getPort()});
  const auto &l_node_info = getNodeInfo().getLogicalNodeInfo();
  access_->logger.logNode({l_node_info.getUuid(), l_node_info.getLevel(), l_node_info.getNumber(),
                           l_node_info.getFanout(), l_node_info.isInitialized()});
  access_->logger.logNodeUninit({l_node_info.getUuid(), 0});

  if (!getNodeInfo().getLogicalNodeInfo().isRoot() && auto_connect) {
    // now its time to connect to a "node" and to start a join procedure
    switch (info.join_mode) {
      case JoinInfo::kIp:
        if (info.ip.empty() || info.port == 0) {
          throw std::runtime_error("Invalid data for IP JoinMode");
        }
        processSignal(minhton::Signal::joinNetworkViaAddress(info.ip, info.port));
        break;
      case JoinInfo::kDiscovery:
        processSignal(minhton::Signal::joinNetworkViaBootstrap());
        break;
      case JoinInfo::kNone:
        throw std::runtime_error("Cannot start non-root node with JoinMode None");
      default:
        throw std::runtime_error("Encountered not implemented JoinMode");
    }
  }

  if (getNodeInfo().getLogicalNodeInfo().isRoot()) {
    if (info.join_mode != JoinInfo::kNone) {
      throw std::runtime_error("Cannot start root with JoinMode other that None");
    }
    routing_info_->setNodeStatus(minhton::NodeStatus::kRunning, 0);
  }
}

minhton::NodeInfo MinhtonNode::getNodeInfo() const { return routing_info_->getSelfNodeInfo(); }

void MinhtonNode::processSignal(Signal signal) {
  if (signal.signal_type == SignalType::kLeaveNetwork) {
    TimeoutType type = minhton::kSelfDepartureRetry;
    watchdog_.addJob(
        [this]() {
          if (fsm_.current_state() != minhton::kIdle) {
            processSignal(Signal::leaveNetwork());
          }
        },
        1000, type);
    if (fsm_.current_state() == kConnectedReplacing) {
      return;
    }
    // TODO Not always working (rightmost node in full level?)
    signal.can_leave_position = logic_->canLeaveWithoutReplacement();
  }

  FSMState state = getFsmState();
  fsm_.process_event(signal);

  if (!fsm_.isActionValid()) {
    return;
  }

  if (fsm_.current_state() == FSMState::kErrorState) {
    throw FSMException(state, signal, "");
  }

  logic_->processSignal(signal);
  //  Currently the FSM is msg-driven, but it should algorithm driven?
}

void MinhtonNode::send(const MessageVariant &msg) {
  prepareSending(msg);
  network_facade_.send(msg);
}

void MinhtonNode::prepareSending(const MessageVariant &msg_variant) {
  std::visit(
      [this](auto &&msg) {
        const MinhtonMessageHeader header = msg.getHeader();
        LOG_INFO("send " + getMessageTypeString(header.getMessageType()) + " to " +
                 header.getTarget().getString());

        if (!msg.getHeader().validate()) {
          // Don't accept messages with an invalid header
          throw InvalidMessageException(header);
        }

        FSMState state = getFsmState();
        SendMessage fsm_event{header.getMessageType()};
        fsm_.process_event(fsm_event);

        if (!fsm_.isActionValid()) {
          throw FSMException(state, fsm_event, "Invalid Action");
        }

        if (fsm_.current_state() == FSMState::kErrorState) {
          throw FSMException(state, fsm_event, "");
        }

        access_->logger.logTraffic(header.getMessageLoggingInfo(false));
      },
      msg_variant);
}

void MinhtonNode::stop() {
  if (fsm_.current_state() == kConnected) {
    processSignal(minhton::Signal::leaveNetwork());
  }
}

void MinhtonNode::recv(const MessageVariant &msg) {
  prepareReceiving(msg);
  logic_->process(msg);
}

void MinhtonNode::prepareReceiving(const MessageVariant &msg_variant) {
  std::visit(
      [this, msg_variant](auto &&msg) {
        const MinhtonMessageHeader header = msg.getHeader();
        LOG_INFO("recv " + getMessageTypeString(header.getMessageType()) + " from " +
                 header.getSender().getString());

        FSMState state = getFsmState();
        ReceiveMessage fsm_event{header.getMessageType()};

        // If a find_replacement msg reaches the node that wants to leave and is chosen as the
        // replacement node, this means it can leave the network on its own
        if (header.getMessageType() == kFindReplacement) {
          auto msg_find_repl = std::get<MessageFindReplacement>(msg_variant);
          if (msg_find_repl.getNodeToReplace() == getNodeInfo() &&
              msg_find_repl.getSearchProgress() == SearchProgress::kReplacementNode) {
            fsm_event.does_not_need_replacement = true;
          }
        }

        if (fsm_.current_state() == kIdle && replacing_node_.isInitialized()) {
          // Drop specific messages
          if (header.getMessageType() == kFindReplacement || header.getMessageType() == kJoin) {
            return;
          }

          // Forward messages to old node
          if (header.getMessageType() == kAttributeInquiryRequest) {
            auto old_attribute_request = std::get<MessageAttributeInquiryRequest>(msg_variant);
            auto new_header = old_attribute_request.getHeader();
            new_header.setTarget(replacing_node_);
            MessageAttributeInquiryRequest attribute_request(
                new_header, old_attribute_request.getInquireAll(),
                old_attribute_request.getMissingKeys());
            send(attribute_request);
          }

          else if (header.getMessageType() == kSignOffParentRequest) {
            auto old_signoff_parent_request = std::get<MessageSignoffParentRequest>(msg_variant);
            auto new_header = old_signoff_parent_request.getHeader();
            new_header.setTarget(replacing_node_);
            MessageSignoffParentRequest signoff_parent_request(new_header);
            send(signoff_parent_request);
          }

          else if (header.getMessageType() == kAttributeInquiryAnswer) {
            auto old_attribute_answer = std::get<MessageAttributeInquiryAnswer>(msg_variant);
            auto new_header = old_attribute_answer.getHeader();
            new_header.setTarget(replacing_node_);
            MessageAttributeInquiryAnswer attribute_answer(
                new_header, old_attribute_answer.getInquiredNode(),
                old_attribute_answer.getAttributeValuesAndTypes(),
                old_attribute_answer.getRemovedAttributeKeys());
            send(attribute_answer);
          } else {
            throw std::runtime_error("forwarding this message type not handled yet");
          }

          return;
        }

        if (header.getMessageType() == kFindReplacement &&
            header.getTarget().getLogicalNodeInfo() !=
                routing_info_->getSelfNodeInfo().getLogicalNodeInfo()) {
          // Message was forwarded to us because with knowledge of the sender we are nearer to the
          // successor. But the sender had outdated information about our position. Therefore
          // discard the message for now.
          return;
        }

        if (fsm_.current_state() == kIdle) {
          // We should forward the message but were last
          return;
        }

        fsm_.process_event(fsm_event);

        if (!fsm_.isActionValid()) {
          throw FSMException(state, fsm_event, "Invalid Action");
        }

        if (fsm_.current_state() == FSMState::kErrorState) {
          throw FSMException(state, fsm_event, "");
        }

        if (header.getMessageType() != MessageType::kEmpty) {
          access_->logger.logTraffic(header.getMessageLoggingInfo(true));
        }

        if (header.getMessageType() == MessageType::kReplacementNack) {
          watchdog_.cancelJob(minhton::kReplacementOfferResponseTimeout);
          watchdog_.cancelJob(minhton::kSelfDepartureRetry);

          TimeoutType type = minhton::kSelfDepartureRetry;
          watchdog_.addJob([this]() { processSignal(Signal::leaveNetwork()); }, 1000, type);

          return;
        }

        if (header.getMessageType() == MessageType::kReplacementOffer) {
          replacing_node_ = header.getSender();
          replacing_node_.setPosition(routing_info_->getSelfNodeInfo().getLogicalNodeInfo());
        }

        // TODO WORKAROUND TO SWITCH BACK TO CONNECTED IF SIGNOFF PARENT WAS NEGATIVE
        if (header.getMessageType() == MessageType::kSignOffParentAnswer) {
          auto a = std::get<MessageSignoffParentAnswer>(msg_variant);
          if (!a.wasSuccessful()) {
            fsm_ = FiniteStateMachine(kConnected);
          }
        }
      },
      msg_variant);
}

void MinhtonNode::triggerTimeout(const TimeoutType &timeout_type) {
  auto timeout_event = Timeout{timeout_type, false};
  if (timeout_type == TimeoutType::kBootstrapResponseTimeout) {
    timeout_event.valid_bootstrap_response = logic_->isBootstrapResponseValid();
  }

  FSMState state = getFsmState();
  fsm_.process_event(timeout_event);

  if (!fsm_.isActionValid()) {
    throw FSMException(state, timeout_event, "Invalid Action");
  }

  if (fsm_.current_state() == FSMState::kErrorState) {
    throw FSMException(state, timeout_event, "");
  }

  if (timeout_type == kJoinAcceptResponseTimeout) {
    // TODO Should be handled in join algorithm
    assert(fsm_.current_state() == FSMState::kJoinFailed);
    TimeoutType type = kJoinRetry;
    watchdog_.addJob(
        [this]() {
          processSignal(minhton::Signal::joinNetworkViaAddress("255.255.255.255", 9999));
        },
        getTimeoutLength(type), type);
    return;
  }

  logic_->processTimeout(timeout_event);
}

void MinhtonNode::setTimeout(TimeoutType timeout_type) {
  uint16_t timeout_length = getTimeoutLength(timeout_type);
  // not setting a timeout, iff the timeout length of that timeout type
  // is set to 0

  if (timeout_length > 0) {
    watchdog_.addJob([&, timeout_type]() { triggerTimeout(timeout_type); }, timeout_length,
                     timeout_type);
  }
}

uint16_t MinhtonNode::getTimeoutLength(const TimeoutType &timeout_type) const {
  switch (timeout_type) {
    case minhton::TimeoutType::kBootstrapResponseTimeout:
      return timeout_lengths_container_.bootstrap_response;
    case minhton::TimeoutType::kJoinAcceptResponseTimeout:
      return timeout_lengths_container_.join_response;
    case minhton::TimeoutType::kJoinAcceptAckResponseTimeout:
      return timeout_lengths_container_.join_accept_ack_response;
    case minhton::TimeoutType::kReplacementOfferResponseTimeout:
      return timeout_lengths_container_.replacement_offer_response;
    case minhton::TimeoutType::kReplacementAckResponseTimeout:
      return timeout_lengths_container_.replacement_ack_response;
    case minhton::TimeoutType::kDsnAggregationTimeout:
      return timeout_lengths_container_.dsn_aggregation;
    case minhton::TimeoutType::kInquiryAggregationTimeout:
    case minhton::TimeoutType::kSelfDepartureRetry:
      return timeout_lengths_container_.inquiry_aggregation;
    case minhton::TimeoutType::kJoinRetry:
      return 1000;
  }
  return 0;
}

FSMState MinhtonNode::getFsmState() { return FSMState{fsm_.current_state()}; }

void MinhtonNode::initFSM(minhton::FSMState &init_state) {
  fsm_ = minhton::FiniteStateMachine(init_state);
}

/*uint32_t MinhtonNode::sendMulticast(const MessageVariant &msg) {
  return network_interface_->sendMulticast(msg);
}*/

std::shared_ptr<minhton::RoutingInformation> MinhtonNode::getRoutingInformation() {
  return routing_info_;
}

void MinhtonNode::insert(const std::vector<Entry> &data) { logic_->localInsert(data); }

void MinhtonNode::update(const std::vector<Entry> &data) { logic_->localUpdate(data); }

void MinhtonNode::remove(const std::vector<std::string> &keys) { logic_->localRemove(keys); }

std::future<FindResult> MinhtonNode::find(const FindQuery &query) { return logic_->find(query); }

std::future<FindResult> MinhtonNode::find([[maybe_unused]] const std::string &query) {
  // TODO: parsing string into find query
  // query: "select * from ..."

  FindQuery find_query;
  return logic_->find(find_query);
}

void MinhtonNode::performSearchExactTest(const NodeInfo &destination,
                                         std::shared_ptr<minhton::MessageSEVariant> query) {
  uint64_t event_id =
      std::visit([this](auto &&msg) -> uint64_t { return msg.getHeader().getEventId(); }, *query);
  LOG_SEARCH_EXACT(minhton::SearchExactTestEntryTypes::kStart, event_id, getNodeInfo(), destination,
                   minhton::NodeInfo());
  logic_->performSearchExactTest(destination, query);
}

}  // namespace minhton
