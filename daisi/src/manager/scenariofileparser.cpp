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

#include "scenariofileparser.h"

#include <yaml-cpp/yaml.h>

#include <filesystem>
#include <fstream>
#include <iterator>
#include <stdexcept>
#include <string>

namespace daisi {

ScenariofileParser::ScenariofileParser(const std::string &path_to_file)
    : scenariofile_path_(path_to_file) {
  loadFileContent(path_to_file);
  parse();
}

void ScenariofileParser::loadFileContent(const std::string &path_to_file) {
  try {
    std::fstream fs;
    fs.open(path_to_file);
    if (fs.fail()) {
      scenariofile_content_ = path_to_file;
    } else {
      std::ifstream t(path_to_file);
      std::string file_as_string((std::istreambuf_iterator<char>(t)),
                                 std::istreambuf_iterator<char>());
      scenariofile_content_ = file_as_string;
    }
  } catch (const std::runtime_error &e) {
    throw;
  }
}

void ScenariofileParser::parse() {
  YAML::Node node = YAML::Load(scenariofile_content_);
  parsed_content_ = parseRecursive(node);
}

std::shared_ptr<ScenariofileParser::Table> ScenariofileParser::parseRecursive(YAML::Node node) {
  auto table = std::make_shared<ScenariofileParser::Table>();

  for (auto const &it : node) {
    std::string key = it.first.as<std::string>();
    if (it.second.IsScalar()) {
      try {
        table->content[key] = it.second.as<uint64_t>();
      } catch (const YAML::BadConversion &e) {
        std::string str = it.second.as<std::string>();

        try {
          float val = std::stof(str);
          table->content[key] = val;
        } catch (const std::invalid_argument &inv_arg) {
          table->content[key] = str;
        } catch (const std::out_of_range &oor) {
          table->content[key] = str;
        }
      };

    } else if (it.second.IsMap()) {
      table->content[key] = parseRecursive(it.second);

    } else if (it.second.IsSequence()) {
      std::vector<std::shared_ptr<ScenariofileParser::Table>> temp_vec;
      temp_vec.reserve(it.second.size());

      for (std::size_t i = 0; i < it.second.size(); i++) {
        temp_vec.push_back(parseRecursive(it.second[i]));
      }

      table->content[key] = temp_vec;
    } else {
      // empty
      table->content[key] = "";
    }
  }
  return table;
}

std::string ScenariofileParser::getHelperFileContent(const std::string &path_str,
                                                     const std::string &file_name) {
  std::filesystem::path path(path_str);
  path.append(file_name);

  try {
    std::fstream fs;
    fs.open(path);
    if (!fs.fail()) {
      std::ifstream t(path);
      std::string file_as_string((std::istreambuf_iterator<char>(t)),
                                 std::istreambuf_iterator<char>());
      return file_as_string;
    }
  } catch (const std::runtime_error &e) {
    throw;
  }

  return "";
}

std::string ScenariofileParser::pathWithEndingSlash(std::string path) {
  if (path.empty()) throw std::runtime_error("cannot modify empty path");
  if (path.at(path.size() - 1) != '/') path.append("/");
  return path;
}
}  // namespace daisi
