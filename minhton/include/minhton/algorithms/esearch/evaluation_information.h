// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#ifndef MINHTON_ALGORITHMS_ESEARCH_EVALUATION_INFORMATION_H_
#define MINHTON_ALGORITHMS_ESEARCH_EVALUATION_INFORMATION_H_

#include <cstdint>

struct EvaluationInformation {
  uint64_t validity_threshold_timestamp = 0;
  bool all_information_present = false;
  bool inquire_unknown_attributes = false;
  bool inquire_outdated_attributes = false;
  bool permissive = false;  // only relevant if inquire_outdated_attributes=false
};

#endif
