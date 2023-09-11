// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#ifndef MINHTON_CORE_ROUTING_CALCULATIONS_H_
#define MINHTON_CORE_ROUTING_CALCULATIONS_H_

#include <cstdint>
#include <set>
#include <tuple>
#include <vector>

namespace minhton {
///
/// This gets the Sweep Direction, which is needed for the sophisticated join
/// procedure. Depending on the level a bool is returned, true if
/// the level is being filled from right to left.
///
/// Typical usage:
/// \code
///     bool fill_level_right_to_left = getFillLevelRightToLeft(0);
/// \endcode
///
/// \param level The level, where you want to know the sweep direction.
///
/// \return a bool, true for filling the level from right to left
///
bool getFillLevelRightToLeft(uint32_t level);

///
/// Enter the Level, Number and fanout of a node, to check, whether this node is
/// a PrioNode (-> a node handling the routing for the sophisticated join).
///
/// Typical usage:
/// \code
///     bool isPrio = isNodePartOfPrioSet(1, 0, 3);
/// \endcode
///
/// \param level The level of a node
/// \param number The number of a node
/// \param fanout The fanout the network is working with
///
/// \return True, if the given level, number and level belongs to a PrioNode
///
bool isNodePartOfPrioSet(uint32_t level, uint32_t number, uint16_t fanout);

///
/// Retrieve the parent-node of a node via level, number and fanout
///
/// Typical usage:
/// \code
///     std::tuple<uint16_t, uint16_t> parent = calcParent(1, 0, 3);
/// \endcode
///
/// \param level The level of a node
/// \param number The number of a node
/// \param fanout The fanout the network is working with
///
/// \return The tuple with level and number, otherwise throws std:invalid_argument if the level and
/// number cannot
///         exist in the network (like node 1:5 on fanout 2), or when retrieving the parent of the
///         root node 0:0
///
std::tuple<uint32_t, uint32_t> calcParent(uint32_t level, uint32_t number, uint16_t fanout);

///
/// Gets an ordered list (vector) of tuples with the level and number of children of a specific
/// node, by providing the level, number and fanout.
///
/// Typical usage:
/// \code
///     std::vector<std::tuple<uint16_t, uint16_t>> children = calcChildren(1, 1, 3);
///     // an ordered child list of node 1:1 for fanout 3  -> (2:3, 2:4, 2:5)
/// \endcode
///
/// \param level The level of a node
/// \param number The number of a node
/// \param fanout The fanout the network is working with
///
/// \return An ordered vector (or list) of the children for the given level, number and fanout.
/// Throws
///         std:invalid_argument when fanout is set to 0, or the provided level and number generates
///         an overflow or cannot be present in the network (like node 1:5 for fanout 2)
///
std::vector<std::tuple<uint32_t, uint32_t>> calcChildren(uint32_t level, uint32_t number,
                                                         uint16_t fanout);

///
/// Here we return a sequence of distances between node entries for the routing tables. More
/// specific: We return for fanout 2 the sequence: 1, 2, 4, 8, ... This vector can be then used to
/// calculate, depending on the nodes position, the actual number of a node in the routing table.
/// The level is used to make a sufficient large sequence whereby the fanout describes the gaps
/// between the numbers.
///
/// Typical usage:
/// \code
///     std::vector<uint64_t> seq = calcRoutingSequence(2, 2);
/// \endcode
///
/// \param level The level to get a large enough sequence
/// \param fanout The fanout which describe the gaps inside the sequence
///
/// \return A sequence of numbers for the routing tables, throws std:invalid_argument if fanout is
/// set to 0
///
std::vector<uint32_t> calcRoutingSequence(uint32_t level, uint16_t fanout);

///
/// Get the left routing table for a specifc node by providing the level, number and fanout.
/// This will give you an ordered list (vector) of tuples, which describe the routing table
/// neighbors
///
/// Typical usage:
/// \code
///     std::vector<std::tuple<uint32_t, uint32_t>> children = calcLeftRT(1, 2, 3);
///     // this would give you: (1:1, 1:0)
/// \endcode
///
/// \param level The level of a node
/// \param number The number of a node
/// \param fanout The fanout the network is working with
///
/// \return An ordered list (vector) of tuples, containing level and number left routing table.
/// Throws
///         std:invalid_argument if fanout is 0 or level and number cannot exist in the network
///
std::vector<std::tuple<uint32_t, uint32_t>> calcLeftRT(uint32_t level, uint32_t number,
                                                       uint16_t fanout);

///
/// Get the right routing table for a specifc node by providing the level, number and fanout.
/// This will give you an ordered list (vector) of tuples, which describe the routing table
/// neighbors
///
/// Typical usage:
/// \code
///     std::vector<std::tuple<uint16_t, uint16_t>> children = calcRightRT(1, 0, 3);
///     // this would give you: (1:1, 1:2)
/// \endcode
///
/// \param level The level of a node
/// \param number The number of a node
/// \param fanout The fanout the network is working with
///
/// \return An ordered list (vector) of tuples, containing level and number left routing table.
/// Throws
///         std:invalid_argument if fanout is 0, level and number cannot exist in the network or if
///         an overflow occured.
///
std::vector<std::tuple<uint32_t, uint32_t>> calcRightRT(uint32_t level, uint32_t number,
                                                        uint16_t fanout);

///
/// Calculates the numbers of the prio nodes, with a given level and fanout.
///
/// This method implements the formula described in the nBATON* paper.
///
/// Typical usage:
/// \code
///     calcPrioNet(level, fanout);
/// \endcode
///
/// \param level
/// \param fanout
///
/// \returns ordered set of numbers of the prio nodes on the level
///
std::set<uint32_t> calcPrioSet(uint32_t level, uint16_t fanout);

/// \returns true if the position is valid, false if not.
bool isPositionValid(uint32_t level, uint32_t number, uint16_t fanout);

/// \returns true if the fanout is >= 2 and not out of reange of uint16_t
bool isFanoutValid(uint16_t fanout);

///
/// With this method we can recursively calculate the relative horizontal value of each node,
/// given its position in the tree, and the fanout of the network.
///
/// The constant K has to be the same in the different use-cases to
/// for comparibility of the relative horizontal values.
///
/// Typical usage:
/// \code
///     double value = treeMapper(level, number, fanout, 1000);
/// \endcode
///
/// \param level
/// \param number
/// \param fanout
/// \param K an arbitrary constant which has to be the same
///
/// \returns horizontal value
///
double treeMapper(uint32_t level, uint32_t number, uint16_t fanout, double K);

///
/// Method internally used by the treeMapper to additionally pass lower and upper bounds.
/// Only the center is used in the outer treeMapper method.
///
/// \returns tuple of lower bound, upper bound, and center
///
std::tuple<double, double, double> treeMapperInternal(uint32_t level, uint32_t number,
                                                      uint16_t fanout, uint8_t K);

std::tuple<uint32_t, uint32_t> getCoveringDSN(uint32_t level, uint32_t number, uint16_t fanout);
std::vector<std::tuple<uint32_t, uint32_t>> getCoverArea(uint32_t level, uint32_t number,
                                                         uint16_t fanout);
std::vector<uint32_t> getDSNSet(uint32_t level, uint16_t fanout);

}  // namespace minhton
#endif
