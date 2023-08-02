# Copyright The SOLA Contributors
#
# Licensed under the MIT License.
# For details on the licensing terms, see the LICENSE file.
# SPDX-License-Identifier: MIT

from minhton.analysis.parent_child import get_children_relationship_dict, get_children_data_for_event_id
from minhton.model.constants import NeighborRelationship
from minhton.analysis.general_helpers import get_sql_data
from .node import Node


def get_routing_table_neighbors_data_for_event_id(event_id, sql_connector):
    """
    Getting the latest routing_table_neighbor relationships.
    Only those with the highest id for each level-number combination.
    """
    event_id_condition = ""
    if event_id:
        event_id_condition = "=" + str(event_id[0])

    statement = """
        SELECT NodeLevel, NodeNumber, NodeIp, NodePort, NeighborLevel, NeighborNumber, NeighborIp, NeighborPort
        FROM viewMinhtonRoutingInfo
        WHERE Relationship = '{}' and Id in(
            SELECT max(Id)
            FROM viewMinhtonRoutingInfo
            WHERE Relationship = '{}' AND EventId {}
            GROUP BY NodeLevel, NodeNumber, NeighborLevel, NeighborNumber
        )
    """.format(NeighborRelationship.ROUTING_TABLE_NEIGHBOR.name, NeighborRelationship.ROUTING_TABLE_NEIGHBOR.name, event_id_condition)

    return get_sql_data(sql_connector, statement)


def get_routing_table_neighbor_children_data_for_event_id(event_id, sql_connector):
    """
    Getting the latest routing_table_neighbor_children relationships.
    Only those with the highest id for each level-number combination.
    """
    event_id_condition = ""
    if event_id:
        event_id_condition = "=" + str(event_id[0])

    statement = """
        SELECT NodeLevel, NodeNumber, NodeIp, NodePort, NeighborLevel, NeighborNumber, NeighborIp, NeighborPort
        FROM viewMinhtonRoutingInfo
        WHERE Relationship = '{}' and Id in(
            SELECT max(Id)
            FROM viewMinhtonRoutingInfo
            WHERE Relationship = '{}' AND EventId {}
            GROUP BY NodeLevel, NodeNumber, NeighborLevel, NeighborNumber
        )
    """.format(NeighborRelationship.ROUTING_TABLE_NEIGHBOR_CHILD.name, NeighborRelationship.ROUTING_TABLE_NEIGHBOR_CHILD.name, event_id_condition)

    return get_sql_data(sql_connector, statement)


def get_routing_table_neighbor_relationship_dict(routing_table_neighbors_data, fanout):
    """
    Creating a dictionary for the routing_table_neighbors_relationship.

    Example for fanout 2:
    the routing_table_neighbors of 3:1 are 3:0, 3:2, 3:3, 3:5.

    In the dictionary:
    routing_table_neighbors_of[3:1] = [3:0, 3:2, 3:3, 3:5]

    """

    routing_table_neighbor_of = dict()

    first_node = routing_table_neighbors_data[0]
    current_node = Node(first_node[0], first_node[1],
                        first_node[2], first_node[3], fanout)
    routing_table_neighbors_of_current_node = []

    for line in routing_table_neighbors_data:
        level, number, ip_address, port, neighbor_level, neighbor_number, neighbor_ip_address, neighbor_port = line

        node = Node(level, number, ip_address, port, fanout)

        if node != current_node:
            # new node in fokus
            routing_table_neighbor_of[current_node] = routing_table_neighbors_of_current_node
            current_node = node
            routing_table_neighbors_of_current_node = []

        routing_table_neighbor_of_node = Node(neighbor_level, neighbor_number,
                                              neighbor_ip_address, neighbor_port, fanout)
        routing_table_neighbors_of_current_node.append(
            routing_table_neighbor_of_node)

    # for the last node
    routing_table_neighbor_of[current_node] = routing_table_neighbors_of_current_node

    return routing_table_neighbor_of


def get_routing_table_neighbor_children_relationship_dict(routing_table_neighbor_children_data, fanout):
    """
    Creating a dictionary for the routing_table_neighbor_children_relationship.

    Example for fanout 2:
    the routing_table_neighbors of 2:1 are 2:0, 2:2, 2:3.
    Then the routing_table_neighbor_childs of 2:1 are the childs of these nodes,
    so 3:0, 3:1, 3:4, 3:5, 3:6, 3:7.

    In the dictionary:
    routing_table_neighbor_childs_of[2:1] = [3:0, 3:1, 3:4, 3:5, 3:6, 3:7]
    """

    routing_table_neighbor_children_of = dict()

    first_node = routing_table_neighbor_children_data[0]
    current_node = Node(first_node[0], first_node[1],
                        first_node[2], first_node[3], fanout)
    routing_table_neighbor_children_of_current_node = []

    for line in routing_table_neighbor_children_data:
        level, number, ip_address, port, neighbor_level, neighbor_number, neighbor_ip_address, neighbor_port = line

        node = Node(level, number, ip_address, port, fanout)

        if node != current_node:
            routing_table_neighbor_children_of[current_node] = routing_table_neighbor_children_of_current_node
            current_node = node
            routing_table_neighbor_children_of_current_node = []

        routing_table_neighbor_child_of_node = Node(neighbor_level, neighbor_number,
                                                    neighbor_ip_address, neighbor_port, fanout)
        routing_table_neighbor_children_of_current_node.append(
            routing_table_neighbor_child_of_node)

    # for the last node
    routing_table_neighbor_children_of[current_node] = routing_table_neighbor_children_of_current_node

    return routing_table_neighbor_children_of


def getRoutingTableNumbers(level: int, number: int, fanout: int):
    """
    calculating the valid numbers of the routing table neighbors and rt neighbor childs for one node
    """

    seq = [d*(fanout**i) for i in range((fanout-1)*level)
           for d in range(1, fanout)]
    nList_left = [number - k for k in seq if number - k >= 0]
    nList_right = [number + k for k in seq if number + k < fanout**level]

    neighbor_numbers = sorted(nList_left + nList_right)
    neighbor_children_numbers = [
        parent_number * fanout + k for parent_number in neighbor_numbers for k in range(0, fanout)]

    return neighbor_numbers, neighbor_children_numbers


def validate_positions_of_routing_table_neighbors(routing_table_neighbors_of):
    """
    the routing_table_neighbors of node_a have the numbers calculated by:
    node_a.number +- [1,2,..,fanout-1]*fanout**i, i>=0

    example for fanout 2 and node 3:0:

    the numbers of the routing_table_neighbors have to be
    0+1*2⁰, 0+1*2¹, 0+1*2² = 3:1, 3:2, 3:4
    """
    valid = True
    invalid_pairs = []

    for node_a in routing_table_neighbors_of.keys():
        routing_table_neighbors = routing_table_neighbors_of[node_a]
        # only get numbers of the routing table neighbors, not also for the neighbor children
        routing_table_neighbor_numbers = getRoutingTableNumbers(
            node_a.level, node_a.number, node_a.fanout)[0]

        for neighbor in routing_table_neighbors:
            correct_level = node_a.level == neighbor.level
            correct_number = neighbor.number in routing_table_neighbor_numbers

            if not(correct_level and correct_number):
                valid = False
                invalid_pairs.append([node_a, neighbor])

    return valid, invalid_pairs


def validate_positions_of_routing_table_neighbor_children(routing_table_neighbor_children_of):
    """
    the routing_table_neighbors of node_a have the numbers calculated by:
    node_a.number +- [1,2,..,fanout-1]*fanout**i, i>=0
    the routing table neighbor children are all children of those neighbors.

    example for fanout 2 and node 3:0:

    the numbers of the routing_table_neighbors have to be
    0+1*2⁰, 0+1*2¹, 0+1*2² = 3:1, 3:2, 3:4

    4:2, 4:3, 4:4, 4:5, 4:8, 4:9 are all valid positions for the routing table neighbor children of 3:0


    """
    valid = True
    invalid_pairs = []

    for node_a in routing_table_neighbor_children_of.keys():
        routing_table_neighbor_children = routing_table_neighbor_children_of[node_a]
        # only get numbers of the routing table neighbor children, not also for the neighbors
        routing_table_neighbor_children_numbers = getRoutingTableNumbers(
            node_a.level, node_a.number, node_a.fanout)[1]

        for neighbor_child in routing_table_neighbor_children:
            correct_level = node_a.level == neighbor_child.level-1
            correct_number = neighbor_child.number in routing_table_neighbor_children_numbers

            if not(correct_level and correct_number):
                valid = False
                invalid_pairs.append([node_a, neighbor_child])

    return valid, invalid_pairs


def validate_routing_table_neighbor_bidirectionality(routing_table_neighbors_of):
    """
    node_a has node_b as routing_table_neighbor <--> node_b has node_a as routing_table_neighbor
    """

    valid = True
    invalid_pairs = []

    for node_a in routing_table_neighbors_of.keys():
        for node_b in routing_table_neighbors_of[node_a]:
            has_rt_neighbors = node_b in routing_table_neighbors_of.keys()
            if has_rt_neighbors:
                rt_neighbors_of_b = routing_table_neighbors_of[node_b]
                is_rt_neighbor_of_a = node_a in rt_neighbors_of_b
            if not(has_rt_neighbors and is_rt_neighbor_of_a):
                valid = False
                invalid_pairs.append([node_a, node_b])

    return valid, invalid_pairs


def validate_known_routing_table_neighbor_childs(routing_table_neighbors_of, routing_table_neighbor_children_of, children_of):
    """
    if node_a has node_b as routing_table_neighbor,
    node_a should also have all childs of node_b as routing_table_neighbor_childs
    """
    valid = True
    invalid_pairs = []

    for node_a in routing_table_neighbors_of.keys():
        for node_b in routing_table_neighbors_of[node_a]:
            if node_b in children_of.keys():
                for child_of_b in children_of[node_b]:
                    if node_a not in routing_table_neighbor_children_of.keys() or child_of_b not in routing_table_neighbor_children_of[node_a]:
                        valid = False
                        invalid_pairs.append([node_a, child_of_b])

    return valid, invalid_pairs


def validate_routing_table_information(event_id, sql_connector, general_db_information):
    """
        Check if the routing-table neighbors and neighbor children are correct
        for each node in the network for a given event id.

        Returns four boolean values, representing the correctness of:
            - the position of the routing-table neighbors
            - the bidirectionality between two routing-table neighbors
            - the position of the routing table neighbor children
            - the routing table neighbor children of each node
    """

    # getting raw data from database
    routing_table_neighbor_data = get_routing_table_neighbors_data_for_event_id(
        event_id, sql_connector)
    if len(routing_table_neighbor_data) == 0:
        return [True, ], [True, ], [True, ], [True, ]

    routing_table_neighbor_children_data = get_routing_table_neighbor_children_data_for_event_id(
        event_id, sql_connector)
    exist_rt_neighbor_children = len(
        routing_table_neighbor_children_data) > 0

    children_data = get_children_data_for_event_id(event_id, sql_connector)
    exist_children = len(children_data) > 0

    # creating the relationship dictionaries
    routing_table_neighbors_of = get_routing_table_neighbor_relationship_dict(
        routing_table_neighbor_data, general_db_information.fanout)

    if exist_rt_neighbor_children:
        routing_table_neighbor_children_of = get_routing_table_neighbor_children_relationship_dict(
            routing_table_neighbor_children_data, general_db_information.fanout)

        if exist_children:
            children_of = get_children_relationship_dict(
                children_data, general_db_information.fanout)

    # validating
    valid_rt_neighbor_positions = validate_positions_of_routing_table_neighbors(
        routing_table_neighbors_of)

    valid_bidirectionality = validate_routing_table_neighbor_bidirectionality(
        routing_table_neighbors_of)

    if exist_rt_neighbor_children:
        valid_rt_neighbor_children_positions = validate_positions_of_routing_table_neighbor_children(
            routing_table_neighbor_children_of)

        if exist_children:
            valid_rt_neighbor_children = validate_known_routing_table_neighbor_childs(
                routing_table_neighbors_of, routing_table_neighbor_children_of, children_of)
        else:
            valid_rt_neighbor_children = [True, ]
    else:
        valid_rt_neighbor_children_positions = [True, ]
        valid_rt_neighbor_children = [True, ]

    return valid_rt_neighbor_positions, valid_bidirectionality, valid_rt_neighbor_children_positions, valid_rt_neighbor_children
