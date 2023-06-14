// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#ifndef SOLA_STORAGE_STORAGE_H_
#define SOLA_STORAGE_STORAGE_H_

#include <future>
#include <string>
#include <variant>
#include <vector>

#include "minhton/core/definitions.h"
namespace sola {

struct Request {
  bool all;         // false -> some results are enough -> more efficient
  bool permissive;  // true -> results that dont fully satisfy query could be returned -> more
                    // efficient
  std::string request;
};

using Entry = std::tuple<std::string, std::variant<int, float, bool, std::string>>;

class Storage {
public:
  virtual ~Storage() = default;
  virtual void insert(std::vector<Entry> entry) = 0;
  // virtual void update() = 0;
  // virtual void remove() = 0;
  virtual std::future<minhton::FindResult> find(Request r) = 0;

  virtual void stop() = 0;
  virtual bool canStop() const = 0;
};
}  // namespace sola

#endif
