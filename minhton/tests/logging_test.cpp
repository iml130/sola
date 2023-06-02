// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#include <catch2/catch_test_macros.hpp>
#include <filesystem>
#include <string>

// #include "logging/interface.h"
// #include "logging/mysql.h"
// using namespace minhton;

// void logTestdata(ILogger &logger) {
//   LoggerInfoSearchExact logger_info_search = {1, 2, 2, 3, 4, 5, 6, 7, 8};
//   LoggerInfoSearchExact logger_info_search1 = {1, 2, 3, -4, -5, -6, -7, -8};
//   logger.add_search_exact_test(logger_info_search);
//   logger.add_search_exact_test(logger_info_search1);

//   LoggerInfoAddNode logger_info_peer = {1, 2, 3, 4, 5, 6};
//   logger.add_peer(logger_info_peer);

//   LoggerInfoAddNeighbor logger_info_neighbor = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
//   logger.add_neighbor(logger_info_neighbor);

//   MessageLoggingAdditionalInfo additionalInfo;
//   additionalInfo.node1_level = 1;
//   additionalInfo.node1_number = 2;
//   additionalInfo.node1_address = 3;
//   additionalInfo.node2_level = 4;
//   additionalInfo.node2_number = 5;
//   additionalInfo.node2_address = 6;

//   MessageLoggingInfo loggingInfo;
//   loggingInfo.timestamp = 1;
//   loggingInfo.cmd_type = 2;
//   loggingInfo.mode = 3;
//   loggingInfo.event_id = 4;
//   loggingInfo.ref_event_id = 5;
//   loggingInfo.sender_level = 6;
//   loggingInfo.sender_number = 7;
//   loggingInfo.sender_address = 8;
//   loggingInfo.target_level = 9;
//   loggingInfo.target_number = 10;
//   loggingInfo.target_address = 11;
//   loggingInfo.additional_info = additionalInfo;

//   logger.log_traffic(loggingInfo);

//   logger.log_traffic(loggingInfo);
// }
