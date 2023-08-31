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

#ifndef DAISI_MANAGER_GENERAL_SCENARIOFILE_H_
#define DAISI_MANAGER_GENERAL_SCENARIOFILE_H_

#include <fstream>
#include <string>

#include "scenariofile_component.h"

namespace daisi {

struct GeneralScenariofile {
  explicit GeneralScenariofile(const std::string &path_to_file)
      : file_path_(std::move(path_to_file)) {
    loadFileContent();
    node = YAML::Load(file_content_);

    SERIALIZE_VAR(title);
    SERIALIZE_VAR(version);

    SERIALIZE_VAR(random_seed);

    SERIALIZE_VAR(stop_time);
    SERIALIZE_VAR(default_delay);

    SERIALIZE_VAR(output_path);
  }

  std::string getOutputPath() {
    // output_path from yaml takes precedence over environment variable DAISI_OUTPUT_PATH

    if (output_path.has_value() && !output_path.value().empty()) {
      return pathWithEndingSlash(output_path.value());
    }

    std::string output_path_env = std::getenv("DAISI_OUTPUT_PATH");
    if (!output_path_env.empty()) return pathWithEndingSlash(output_path_env);

    throw std::runtime_error("No output path set!");
  }

  // Add slash at end of path if missing
  static std::string pathWithEndingSlash(std::string path) {
    if (path.empty()) throw std::runtime_error("cannot modify empty path");
    if (path.at(path.size() - 1) != '/') path.append("/");
    return path;
  }

  // header
  std::string title;
  std::string version;

  // setup information
  int random_seed = 0;

  // simulation information
  uint64_t stop_time = 0;
  uint64_t default_delay = 0;

  std::optional<std::string> output_path;

  YAML::Node node;

private:
  void loadFileContent() {
    std::fstream fs;
    fs.open(file_path_);

    if (fs.fail()) {
      file_content_ = file_path_;
    } else {
      std::ifstream t(file_path_);
      std::string file_as_string((std::istreambuf_iterator<char>(t)),
                                 std::istreambuf_iterator<char>());
      file_content_ = file_as_string;
    }
  }

  std::string file_content_;
  std::string file_path_;
};
}  // namespace daisi
#endif
