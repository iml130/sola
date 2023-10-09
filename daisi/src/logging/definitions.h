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

#ifndef DAISI_LOGGING_DEFINITIONS_H_
#define DAISI_LOGGING_DEFINITIONS_H_

#include <cstdarg>
#include <cstdint>
#include <functional>
#include <map>
#include <sstream>
#include <string>
#include <tuple>
#include <type_traits>
#include <unordered_set>

// Args: (Application UUID)
using LogDeviceApp = const std::function<void(const std::string &)>;

// Args: (SQL statement)
using LogFunction = std::function<void(const std::string &)>;

// Args: (Event UUID, Event type, Application ID)
using LogEvent = const std::function<void(const std::string &, uint8_t, const std::string &)>;

/// @brief Creates a SQL Statement by calling snprintf. The amount of format specifiers in
/// format_str has to be identical to the provided values
/// @tparam ...Tp Derived automatically from the value types
/// @param format_str String with format specifiers
/// @param ...values Arbitrary number of values to be inserted into the format_str
/// @return String with the complete SQL Statement
template <typename... Tp> inline std::string toSQL(const std::string &format_str, Tp... values) {
  // Calculate length of formatted statement
  int len = snprintf(nullptr, 0, format_str.c_str(), values...);
  if (len < 0) {
    throw std::runtime_error("Unable to format SQL statement!");
  }

  // Write statement to string
  std::string statement;
  statement.resize(len);
  int rc = snprintf(statement.data(), len + 1, format_str.c_str(), values...);
  if (rc < 0) {
    throw std::runtime_error("Unable to format SQL statement!");
  }

  return statement;
}

class DatabaseColumnInfo {
public:
  std::string name;
  std::string format = "NULL";
  bool not_null = false;
  bool is_id = false;
  std::string foreign_key = "";
  bool is_primary_key = false;
  std::string data_type = "INTEGER";

  void setDataType() {
    static const std::unordered_set<std::string> int_specifiers{"%d",  "%i",  "%ld",
                                                                "%li", "%lu", "%u"};
    static const std::unordered_set<std::string> real_specifiers{"%f", "%lf"};
    bool plain_string = true;
    std::string format_copy = format;
    if (format.rfind("sql", 0) == 0) {
      // If format starts with "sql", we will parse the value as an SQL statement
      format_copy.erase(0, 3);
      format = "%s";
      plain_string = false;
    }

    if (int_specifiers.count(format_copy) != 0) {
      data_type = "INTEGER";
    } else if (real_specifiers.count(format_copy) != 0) {
      data_type = "REAL";
    } else if (format_copy == "%s") {
      data_type = "TEXT";
      if (plain_string) format = "'%s'";
    } else if (format_copy == "NULL") {
      data_type = "";
    } else {
      throw std::runtime_error("Unknown ColumnDataType");
    }
  }

  // TODO: Declare as explicit(false) for implicit conversion in C++20
  /// @brief Constructor for primary key id column
  /// @param name Column name
  explicit DatabaseColumnInfo(std::string name) : name(std::move(name)), is_id(true){};

  /// @brief Constructor for regular and foreign key column
  /// @param name Column name
  /// @param format Format specifier like in a printf call. Possible formats: "%d", "%f", "%i",
  /// "%ld", "%li", "%lu", "%s", "%u", "NULL". Each format can be prepended with "sql", if the value
  /// will be collected with a select statement.
  /// @param not_null If true, the column will be restricted to be not null
  /// @param foreign_key If set, the foreign key reference will be included, e.g., "Event(Id)"
  /// @param is_primary_key If true, the column will be set as the primary key
  DatabaseColumnInfo(std::string name, std::string format, bool not_null = false,
                     std::string foreign_key = "", bool is_primary_key = false)
      : name(std::move(name)),
        format(std::move(format)),
        not_null(not_null),
        foreign_key(std::move(foreign_key)),
        is_primary_key(is_primary_key) {
    setDataType();
  };
};

class DatabaseTable {
public:
  std::string name;
  std::vector<DatabaseColumnInfo> columns;
  std::string additional_constraints;

  /// @brief Constructor for a table definition
  /// @param name Table name
  /// @param columns Column definitions of this table
  /// @param additional_constraints Will be appended at the end. Use this for unique of multiple
  /// columns or join definitions
  explicit DatabaseTable(std::string name, std::vector<DatabaseColumnInfo> columns = {},
                         std::string additional_constraints = "")
      : name(std::move(name)),
        columns(std::move(columns)),
        additional_constraints(std::move(additional_constraints)){};
};

/// @brief Generate a string containing a SQL Create Table Statement based on the table argument
/// @param table DatabaseTable definition
/// @return String with the complete SQL Statement
inline std::string getCreateTableStatement(const DatabaseTable &table) {
  std::ostringstream statement;
  statement << "CREATE TABLE IF NOT EXISTS " << table.name << "(";

  std::ostringstream foreign_keys;

  // Iterate over columns and append each column name with its specification
  // followed by a comma when it's not the last column
  for (auto col_it = table.columns.begin(); col_it != table.columns.end(); col_it++) {
    statement << col_it->name << " " << col_it->data_type;
    if (col_it->is_id) {
      statement << " NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE";
    } else if (col_it->is_primary_key) {
      statement << " NOT NULL PRIMARY KEY UNIQUE";
    } else if (col_it->not_null) {
      statement << " NOT NULL";
    }
    if (col_it->foreign_key.length() != 0U) {
      // Needs to be appended after all columns are created
      foreign_keys << ",FOREIGN KEY(" << col_it->name << ") REFERENCES " << col_it->foreign_key;
    }

    if (col_it != --table.columns.end()) {
      statement << ",";
    }
  }

  statement << foreign_keys.str();
  if (!table.additional_constraints.empty()) {
    statement << "," << table.additional_constraints;
  }

  statement << ");";
  return statement.str();
}

/// @brief Generate a string containing a SQL Create View Statement based on the table argument
/// @param table DatabaseTable which will be the base
/// @param replacements maps the columns from table to the replacements, e.g.,
/// {{"PositionUuid","N1.Level AS Level, N1.Number AS Number"}} where PositionUuid is the original
/// column and Level as well as Number are the new columns at the position of PositionUuid
/// @param joins contains the join definitions in the SQL format, e.g., {"LEFT JOIN MinhtonNode AS
/// N1 ON MinhtonNodeState.PositionUuid = N1.PositionUuid"}
/// @param view_name Optional name for the View. If not provided, the name will be derived from the
/// original Table name
/// @return String with the complete SQL Statement
inline std::string getCreateViewStatement(
    const DatabaseTable &table, const std::unordered_map<std::string, std::string> &replacements,
    const std::vector<std::string> &joins, const std::string &view_name = "") {
  std::ostringstream statement;

  std::string final_view_name = view_name.empty() ? "view" + table.name : view_name;
  statement << "CREATE VIEW IF NOT EXISTS " << final_view_name << " AS SELECT ";

  // Iterate over columns and append each column name with its specification
  // followed by a comma when it's not the last column
  for (auto col_it = table.columns.begin(); col_it != table.columns.end(); col_it++) {
    auto repl = replacements.find(col_it->name);
    if (repl != replacements.end()) {
      if (!repl->second.empty()) {
        // Append replaced column from joined table
        statement << repl->second;
      }
    } else {
      // Append own column
      statement << table.name << "." << col_it->name;
    }

    // Append comma if another column follows after this one
    if (col_it != --table.columns.end()) {
      statement << ",";
    }
  }

  statement << " FROM " << table.name;

  // Append join definitions
  if (!joins.empty()) {
    statement << " ";
    for (auto join_it = joins.begin(); join_it != joins.end(); join_it++) {
      statement << *join_it;

      // Append space if another join definition follows after this one
      if (join_it != --joins.end()) {
        statement << " ";
      }
    }
  }

  statement << ";";
  return statement.str();
}

/// @brief Generate a string containing a SQL Insert Statement based on the table and values
/// arguments
/// @tparam ...Tp Derived automatically from the types of the values tuple
/// @param table DatabaseTable definition
/// @param values Values to be inserted
/// @return String with the complete SQL Statement
template <typename... Tp>
std::string getInsertStatement(const DatabaseTable &table, const std::tuple<Tp...> &values) {
  static constexpr bool kStringInTuple{(std::is_same_v<std::string, Tp> || ...)};
  static_assert(!kStringInTuple,
                "Error generating SQL Insert Statement: The tuple contains a string. "
                "Convert the string to char array with .c_str() before calling!");

  std::ostringstream statement;
  statement << "INSERT INTO " << table.name << " VALUES(";

  for (auto col_it = table.columns.begin(); col_it != table.columns.end(); col_it++) {
    statement << col_it->format;

    if (col_it != --table.columns.end()) {
      statement << ",";
    }
  }

  statement << ");";
  const std::string format_str = statement.str();

  // Calculate length of the formatted query
  std::tuple<char *, int, const char *> buffer_format_tuple{nullptr, 0, format_str.c_str()};
  auto snprintf_args = std::tuple_cat(buffer_format_tuple, values);
  //! A segmentation fault at this position could indicate a mismatch between table and values
  int len = std::apply(snprintf, snprintf_args);
  if (len < 0) {
    throw std::runtime_error("Unable to format SQL query!");
  }

  // Write query with now known length to string and return it
  std::string query;
  query.resize(len);
  buffer_format_tuple = std::make_tuple(query.data(), len + 1, format_str.c_str());
  snprintf_args = std::tuple_cat(buffer_format_tuple, values);
  int rc = std::apply(snprintf, snprintf_args);
  if (rc < 0) {
    throw std::runtime_error("Unable to format SQL query!");
  }
  return query;
}

#endif
