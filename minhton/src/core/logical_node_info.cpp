// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#include "minhton/core/logical_node_info.h"

#include <stdexcept>

#include "minhton/core/constants.h"
#include "minhton/core/routing_calculations.h"
namespace minhton {
/// 0:0 is always a valid position, therefore we can use setFanout to check if the fanout is valid
LogicalNodeInfo::LogicalNodeInfo(uint16_t fanout) { this->setFanout(fanout); }

/// The fanout isn't valid, therefore setPosition
/// will always throw an exception.
/// When setFanout is executed afterwards (which must happen),
/// the position will get verified this way.
/// Therefore we can set the position directly without setPosition
LogicalNodeInfo::LogicalNodeInfo(uint32_t p_level, uint32_t p_number)
    : level_(p_level), number_(p_number) {}

/// The validity of the fanout will get checked in setPosition.
/// There is no need to call setFanout.
LogicalNodeInfo::LogicalNodeInfo(uint32_t p_level, uint32_t p_number, uint16_t p_fanout)
    : fanout_(p_fanout), initialized_(true) {
  this->setPosition(p_level, p_number);
}

LogicalNodeInfo::LogicalNodeInfo() {
  this->level_ = 0;
  this->number_ = 0;
  this->fanout_ = 0;
  this->initialized_ = false;
}

void LogicalNodeInfo::setPosition(uint32_t level, uint32_t number) {
  if (isPositionValid(level, number, this->fanout_)) {
    this->number_ = number;
    this->level_ = level;
    this->uuid_ = generateUUID();  // TODO Make object const
  } else {
    throw std::invalid_argument("Invalid Parameter Level or Number");
  }
}

void LogicalNodeInfo::setPosition(LogicalNodeInfo other) {
  if (isPositionValid(other.level_, other.number_, other.fanout_)) {
    this->level_ = other.level_;
    this->number_ = other.number_;
    this->fanout_ = other.fanout_;
    this->initialized_ = other.initialized_;
    this->uuid_ = other.uuid_;
  } else {
    throw std::invalid_argument("Peer has invalid Position");
  }
}

std::string LogicalNodeInfo::getString() const {
  return std::to_string(this->level_) + ":" + std::to_string(this->number_) +
         " | m=" + std::to_string(this->fanout_) + " | " + minhton::uuidToString(this->uuid_);
}

uint32_t LogicalNodeInfo::getLevel() const { return this->level_; }

uint32_t LogicalNodeInfo::getNumber() const { return this->number_; }

uint16_t LogicalNodeInfo::getFanout() const { return this->fanout_; }

void LogicalNodeInfo::setFanout(uint16_t fanout) {
  if (!isFanoutValid(fanout)) {
    throw std::invalid_argument("Fanout has to be >= " + std::to_string(kFanoutMinimum) +
                                " and <= " + std::to_string(kFanoutMaximum));
  }

  if (isPositionValid(this->level_, this->number_, fanout)) {
    this->fanout_ = fanout;
  } else {
    throw std::invalid_argument("Position with the given Fanout is not valid");
  }
}

bool LogicalNodeInfo::isRoot() const {
  if (this->number_ != 0 || this->level_ != 0) return false;

  if (!this->initialized_ && this->number_ == 0 && this->level_ == 0) return false;

  return true;
}

bool LogicalNodeInfo::isInitialized() const { return this->initialized_; }

bool LogicalNodeInfo::isSameLevel(LogicalNodeInfo other) const {
  // one of them is not initialized
  if (!this->isInitialized() || !other.isInitialized()) {
    return false;
  }

  // comparing levels
  return (this->level_ == other.level_);
}

bool LogicalNodeInfo::isDeeperThan(LogicalNodeInfo other) const {
  // one of them is not initialized
  if (!this->isInitialized() || !other.isInitialized()) {
    return false;
  }

  // comparing our level is deeper than the others
  return (this->level_ > other.level_);
}

bool LogicalNodeInfo::isDeeperThanOrSameLevel(LogicalNodeInfo other) const {
  // one of them is not initialized
  if (!this->isInitialized() || !other.isInitialized()) {
    return false;
  }

  // comparing our level is deeper than the others
  return (this->level_ >= other.level_);
}

bool LogicalNodeInfo::isPrioNode() const {
  return this->isInitialized() && isNodePartOfPrioSet(this->level_, this->number_, this->fanout_);
}

std::string LogicalNodeInfo::getUuid() const { return minhton::uuidToString(this->uuid_); }

minhton::UUID LogicalNodeInfo::getRawUuid() const { return this->uuid_; }

double LogicalNodeInfo::getHorizontalValue() const {
  double value = treeMapper(this->level_, this->number_, this->fanout_, k_TREEMAPPER_ROOT_VALUE);
  return value;
}

bool operator==(const minhton::LogicalNodeInfo &p1, const minhton::LogicalNodeInfo &p2) {
  return (p1.getLevel() == p2.getLevel() && p1.getNumber() == p2.getNumber() &&
          p1.getFanout() == p2.getFanout() && p1.isInitialized() == p2.isInitialized());
}

bool operator!=(const minhton::LogicalNodeInfo &p1, const minhton::LogicalNodeInfo &p2) {
  return !(p1 == p2);
}

bool operator<(const minhton::LogicalNodeInfo &p1, const minhton::LogicalNodeInfo &p2) {
  return p1.getHorizontalValue() < p2.getHorizontalValue();
}

bool operator<=(const minhton::LogicalNodeInfo &p1, const minhton::LogicalNodeInfo &p2) {
  return p1.getHorizontalValue() <= p2.getHorizontalValue();
}

bool operator>(const minhton::LogicalNodeInfo &p1, const minhton::LogicalNodeInfo &p2) {
  return !(p1 <= p2);
}

bool operator>=(const minhton::LogicalNodeInfo &p1, const minhton::LogicalNodeInfo &p2) {
  return !(p1 < p2);
}

}  // namespace minhton
