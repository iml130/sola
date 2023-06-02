// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#ifndef MINHTON_MESSAGE_FIND_END_H_
#define MINHTON_MESSAGE_FIND_END_H_

namespace minhton {
/// Used by the minhton join & leave algorithm to save the progress of the position finding
enum SearchProgress { kSearchRight, kSearchLeft, kNone, kCheckRight, kReplacementNode };
}  // namespace minhton
#endif
