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

#ifndef DAISI_CPPS_LOGICAL_ORDER_MANAGEMENT_METRICS_COMPOSITION_H_
#define DAISI_CPPS_LOGICAL_ORDER_MANAGEMENT_METRICS_COMPOSITION_H_

#include "metrics.h"
#include "solanet/serializer/serialize.h"

namespace daisi::cpps::order_management {

class MetricsComposition {
public:
  MetricsComposition() : insertion_metrics_set_(false), diff_insertion_metrics_set_(false){};

  explicit MetricsComposition(const Metrics &current_metrics)
      : current_metrics_(current_metrics),
        insertion_metrics_(current_metrics),
        insertion_metrics_set_(false),
        diff_insertion_metrics_set_(false) {}

  MetricsComposition(const Metrics &current_metrics, const Metrics &insertion_metrics,
                     const Metrics &diff_insertion_metrics)
      : current_metrics_(current_metrics),
        insertion_metrics_(insertion_metrics),
        insertion_metrics_set_(true),
        diff_insertion_metrics_(diff_insertion_metrics),
        diff_insertion_metrics_set_(false) {}

  const Metrics &getCurrentMetrics() const { return current_metrics_; }

  void updateCurrentMetrics(const Metrics &updated_current_metrics) {
    current_metrics_ = updated_current_metrics;
  }

  void setDiffInsertionMetrics(const Metrics &diff_insertion_metrics) {
    if (!diff_insertion_metrics_set_) {
      diff_insertion_metrics_ = diff_insertion_metrics;
      diff_insertion_metrics_set_ = true;
    } else {
      throw std::logic_error("diff insertion metrics already set");
    }
  }

  const Metrics &getInsertionMetrics() const { return insertion_metrics_; }

  const Metrics &getDiffInsertionMetrics() const { return diff_insertion_metrics_; }

  const Metrics &getMetricsForAuction() const { return getDiffInsertionMetrics(); }

  bool hasDiffInsertionMetrics() const { return diff_insertion_metrics_set_; }

  void fixInsertionMetrics() {
    if (!insertion_metrics_set_) {
      insertion_metrics_ = current_metrics_;
      insertion_metrics_set_ = true;
    } else {
      throw std::logic_error("insertion metrics already set");
    }
  }

  bool operator<(const MetricsComposition &other) const {
    return getMetricsForAuction() < other.getMetricsForAuction();
  }

  bool operator==(const MetricsComposition &other) const {
    return getMetricsForAuction() == other.getMetricsForAuction();
  }

  MetricsComposition operator-(const MetricsComposition &other) const {
    return MetricsComposition{current_metrics_, insertion_metrics_,
                              current_metrics_ - other.current_metrics_};
  }

  SERIALIZE(current_metrics_, insertion_metrics_, insertion_metrics_set_, diff_insertion_metrics_,
            diff_insertion_metrics_set_);

private:
  // the current costs
  // always updated when something got inserted
  Metrics current_metrics_;

  // metrics when the task got inserted
  Metrics insertion_metrics_;
  bool insertion_metrics_set_;

  // metric difference when the task got inserted
  Metrics diff_insertion_metrics_;
  bool diff_insertion_metrics_set_;
};

}  // namespace daisi::cpps::order_management

#endif
