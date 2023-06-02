// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#ifndef MINHTON_LOGGING_LOGGING_H_
#define MINHTON_LOGGING_LOGGING_H_

#include "minhton/logging/logger.h"

#define LOG_CRITICAL(M) access_->logger.logCritical(M)
#define LOG_WARNING(M) access_->logger.logWarning(M)
#define LOG_INFO(M) access_->logger.logInfo(M)
#define LOG_DEBUG(M) access_->logger.logDebug(M)

#define LOG_NODE(n)                                    \
  (access_->logger.logNode(minhton::LoggerInfoAddNode{ \
      n.getPeerInfo().getUuid(), n.getLevel(), n.getNumber(), n.getFanout(), n.isValidPeer()}))

#define LOG_EVENT(event_type, event_id) \
  (access_->logger.logEvent(minhton::LoggerInfoAddEvent{0, event_type, event_id}))

#define LOG_SEARCH_EXACT(status, event_id, sender, target, hop)                      \
  (access_->logger.logSearchExactTest(minhton::LoggerInfoSearchExact{                \
      0, event_id, status, sender.getLevel(), sender.getNumber(), target.getLevel(), \
      target.getNumber(), hop.getLevel(), hop.getNumber()}))

#define LOG_CONTENT(n, content_status, attribute_name, content_type, content_text) \
  (access_->logger.logContent(minhton::LoggerInfoAddContent{                       \
      0, n.getPeerInfo().getUuid(), content_status, attribute_name, content_type, content_text}))

#define LOG_FIND_QUERY(event_id, initiator, find_query)                                         \
  (access_->logger.logFindQuery(minhton::LoggerInfoAddFindQuery{                                \
      0, event_id, initiator.getPeerInfo().getUuid(), find_query.getInquireUnknownAttributes(), \
      find_query.getInquireOutdatedAttributes(), find_query.serializeBooleanExpression()}))

#define LOG_FIND_QUERY_RESULT(event_id, result_node) \
  (access_->logger.logFindQueryResult(               \
      minhton::LoggerInfoAddFindQueryResult{0, event_id, result_node.getPeerInfo().getUuid()}))

#endif
