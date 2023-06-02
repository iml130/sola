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

#include "sqlite_helper.h"

#include <sqlite3.h>

#include <algorithm>
#include <cstdio>
#include <filesystem>
#include <iostream>
#include <sstream>
#include <stdexcept>

static constexpr size_t kMaxQueueSize = 100000;

namespace daisi {

SQLiteHelper::SQLiteHelper(const std::string &file_path, const std::string &file_name)
    : file_path_(std::move(file_path)), file_name_(std::move(file_name)) {
  this->connect();
}

SQLiteHelper::~SQLiteHelper() {
  if (db_ != nullptr) {
    disconnect();
  }

  std::string temp_new = file_path_ + file_name_;
  std::cout << this->temp_file_name_ << std::endl;
  if (failed_database_) {
    temp_new += ".fail";
  }
  if (rename(this->temp_file_name_.c_str(), temp_new.c_str()) != 0) {
    perror("Error renaming file");
  } else {
    std::cout << "File renamed successfully" << std::endl;
  }
}

void SQLiteHelper::connect() {
  if (db_ != nullptr) return;

  if (file_name_.empty()) {
    throw std::runtime_error("filename not set");
  }

  std::cout << "Creating database " << file_path_ + file_name_ << std::endl;
  this->temp_file_name_ = (file_path_ + file_name_ + ".tmp");

  std::filesystem::create_directory(file_path_);

  // open database
  int rc = sqlite3_open(this->temp_file_name_.c_str(), &db_);

  if (rc != SQLITE_OK) {
    if (rc == SQLITE_CANTOPEN) {
      throw std::runtime_error(
          "Cannot create database, probably because the specified output path does not exist!");
    }
    throw std::runtime_error("Cannot create database! SQLite error code: " + std::to_string(rc) +
                             ". More information about this error is at: " +
                             "https://www.sqlite.org/rescode.html#primary_result_code_list");
  }

  // Setting these PRAGMAs improves performance
  // See also https://www.sqlite.org/pragma.html
  rc = sqlite3_exec(db_, "PRAGMA synchronous = OFF", NULL, NULL, NULL);
  if (rc != 0) {
    std::cout << "Failed to set PRAGMA synchronous = OFF" << std::endl;
  }
  rc = sqlite3_exec(db_, "PRAGMA journal_mode = MEMORY", NULL, NULL, NULL);
  if (rc != 0) {
    std::cout << "Failed to set PRAGMA journal_mode = MEMORY" << std::endl;
  }
  rc = sqlite3_exec(db_, "PRAGMA page_size = 65536", NULL, NULL, NULL);
  if (rc != 0) {
    std::cout << "Failed to set PRAGMA page_size = 65536" << std::endl;
  }
}

void SQLiteHelper::logQueueTask(const std::vector<std::string> &queries) {
  // Execute queries as a single transaction
  std::ostringstream stream;
  stream << "BEGIN TRANSACTION;";
  for (const auto &command : queries) {
    stream << command;
  }
  stream << "COMMIT;";
  auto stream_string = stream.str();

  std::cout << "LOGGING" << std::endl;
  char *err_msg = nullptr;
  int rc = sqlite3_exec(db_, stream_string.c_str(), nullptr, nullptr, &err_msg);

  if (rc != SQLITE_OK) {
    setFailed();
    std::string error_message = "Error executing aggregate query: ";
    error_message += err_msg;
    throw std::runtime_error(error_message);
  }
}

void SQLiteHelper::logQueue() {
#ifdef DEFERRED_LOGGING

  // Nothing to do
  if (queries_.empty()) return;

  std::vector<std::string> queries;
  std::swap(queries, queries_);

  if (last_logging_task_.valid()) last_logging_task_.wait();
  last_logging_task_ = std::async(&SQLiteHelper::logQueueTask, this, queries);

#else
  throw std::runtime_error("deferred logging not enabled");
#endif
}

void SQLiteHelper::disconnect() {
  if (db_ == nullptr) return;

#ifdef DEFERRED_LOGGING
  logQueue();
#endif

  if (last_logging_task_.valid()) last_logging_task_.wait();

  int rc = sqlite3_close(db_);
  db_ = nullptr;

  if (rc != SQLITE_OK) {
    throw std::runtime_error("Error closing database!");
  }
}

void SQLiteHelper::execute(const std::string &query) {
#ifdef DEFERRED_LOGGING
  // Store query and execute them all together when simulation ends
  queries_.push_back(std::move(query));

  // Avoid excessive RAM usage
  if (queries_.size() > kMaxQueueSize) logQueue();
#else
  executeSql(query.c_str());
#endif
}

void SQLiteHelper::executeSql(const char *query) {
  if (db_ == nullptr) throw std::runtime_error("Not connected to database!");

  char *err_msg_ptr = 0;
  int rc = sqlite3_exec(db_, query, nullptr, nullptr, &err_msg_ptr);

  if (rc != SQLITE_OK) {
    std::string error_message = "Error executing query: ";
    error_message += err_msg_ptr;
    //! In case of exception: Check query for broken strings
    throw std::runtime_error(error_message);
  }
}

void SQLiteHelper::setFailed() { failed_database_ = true; }

}  // namespace daisi
