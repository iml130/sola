# Copyright The SOLA Contributors
#
# Licensed under the MIT License.
# For details on the licensing terms, see the LICENSE file.
# SPDX-License-Identifier: MIT

import sys
from minhton.model.constants import NodeState

def get_max_level(event_id, sql_connector):
    """
        Highest level of a node in the whole network.
    """
    if event_id is None:
        timestamp = None
    else:
        timestamp_statement = """
            SELECT MAX(Timestamp_ms)
            FROM MinhtonRoutingInfo
            WHERE EventId = {}
            """.format(event_id[0])
        timestamp = sql_connector.fetch_one(timestamp_statement)[0]

    timestamp_condition = ""
    if timestamp:
        timestamp_condition = "<=" + str(timestamp)

    statement = """
        SELECT max(Level) FROM (
            SELECT max(Id), Level, Number, State
            FROM viewMinhtonNodeState
            WHERE Timestamp_ms {}
            GROUP BY Level, Number
            ORDER BY Id DESC
        )
        WHERE State = '{}'
    """.format(timestamp_condition, NodeState.RUNNING.name)

    return sql_connector.fetch_one(statement)[0]


def number_nodes_perfect_tree(fanout, tree_height):
    """
        Calculates number of nodes in perfect MINHTON tree
        with given fanout and height

        Uses the geometric sum formula
    """

    numerator = 1-(fanout**(tree_height+1))
    denominator = 1-fanout

    return numerator/denominator


def is_network_perfectly_balanced(existing_nodes, max_level):
    """
        Checking by the number of nodes in the network.
    """
    expected_network_size = number_nodes_perfect_tree(
        existing_nodes[0].fanout, max_level)
    network_size = len(existing_nodes)

    return expected_network_size == network_size


def validate_number_of_nodes_at_level(level, existing_nodes):

    fanout = existing_nodes[0].fanout
    if level == 0:
        expected_nodes_on_level = 1
    elif level == 1:
        expected_nodes_on_level = number_nodes_perfect_tree(fanout, level) - 1
    else:
        expected_nodes_on_level = number_nodes_perfect_tree(
            fanout, level) - number_nodes_perfect_tree(fanout, level-1)

    nodes_on_level = list(filter(lambda x: x.level == level, existing_nodes))
    # nodes_on_level = sorted(nodes_on_level, key=lambda node: node.value)

    # TODO: Fix this check
    return len(nodes_on_level) == expected_nodes_on_level


def validate_perfectly_balanced(existing_nodes, max_level):
    """
        If we have the correct number of nodes to be a perfectly balanced tree,
        we must also check if there are really no gaps in the tree.

        The last level must be filled completely.
        On each level above there must be no missing node.
    """
    perfectly_balanced = True

    nullbalanced, invalid_levels = validate_null_balanced(
        existing_nodes, max_level)
    balanced_on_max_level = validate_number_of_nodes_at_level(
        max_level, existing_nodes)

    if not (nullbalanced and balanced_on_max_level):
        perfectly_balanced = False
        invalid_levels.append(max_level)

    return perfectly_balanced, invalid_levels


def validate_null_balanced(existing_nodes, max_level):
    """
        If we cannot be a perfectly_balanced tree, we must at least check if we are nullbalanced.

        Only the last level may have missing nodes.
        On each level above there must be no missing node.
    """
    nullbalanced = True
    invalid_levels = []

    for i in range(max_level):
        if not validate_number_of_nodes_at_level(i, existing_nodes):
            nullbalanced = False
            invalid_levels.append(i)

    return nullbalanced, invalid_levels


def validate_tree_structure(event_id, existing_nodes, sql_connector):
    """
        Check if the tree_structure of a network is correct for a given event_id.
        Returns False if the tree is not nullbalanced or not perfectly balanced when it should be.

    """

    max_level = get_max_level(event_id, sql_connector)

    valid_structure = True

    should_be_perfectly_balanced = is_network_perfectly_balanced(
        existing_nodes, max_level)

    if should_be_perfectly_balanced:
        perfectly_balanced = validate_perfectly_balanced(
            existing_nodes, max_level)[0]

        if not perfectly_balanced:
            valid_structure = False
            print(
                "The tree is not perfectly balanced, even though it should be for event id", event_id[0], file=sys.stderr)

    else:
        nullbalanced = validate_null_balanced(existing_nodes, max_level)[0]
        if not nullbalanced:
            valid_structure = False
            print("The tree is not nullbalanced for event id",
                  event_id[0], file=sys.stderr)

    return valid_structure


def validate_height_balanced_structure(existing_nodes):
    """
        Check if the tree_structure of a network is height balanced for a given timestamp.

    """

    root = HeightBalancedTestNode(0, 0, existing_nodes[0].fanout)

    existing_nodes_sorted = sorted(existing_nodes, key=lambda n: n.level)

    for node in existing_nodes_sorted[1:]:
        root.add(node.level, node.number)

    return root.is_height_balanced()


class HeightBalancedTestNode:
    """ Helper class for height balanced validation """

    def __init__(self, level, number, fanout):
        self.level = level
        self.number = number
        self.fanout = fanout
        self.children = [None] * fanout

    def __str__(self):
        return "({}:{})".format(self.level, self.number)

    @staticmethod
    def getParent(level, number, fanout):
        return (int(level-1), int((number - (number % fanout)) / fanout))

    def add(self, level, number):

        if level == self.level+1:
            child = HeightBalancedTestNode(level, number, self.fanout)

            # our level
            pos = number % self.fanout
            self.children[pos] = child
        else:
            # forwarding to one of our children
            # in who's subtree the node will be

            parent_level, parent_number = HeightBalancedTestNode.getParent(
                level, number, self.fanout)
            while parent_level-1 > self.level:
                parent_level, parent_number = HeightBalancedTestNode.getParent(
                    parent_level, parent_number, self.fanout)

            child_forward_pos = parent_number % self.fanout
            self.children[child_forward_pos].add(level, number)

    def height(self) -> int:
        children_heights = []
        for child in self.children:
            if child:
                children_heights.append(child.height())

        if len(children_heights) == 0:
            return 1
        return max(children_heights) + 1

    def is_height_balanced(self) -> bool:
        children_heights = []
        for child in self.children:
            if not child:
                children_heights.append(0)
            if child:
                if not child.is_height_balanced():
                    return False
                children_heights.append(child.height())

        if len(children_heights) == 0:
            return True

        min_height = min(children_heights)
        max_height = max(children_heights)
        if abs(max_height - min_height) > 1:
            return False

        return True
