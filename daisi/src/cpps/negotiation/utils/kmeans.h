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

#ifndef DAISI_NEGOTIATION_UTILS_KMEANS_H_
#define DAISI_NEGOTIATION_UTILS_KMEANS_H_

#include <math.h>

#include <cassert>
#include <vector>

namespace daisi::cpps {

std::vector<std::vector<std::pair<std::string, double>>> kmeans(
    const int &k, std::vector<std::pair<std::string, double>> &data) {
  double max_distance = 1000000;
  assert(data.size() >= k);

  // initializing centroids
  std::vector<double> centroids;
  if (k == 2) {
    std::vector<double> values;
    std::transform(data.begin(), data.end(), std::back_inserter(values),
                   [](const std::pair<std::string, double> &p) { return std::get<1>(p); });
    centroids.push_back(*std::min_element(values.begin(), values.end()));
    centroids.push_back(*std::max_element(values.begin(), values.end()));
  }

  for (int c = 0; c < k; c++) {
    centroids.push_back(std::get<1>(data[c]));
  }

  std::vector<std::vector<std::pair<std::string, double>>> cluster_data;

  double total_avg_distance = max_distance;
  for (int iteration = 0; iteration < 10; iteration++) {
    cluster_data.clear();
    cluster_data.resize(k);

    // finding nearest centroid
    for (size_t point = 0; point < data.size(); point++) {
      // find nearest centroid
      int nearest_cluster = -1;
      double nearest_distance = max_distance;
      for (int cluster = 0; cluster < k; cluster++) {
        double current_distance = std::abs(std::get<1>(data[point]) - centroids[cluster]);
        if (current_distance < nearest_distance) {
          nearest_cluster = cluster;
          nearest_distance = current_distance;
        }
      }

      // assign point to closest cluster
      cluster_data[nearest_cluster].push_back(data[point]);
    }

    // calculate new centroids
    for (int cluster = 0; cluster < k; cluster++) {
      double sum = 0;
      for (auto const &[id, value] : cluster_data[cluster]) {
        sum += value;
      }
      assert(sum > 0);
      centroids[cluster] = sum / cluster_data[cluster].size();

      for (auto const &[id, value] : cluster_data[cluster]) {
        total_avg_distance += std::abs(value - centroids[cluster]);
      }
    }

    total_avg_distance = total_avg_distance / data.size();
  }

  return cluster_data;
}

std::vector<std::pair<std::string, double>> getMinCluster(
    const std::vector<std::vector<std::pair<std::string, double>>> &cluster_data) {
  int min_cluster = -1;
  double min_centroid_value = std::numeric_limits<double>::max();

  for (size_t k = 0; k < cluster_data.size(); k++) {
    double centroid_value = 0;
    for (auto const &[id, value] : cluster_data[k]) {
      centroid_value += value;
    }
    centroid_value = centroid_value / cluster_data[k].size();

    if (centroid_value < min_centroid_value) {
      min_cluster = k;
      min_centroid_value = centroid_value;
    }
  }

  return cluster_data[min_cluster];
}

}  // namespace daisi::cpps

#endif
