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

#ifndef DAISI_SQLITE_HELPER_H_
#define DAISI_SQLITE_HELPER_H_

#include <future>
#include <string>
#include <vector>

class sqlite3;

namespace daisi {

class SQLiteHelper {
public:
  /// Constructs a SQLite logger and generates database
  /// \param file_path Absolute path to the location where the database should be created
  /// \param file_name Name of the database to be created
  SQLiteHelper(const std::string &file_path, const std::string &file_name);

  /// Disconnects and renames the temporary database filename to final name
  ~SQLiteHelper();

  void setFailed();

  void execute(const std::string &query);

private:
  sqlite3 *db_ = nullptr;
  const std::string file_path_;
  std::string file_name_;
  std::string temp_file_name_;
  std::vector<std::string> queries_;
  std::future<void> last_logging_task_;
  bool failed_database_ = false;

  // creates and opens the database files
  void connect();

  // log queued queries to database. Only used if deferred logging is enabled!
  [[maybe_unused]] void logQueue();
  [[maybe_unused]] void logQueueTask(const std::vector<std::string> &queries);

  // executes a sql query
  void executeSql(const char *query);

  // closes the connection (or the file)
  void disconnect();
};

}  // namespace daisi

#endif
