// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#ifndef MINHTON_ALGORITHMS_ESEARCH_FUZZY_VALUE_H_
#define MINHTON_ALGORITHMS_ESEARCH_FUZZY_VALUE_H_

#include <ostream>

namespace minhton {

class FuzzyValue {
public:
  static FuzzyValue createFalse() { return FuzzyValue(kFalse); }
  static FuzzyValue createUndecided() { return FuzzyValue(kUndecided); }
  static FuzzyValue createTrue() { return FuzzyValue(kTrue); }

  bool isFalse() const { return value_ == kFalse; }

  bool isUndecided() const { return value_ == kUndecided; }

  bool isTrue() const { return value_ == kTrue; }

  friend std::ostream &operator<<(std::ostream &os, const FuzzyValue &fuzz) {
    if (fuzz.isFalse()) {
      os << "false";
      return os;
    }

    if (fuzz.isUndecided()) {
      os << "undecided";
      return os;
    }

    if (fuzz.isTrue()) {
      os << "true";
      return os;
    }

    os << "invalid";
    return os;
  }

  FuzzyValue operator!() const { return FuzzyValue(kTrue - value_); }

  FuzzyValue operator&&(FuzzyValue other) const {
    return FuzzyValue(std::min(this->value_, other.value_));
  }

  FuzzyValue operator||(FuzzyValue other) const {
    return FuzzyValue(std::max(this->value_, other.value_));
  }

  bool operator==(FuzzyValue other) const { return this->value_ == other.value_; }

  bool operator!=(FuzzyValue other) const { return this->value_ != other.value_; }

private:
  static const short kFalse = 0;
  static const short kUndecided = 1;
  static const short kTrue = 2;

  explicit FuzzyValue(short value) : value_(value){};

  short value_;
};

}  // namespace minhton

#endif
