# Copyright The SOLA Contributors
#
# Licensed under the MIT License.
# For details on the licensing terms, see the LICENSE file.
# SPDX-License-Identifier: MIT

import math

calculated_mappings = {}
last_fanout = 0
ceiled_half_f = 0

def treeMapper(l: int, n: int, f: int, K: int=100):
    if f < 2:
        raise ValueError("Fanout in treeMapper call is below 2")
    if K < 0:
        raise ValueError("K in treeMapper call is below 0")

    global last_fanout
    global ceiled_half_f
    if last_fanout != f:
        # Fanout has changed or wasn't set yet
        ceiled_half_f = math.ceil(f / 2.0)
        last_fanout = f

    key = f"{l}-{n}-{f}-{K}"
    global calculated_mappings
    if key in calculated_mappings:
        return calculated_mappings[key]
    else:
        calculated_mappings[key] = treeMapperInternal(l, n, f, K)[2]
        return calculated_mappings[key]


def treeMapperInternal(l: int, n: int, f: int, K):
    '''   K > 0 !
        This function calculates the correct Mapping from a level and number 
        of a Node with the given constant K and fanout. 

        As long as K and fanout stays the same, the returned value can be compared to other 
        level and number values.
    '''

    if l == 0:
        lower = 0
        upper = (K / ceiled_half_f) * f
        center = ((upper - lower) / f) * ceiled_half_f
    else:
        parent_level = l - 1
        parent_number = (n - (n % f)) / f

        # recursive call to get the parent bounds
        parent_results = treeMapperInternal(parent_level, parent_number, f, K)
        lower_parent = parent_results[0]
        upper_parent = parent_results[1]
        next_width = (upper_parent - lower_parent) / f;  # step width of children of our parent

        lower = lower_parent + next_width * (n % f)
        upper = lower + next_width
        center = lower + ((upper - lower) / f) * ceiled_half_f

    return (lower, upper, center)
