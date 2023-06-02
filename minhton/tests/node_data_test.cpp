// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#include "algorithms/esearch/node_data.h"

#include <catch2/catch_test_macros.hpp>

#include "algorithms/esearch/distributed_data.h"

using namespace minhton;

// TEST_CASE("NodeData insert update remove", "[NodeData][insert, update, remove]") {
//   NodeData data;

//   REQUIRE(data.insert("a1", {13, 12345}));
//   REQUIRE(data.hasKey("a1"));
//   auto val1 = data.getValue("a1");
//   auto pval1 = std::get_if<int>(&val1);
//   REQUIRE(pval1 != nullptr);
//   REQUIRE(*pval1 == 13);
//   REQUIRE(std::get<1>(data.getValueAndTimestamp("a1")) == 12345);
//   REQUIRE(!data.insert("a1", {14, 123456}));
//   REQUIRE_NOTHROW(data.remove("a1"));
//   REQUIRE(!data.hasKey("a1"));

//   REQUIRE(data.insert("a2", {3.14159f, 1234567}));
//   REQUIRE(data.hasKey("a2"));
//   auto val2 = data.getValue("a2");
//   auto pval2 = std::get_if<float>(&val2);
//   REQUIRE(pval2 != nullptr);
//   REQUIRE(*pval2 == 3.14159f);
//   REQUIRE(std::get<1>(data.getValueAndTimestamp("a2")) == 1234567);
//   REQUIRE(!data.insert("a2", {"does not work hopefully", 123456}));
//   REQUIRE_NOTHROW(data.remove("a2"));
//   REQUIRE(!data.hasKey("a2"));

//   REQUIRE(data.insert("a3", {true, 42}));
//   REQUIRE(data.hasKey("a3"));
//   auto val3 = data.getValue("a3");
//   auto pval3 = std::get_if<bool>(&val3);
//   REQUIRE(pval3 != nullptr);
//   REQUIRE(*pval3);
//   REQUIRE(std::get<1>(data.getValueAndTimestamp("a3")) == 42);
//   REQUIRE(!data.insert("a3", {42, 43}));
//   REQUIRE_NOTHROW(data.remove("a3"));
//   REQUIRE(!data.hasKey("a3"));

//   std::string my_string = "es regnet";
//   REQUIRE(data.insert("a4", {my_string, 42}));
//   REQUIRE(data.hasKey("a4"));
//   REQUIRE(std::get<1>(data.getValueAndTimestamp("a4")) == 42);
//   auto val4 = data.getValue("a4");
//   auto pval4 = std::get_if<std::string>(&val4);
//   REQUIRE(pval4 != nullptr);
//   REQUIRE(*pval4 == "es regnet");
//   REQUIRE(!data.insert("a4", {42, 43}));
//   REQUIRE_NOTHROW(data.remove("a4"));
//   REQUIRE(!data.hasKey("a4"));

//   std::string new_string = "insert joke here";
//   REQUIRE(data.insert("a1", {new_string, 422}));
//   REQUIRE(data.hasKey("a1"));

//   std::string updating_string = "hahaha";
//   REQUIRE(!data.insert("a1", {updating_string, 4223}));
//   auto val5 = data.getValue("a1");
//   auto pval5 = std::get_if<std::string>(&val5);
//   REQUIRE(pval5 != nullptr);
//   REQUIRE(*pval5 == "insert joke here");

//   REQUIRE(data.update("a1", {updating_string, 4234}));
//   auto val6 = data.getValue("a1");
//   auto pval6 = std::get_if<std::string>(&val6);
//   REQUIRE(pval6 != nullptr);
//   REQUIRE(*pval6 == "hahaha");

//   REQUIRE(!data.update("keydoesnotexist", {45, 4234}));
//   REQUIRE(!data.insert("a1", {"bla", 45454}));

//   REQUIRE(data.update("a1", {111, 2222}));
//   auto val7 = data.getValue("a1");
//   auto pval7 = std::get_if<int>(&val7);
//   REQUIRE(pval7 != nullptr);
//   REQUIRE(*pval7 == 111);
// }

// TEST_CASE("NodeData getData", "[NodeData][getData]") {
//   NodeData data;

//   REQUIRE(data.insert("a1", {42, 43}));
//   REQUIRE(data.insert("a2", {4.144f, 44}));
//   REQUIRE(data.insert("a3", {false, 45}));
//   std::string my_string = "teststring";
//   REQUIRE(data.insert("a4", {my_string, 46}));

//   auto map = data.getData();

//   auto a1_val = map["a1"];
//   auto a1_pval = *std::get_if<int>(&std::get<0>(a1_val));
//   auto a1_timestamp = std::get<1>(a1_val);
//   REQUIRE(a1_timestamp == 43);
//   REQUIRE(a1_pval == 42);

//   auto a2_val = map["a2"];
//   auto a2_pval = *std::get_if<float>(&std::get<0>(a2_val));
//   auto a2_timestamp = std::get<1>(a2_val);
//   REQUIRE(a2_timestamp == 44);
//   REQUIRE(a2_pval == 4.144f);

//   auto a3_val = map["a3"];
//   auto a3_pval = *std::get_if<bool>(&std::get<0>(a3_val));
//   auto a3_timestamp = std::get<1>(a3_val);
//   REQUIRE(a3_timestamp == 45);
//   REQUIRE(!a3_pval);

//   auto a4_val = map["a4"];
//   auto a4_pval = *std::get_if<std::string>(&std::get<0>(a4_val));
//   auto a4_timestamp = std::get<1>(a4_val);
//   REQUIRE(a4_timestamp == 46);
//   REQUIRE(a4_pval == "teststring");
// }

// TEST_CASE("DistributedData subscriptionOrderKeys", "[NodeData][subscriptionOrderKeys]") {
//   DistributedData data;
//   REQUIRE(data.getSubscriptionOrderKeys().empty());

//   data.addSubscriptionOrderKey("a1");
//   REQUIRE(data.getSubscriptionOrderKeys()[0] == "a1");
//   REQUIRE(data.isKeySubscribed("a1"));

//   data.removeSubscriptionOrderKey("a1");
//   REQUIRE(!data.isKeySubscribed("a1"));
//   REQUIRE(data.getSubscriptionOrderKeys().empty());

//   data.addSubscriptionOrderKey("a2");
//   REQUIRE(data.getSubscriptionOrderKeys().size() == 1);
//   data.addSubscriptionOrderKey("a2");
//   REQUIRE(data.getSubscriptionOrderKeys().size() == 1);
//   data.addSubscriptionOrderKey("a3");
//   REQUIRE(data.getSubscriptionOrderKeys().size() == 2);
//   data.addSubscriptionOrderKey("a4");
//   REQUIRE(data.getSubscriptionOrderKeys().size() == 3);

//   data.removeSubscriptionOrderKey("a1");
//   REQUIRE(data.getSubscriptionOrderKeys().size() == 3);
//   data.removeSubscriptionOrderKey("a2");
//   REQUIRE(data.getSubscriptionOrderKeys().size() == 2);
//   data.removeSubscriptionOrderKey("a4");
//   REQUIRE(data.getSubscriptionOrderKeys().size() == 1);
//   data.removeSubscriptionOrderKey("a3");
//   REQUIRE(data.getSubscriptionOrderKeys().empty());
// }

// TEST_CASE("DistributedData isValueUpToDate", "[NodeData][isValueUpToDate]") {
//   DistributedData data;

//   REQUIRE(!data.isValueUpToDate("a1", 10));

//   REQUIRE(data.insert("a1", {1234, 1000}));

//   // everything below threshold (older) is invalid if not subscribed
//   uint64_t threshold1 = 900;
//   uint64_t threshold2 = 1100;

//   REQUIRE(data.isValueUpToDate("a1", threshold1));
//   REQUIRE(!data.isValueUpToDate("a1", threshold2));

//   data.addSubscriptionOrderKey("a1");
//   REQUIRE(data.isValueUpToDate("a1", threshold1));
//   REQUIRE(data.isValueUpToDate("a1", threshold2));
//   REQUIRE(data.isValueUpToDate("a1", 10));
// }

// TEST_CASE("DistributedData insert, update, remove", "[NodeData][insert, update, remove]") {
//   DistributedData data;
//   REQUIRE(data.getUpdateTimestamps("a").empty());
//   REQUIRE(data.getUpdateTimestamps("b").empty());

//   REQUIRE(data.insert("a", {"hi", 1234}));
//   REQUIRE(data.getUpdateTimestamps("a").front() == 1234);
//   REQUIRE(data.hasKey("a"));

//   REQUIRE(data.insert("b", {"ups", 123456}));
//   REQUIRE(data.getUpdateTimestamps("b").front() == 123456);
//   REQUIRE(data.getUpdateTimestamps("b").size() == 1);
//   REQUIRE(data.getUpdateTimestamps("a").front() == 1234);
//   REQUIRE(data.hasKey("b"));

//   REQUIRE(!data.insert("b", {"geht nicht", 123457}));
//   REQUIRE(data.getUpdateTimestamps("b").size() == 1);
//   REQUIRE(data.hasKey("b"));

//   REQUIRE(data.update("b", {"geht doch", 123457}));
//   REQUIRE(data.getUpdateTimestamps("b").size() == 2);
//   REQUIRE(data.getUpdateTimestamps("b").front() == 123456);
//   REQUIRE(data.getUpdateTimestamps("b").back() == 123457);

//   REQUIRE(data.update("b", {"geht immer noch", 1234578}));
//   REQUIRE(data.getUpdateTimestamps("b").front() == 123456);
//   REQUIRE(data.getUpdateTimestamps("b").back() == 1234578);

//   data.remove("b");
//   REQUIRE(data.getUpdateTimestamps("b").empty());

//   REQUIRE(data.insert("c", {"hi", 0}));
//   for (uint8_t i = 0; i < data.getTimestampStorageLimit(); i++) {
//     REQUIRE(data.update("c", {"hi", i + 1}));
//   }

//   REQUIRE(data.getUpdateTimestamps("c").front() == 1);
//   REQUIRE(data.getUpdateTimestamps("c").back() == data.getTimestampStorageLimit());
//   REQUIRE(data.getUpdateTimestamps("c").size() == data.getTimestampStorageLimit());
// }

// TEST_CASE("DistributedData NetworkInfo", "[NodeData][NetworkInfo]") {
//   DistributedData data;

//   REQUIRE(!data.getNetworkInfo().isInitialized());
//   data.setNetworkInfo(NetworkInfo("1.2.3.4", 20123));

//   REQUIRE(data.getNetworkInfo().getAddress() == "1.2.3.4");
//   REQUIRE(data.getNetworkInfo().getPort() == 20123);

//   DistributedData data2(NetworkInfo("8.8.8.8", 1111));

//   REQUIRE(data2.getNetworkInfo().getAddress() == "8.8.8.8");
//   REQUIRE(data2.getNetworkInfo().getPort() == 1111);
// }
