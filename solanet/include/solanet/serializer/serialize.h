// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#ifndef SOLANET_SERIALIZER_SERIALIZE_H_
#define SOLANET_SERIALIZER_SERIALIZE_H_

#define SERIALIZE(...)                                                                \
  template <class Archive> void serialize(Archive &archive) { archive(__VA_ARGS__); } \
  static_assert(true)

#endif  // SOLANET_MESSAGE_SERIALIZE_H_
