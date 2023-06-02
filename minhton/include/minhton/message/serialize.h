// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#ifndef MINHTON_MESSAGE_SERIALIZE_H_
#define MINHTON_MESSAGE_SERIALIZE_H_

#define SERIALIZE(...) \
  template <class Archive> void serialize(Archive &archive) { archive(__VA_ARGS__); }

#endif  // MINHTON_MESSAGE_SERIALIZE_H_
