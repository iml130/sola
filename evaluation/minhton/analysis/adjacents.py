# Copyright The SOLA Contributors
#
# Licensed under the MIT License.
# For details on the licensing terms, see the LICENSE file.
# SPDX-License-Identifier: MIT

from minhton.model.constants import NeighborRelationship
from minhton.analysis.node import Node
from minhton.analysis.general_helpers import get_existing_nodes_for_event_id, get_sql_data_unfiltered


def get_adjacent_rights_data_for_event_id(event_id, sql_connector):
    """
        Getting the latest adjacent right relationships.
        Only those with the highest id for the level-number combination.
    """

    event_id_condition = ""
    if event_id:
        event_id_condition = "=" + str(event_id[0])

    statement = """
        SELECT * from (
            SELECT NodeLevel, NodeNumber, NodeIp, NodePort, NeighborLevel, NeighborNumber, NeighborIp, NeighborPort
            from viewRoutingInfo
            where Relationship = '{}' and EventId {} and NeighborLevel >= 0
            order by Id desc
        )
        group by NodeLevel, NodeNumber
        """.format(NeighborRelationship.ADJACENT_RIGHT.name, event_id_condition)
    # TODO: Grouping shouldn't be necessary since the routing infos should only be updated once
    # statement = """
    #     SELECT level, number, ip_address, port, neighbor_level, neighbor_number, neighbor_ip_address, neighbor_port
    #     from routing_infos
    #     where relationship = {} and event_id {}
    #     """.format(ADJACENT_RIGHT, event_id_condition)

    return get_sql_data_unfiltered(sql_connector, statement)


def get_adjacent_lefts_data_for_event_id(event_id, sql_connector):
    """
        Getting the latest adjacent left relationships.
        Only those with the highest id for the level-number combination.
    """
    event_id_condition = ""
    if event_id:
        event_id_condition = "=" + str(event_id[0])

    statement = """
        SELECT * from (
            SELECT NodeLevel, NodeNumber, NodeIp, NodePort, NeighborLevel, NeighborNumber, NeighborIp, NeighborPort
            from viewRoutingInfo
            where Relationship = '{}' and EventId {} and NeighborLevel >= 0
            order by Id desc
        )
        group by NodeLevel, NodeNumber
        """.format(NeighborRelationship.ADJACENT_LEFT.name, event_id_condition)

    return get_sql_data_unfiltered(sql_connector, statement)


def get_adjacent_relationship_dicts(left_adjacents_data, right_adjacents_data, fanout):
    """
        Creating a dictionary for the left and right adjacent relationships.

        Example of a network with fanout=2 and a network with 7 nodes:
        The adjacent left of 1:0 is 2:0, because 2:0 is the closest node of 1:0 to the left side (horizontally).
        At the same time the adjacent right of 2:0 is 1:0, because 1:0 is the closest node of 2:0 to the right.

        In the dictionaries there will be:
            adj_left_of [ 1:0 ] = 2:0
            adj_right_of [ 2:0 ] = 1:0
    """

    adj_left_of = dict()
    for line in left_adjacents_data:
        level, number, ip_address, port, neighbor_level, neighbor_number, neighbor_ip_address, neighbor_port = line

        node = Node(level, number, ip_address, port, fanout)
        adj_left_of_node = Node(
            neighbor_level, neighbor_number, neighbor_ip_address, neighbor_port, fanout)

        adj_left_of[node] = adj_left_of_node

    adj_right_of = dict()
    for line in right_adjacents_data:
        level, number, ip_address, port, neighbor_level, neighbor_number, neighbor_ip_address, neighbor_port = line

        node = Node(level, number, ip_address, port, fanout)
        adj_right_of_node = Node(
            neighbor_level, neighbor_number, neighbor_ip_address, neighbor_port, fanout)

        adj_right_of[node] = adj_right_of_node

    return adj_left_of, adj_right_of


def validate_adjacents_bidirectionality(adj_left_of, adj_right_of):
    """
        Bidirectionality Condition!

        If a node a has the adjacent left node b,  it is necessary that the adjacent right of b is a.

        Checking the following condition:
            adj_left_of(a) = b
                <=>
            adj_right_of(b) = a

        Or written differently:
            adj_left_of(a) = b
                and
            adj_right_of(b) = c

            Then a = c must be true

    """

    valid = True
    invalid_pairs = []
    for node_a in adj_left_of.keys():  # each node, which has a adjacent left

        # adj_left_of(a) = b
        node_b = adj_left_of[node_a]

        try:
            # only check further when b is an initialized node
            if node_b.address:
                # adj_right_of(b) = c
                node_c = adj_right_of[node_b]

                # only check further when c is an initialized node
                if node_c.address:
                    # then a and c must be the same node
                    is_current_pair_valid = node_a == node_c

                    if not is_current_pair_valid:
                        valid = False
                        invalid_pairs.append([node_a, node_b, node_c])

        except KeyError:
            valid = False
            invalid_pairs.append([node_a, node_b])

    return valid, invalid_pairs


def validate_adjacents_locations(adj_left_of, adj_right_of):
    """
        Location Condition!

        The adjacent left of a node has to be on its left side.
        The adjacent right of a node has to be on its right side.

        Because the Node class implements the comparision operators, we can check:
            if a has an adjacent left: adj_left_of(a) < a
                and
            if a has an adjacent right: a < adj_right_of(a)
    """

    valid = True
    invalid_pairs = []

    adjacents = {**adj_left_of, **adj_right_of}
    for node_a in adjacents.keys():
        is_left_adj_lower = True
        is_right_adj_greater = True
        value_node_a = node_a.value
        if node_a in adj_left_of.keys():
            node_b = adj_left_of[node_a]
            value_node_b = node_b.value
            if node_b.address:
                is_left_adj_lower = value_node_b < value_node_a
        else:
            node_b = None

        if node_a in adj_right_of.keys():
            node_c = adj_right_of[node_a]
            value_node_c = node_c.value
            if node_c.address:
                is_right_adj_greater = value_node_c > value_node_a
        else:
            node_c = None

        is_current_pair_valid = is_left_adj_lower and is_right_adj_greater

        if not is_current_pair_valid:
            valid = False
            invalid_pairs.append([node_a, node_b, node_c])

    return valid, invalid_pairs


def validate_adjacents_boundary(adj_left_of, adj_right_of, network_size, leftmost_node, rightmost_node):
    """
        Boundary Condition!

        There must be only one node which does not have an adjacent left neighbor,
        and only one node which does not have an adjacent right neighbor.

        The node which is the furthest to the left in the tree (with the lowest horizontal value)
        cannot have a adjacent left, because there is no node further to the left.

        The node which is the furthest to the right in the tree (with the highest horizontal value)
        cannot have a adjacent right, because there is no node further to the right.

        adj_left_of.keys() contains all nodes which have an adjacent left neighbor.
        adj_right_of.keys() contains all nodes which have an adjacent right neighbor.

        The intersection of both, should have exactly two nodes missing. 

    """

    valid = True
    invalid_boudaries = []

    # filter nodes that have only one adjacent neighbor
    intersection = adj_left_of.keys() & adj_right_of.keys()

    if len(adj_left_of) == network_size-1:
        equal_number_of_adjacents = len(adj_left_of) == len(adj_right_of)
    else:
        valid = False

    # check if there is exactly one node in each dict that is not contained in the other one
    nodes_that_have_two_adjacents = len(intersection) == network_size-2

    if not (nodes_that_have_two_adjacents and equal_number_of_adjacents):
        valid = False

    else:
        if leftmost_node in adj_left_of.keys():
            valid = False
            invalid_boudaries.append(leftmost_node)
        if rightmost_node in adj_right_of.keys():
            valid = False
            invalid_boudaries.append(rightmost_node)

    return valid, invalid_boudaries


def validate_adjacents(event_id, sql_connector, general_db_information):
    """
        Check if the left and right adjacent node are correct
        for each node in the network for a given event id.

        Returns three boolean values, representing the correctness of:
            - the bidirectional condition
            - the location condition
            - the boundary condition
    """

    # getting raw data from database
    adj_lefts_data = get_adjacent_lefts_data_for_event_id(
        event_id, sql_connector)
    adj_rights_data = get_adjacent_rights_data_for_event_id(
        event_id, sql_connector)

    if len(adj_lefts_data) == 0 and len(adj_rights_data) == 0:
        return [True, ], [True, ], [True, ]
    # relevant data to validate the boundary condition

    # creating the relationship dictionaries
    adj_left_of, adj_right_of = get_adjacent_relationship_dicts(
        adj_lefts_data, adj_rights_data, general_db_information.fanout)

    # validating
    valid_bidirectionality = validate_adjacents_bidirectionality(
        adj_left_of, adj_right_of)
    valid_locations = validate_adjacents_locations(
        adj_left_of, adj_right_of)

    existing_nodes = get_existing_nodes_for_event_id(
        event_id, sql_connector, general_db_information)
    leftmost_node = min(existing_nodes)
    rightmost_node = max(existing_nodes)
    network_size = len(existing_nodes)
    valid_boundaries = (True, [])
    # if event_id is None:
    #     valid_boundaries = validate_adjacents_boundary(
    #         adj_left_of, adj_right_of, network_size, leftmost_node, rightmost_node)

    return valid_bidirectionality, valid_locations, valid_boundaries
