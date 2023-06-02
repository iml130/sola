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

#ifndef DAISI_MANAGER_SCENARIOFILEPARSER_H_
#define DAISI_MANAGER_SCENARIOFILEPARSER_H_

#include <cstdint>
#include <cstdlib>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace YAML {
class Node;
}

namespace daisi {

class ScenariofileParser {
public:
  struct Table {
    using TableMap =
        std::unordered_map<std::string,
                           std::variant<std::string, uint64_t, float, std::shared_ptr<Table>,
                                        std::vector<std::shared_ptr<Table>>>>;
    TableMap content;

    template <typename T> T getRequired(const std::string &key) {
      auto it = content.find(key);
      if (it == content.end()) {
        throw std::invalid_argument("Scenariofile does not contain required '" + key + "'");
      }

      auto var = content[key];
      auto pval = std::get_if<T>(&var);
      if (!pval) {
        throw std::invalid_argument("Scenariofile required key '" + key + "' has wrong type");
      }

      return *pval;
    }

    template <typename T> std::optional<T> getOptional(const std::string &key) {
      auto it = content.find(key);
      if (it == content.end()) {
        return std::nullopt;
      }

      auto var = content[key];
      if (auto pval = std::get_if<T>(&var)) {
        return *pval;
      }
      return std::nullopt;
    }

    std::optional<std::variant<int, float, bool, std::string>> getValue(const std::string &key) {
      auto it = content.find(key);
      if (it == content.end()) {
        return std::nullopt;
      }

      auto var = content[key];
      std::variant<int, float, bool, std::string> val;

      if (auto pval = std::get_if<std::string>(&var)) {
        val = *pval;
        return val;
      }

      if (auto pval = std::get_if<float>(&var)) {
        val = *pval;
        return val;
      }

      if (auto pval = std::get_if<uint64_t>(&var)) {
        val = (int)*pval;
        return val;
      }

      return std::nullopt;
    }
  };

  explicit ScenariofileParser(const std::string &path_to_file);
  std::shared_ptr<Table> getParsedContent() { return parsed_content_; }

  std::string getTitle() { return parsed_content_->getRequired<std::string>("title"); }
  std::string getName() { return parsed_content_->getRequired<std::string>("name"); }
  std::string getVersion() { return parsed_content_->getRequired<std::string>("version"); }
  std::string getPhysicalLayer() {
    return parsed_content_->getRequired<std::string>("physicalLayer");
  }

  std::string getOutputPath() {
    // outputPath from yaml takes precedence over environment variable DAISI_OUTPUT_PATH
    auto output_path_scenario = parsed_content_->getOptional<std::string>("outputPath");
    if (output_path_scenario.has_value() && !output_path_scenario.value().empty()) {
      return pathWithEndingSlash(output_path_scenario.value());
    }

    std::string output_path_env = std::getenv("DAISI_OUTPUT_PATH");
    if (!output_path_env.empty()) return pathWithEndingSlash(output_path_env);

    throw std::runtime_error("No output path set!");
  }

  std::string getLogLevel() { return parsed_content_->getRequired<std::string>("logLevel"); }

  uint64_t getStopTime() { return parsed_content_->getRequired<uint64_t>("stoptime"); }
  uint64_t getFanout() { return parsed_content_->getRequired<uint64_t>("fanout"); }
  uint64_t getDefaultDelay() { return parsed_content_->getRequired<uint64_t>("defaultDelay"); }
  uint64_t getRandomSeed() { return parsed_content_->getRequired<uint64_t>("randomSeed"); }

  template <typename ValueT>
  std::unordered_map<std::string, ValueT> getTable(const std::string &tableName) {
    std::unordered_map<std::string, ValueT> map;
    auto table =
        parsed_content_->getRequired<std::shared_ptr<ScenariofileParser::Table>>(tableName);

    // Copy values into new map
    for (auto it : table->content) {
      if (!std::holds_alternative<ValueT>(it.second))
        throw std::runtime_error("table " + tableName + " does not contain correct type!");
      map.insert({it.first, std::get<ValueT>(it.second)});
    }

    return map;
  }

  std::vector<std::shared_ptr<Table>> getScenarioSequence() {
    return parsed_content_->getRequired<std::vector<std::shared_ptr<Table>>>("scenarioSequence");
  }

  std::string getScenariofileContent() { return scenariofile_content_; }

  std::string getHelperFileContent(const std::string &path, const std::string &file_name);

private:
  void loadFileContent(const std::string &path_to_file);

  void parse();
  std::shared_ptr<Table> parseRecursive(YAML::Node node);

  // Add slash at end of path if missing
  static std::string pathWithEndingSlash(std::string path);

  std::string scenariofile_path_;
  std::string scenariofile_content_;
  std::shared_ptr<Table> parsed_content_;
};

}  // namespace daisi

#endif

#define INNER_TABLE(iterator) \
  (*std::get_if<std::shared_ptr<ScenariofileParser::Table>>(&iterator->second))

#define STRING_TO_LOWER(s) \
  (std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return std::tolower(c); }))
