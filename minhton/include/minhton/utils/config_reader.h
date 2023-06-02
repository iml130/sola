// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#ifndef MINHTON_UTILS_CONFIG_READER_H_
#define MINHTON_UTILS_CONFIG_READER_H_

#include "minhton/utils/config_node.h"

namespace minhton::config {

minhton::ConfigNode readConfig(std::string path_to_file);

}  // namespace minhton::config

#endif
