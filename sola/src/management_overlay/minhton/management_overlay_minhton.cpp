// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#include "SOLA/management_overlay_minhton.h"

#include <cassert>
#include <string>
#include <tuple>
#include <variant>

#include "minhton/algorithms/esearch/find_query.h"
#include "minhton/algorithms/esearch/node_data.h"
#include "minhton/core/definitions.h"

namespace sola {
ManagementOverlayMinhton::ManagementOverlayMinhton(const minhton::ConfigNode &config)
    : node_(std::make_unique<minhton::Minhton>(
          [](const minhton::ConnectionInfo & /*neighbor*/) { /* nothing required here */ },
          config)) {}

void ManagementOverlayMinhton::insert(std::vector<Entry> entries) {
  std::vector<minhton::Entry> data;
  for (auto &[key, value] : entries) {
    minhton::Entry a = {key, value, minhton::NodeData::ValueType::kValueDynamic};
    data.push_back(a);
  }
  node_->insert(data);
}

std::future<minhton::FindResult> ManagementOverlayMinhton::find(Request r) {
  minhton::FindQuery query;
  query.deserializeBooleanExpression(r.request);
  assert(query.getBooleanExpression() != nullptr);  // deserialization has failed

  if (r.permissive) {
    query.setInquireOutdatedAttributes(false);
    query.setPermissive(true);
  }
  query.setInquireUnknownAttributes(false);

  // all, or specific with endpoint ?
  query.setSelection(minhton::FindQuery::FindQuerySelection::kSelectAll);
  query.setValidityThreshold(10000);

  return node_->find(query);
}

void ManagementOverlayMinhton::stop() { node_->stop(); }

bool ManagementOverlayMinhton::canStop() const {
  return node_->getState() == minhton::State::kIdle;
}

}  // namespace sola
