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

#ifndef DAISI_NEGOTIATION_UTILITY_UTILITY_EVALUATOR_NS3_H_
#define DAISI_NEGOTIATION_UTILITY_UTILITY_EVALUATOR_NS3_H_

#include "cpps/negotiation/mrta_configuration.h"
#include "cpps/negotiation/utility/utility_dimensions.h"

namespace daisi::cpps {

class UtilityEvaluator {
public:
  explicit UtilityEvaluator(const UtilityEvaluationType &type);

  static bool compare(const UtilityDimensions &udim1, const UtilityDimensions &udim2);
  void evaluate(UtilityDimensions &udim) const;
  void reevaluate(UtilityDimensions &udim) const;

private:
  const UtilityEvaluationType type_;

  void setCosts(UtilityDimensions &udim) const;
  static double linearCombination(double const &first, double const &second, double const &alpha);
};

}  // namespace daisi::cpps
#endif
