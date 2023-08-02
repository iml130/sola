# Copyright The SOLA Contributors
#
# Licensed under the MIT License.
# For details on the licensing terms, see the LICENSE file.
# SPDX-License-Identifier: MIT

from minhton.analysis.general_helpers import get_existing_nodes_for_event_id, get_sql_data
from minhton.analysis.node import Node
from minhton.model.constants import NeighborRelationship


def get_parents_data_for_event_id(event_id, sql_connector):
    """
        Getting the latest parent relationships.
        Only those with the highest id for the level-number combination.
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
            WHERE EventId {} AND Relationship = '{}'
            GROUP BY NodeLevel, NodeNumber
        )
    """.format(NeighborRelationship.PARENT.name, event_id_condition, NeighborRelationship.PARENT.name)

    return get_sql_data(sql_connector, statement)


def get_children_data_for_event_id(event_id, sql_connector):
    """
        Getting the latest children relationships.
        Only those with the highest id for the level-number combination.
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
            WHERE EventId {} AND Relationship = '{}'
            GROUP BY NodeLevel, NodeNumber, NeighborLevel, NeighborNumber
        )
    """.format(NeighborRelationship.CHILD.name, event_id_condition, NeighborRelationship.CHILD.name)

    return get_sql_data(sql_connector, statement)


def get_parent_relationship_dict(parent_data, fanout):
    """
        Dictionary with child as key and parent as value.

        parent_of[node_child] = node_parent
    """

    parent_of = dict()

    for line in parent_data:
        level, number, ip_address, port, neighbor_level, neighbor_number, neighbor_ip_address, neighbor_port = line

        node = Node(level, number, ip_address, port, fanout)
        parent_of_node = Node(neighbor_level, neighbor_number,
                              neighbor_ip_address, neighbor_port, fanout)

        parent_of[node] = parent_of_node

    return parent_of


def get_children_relationship_dict(children_data, fanout):
    """
        Dictionary with parent as key and list of children as value.

        children_of[node_parent] = [node_child_0, node_child_1, ...]
    """
    children_of = dict()

    first_node = children_data[0]
    current_node = Node(first_node[0], first_node[1],
                        first_node[2], first_node[3], fanout)
    children_of_current_node = []

    for line in children_data:
        level, number, ip_address, port, neighbor_level, neighbor_number, neighbor_ip_address, neighbor_port = line

        node = Node(level, number, ip_address, port, fanout)
        if node != current_node:

            children_of[current_node] = children_of_current_node
            current_node = node
            children_of_current_node = []

        children_of_node = Node(
            neighbor_level, neighbor_number, neighbor_ip_address, neighbor_port, fanout)
        children_of_current_node.append(children_of_node)

    children_of[current_node] = children_of_current_node

    return children_of


def validate_parent_children_bidirectionality(parent_of, children_of):
    """
        a is child of b <=> b is parent of a
    """
    invalid_pairs = []
    valid = True

    for node_a in parent_of.keys():
        node_b = parent_of[node_a]
        children_of_b = children_of[node_b]
        parent_child_relationship_correct = node_a in children_of_b

        if not parent_child_relationship_correct:
            valid = False
            invalid_pairs.append([node_a, node_b, children_of_b])

    return valid, invalid_pairs


def validate_number_of_children_of_parent(children_of, max_level):
    """
        If the tree is perfectly balanced, each parent must have exactly fanout-many children.
        Otherwise each parent must have at most <= fanout-many children.
    """

    valid = True
    invalid_parent_nodes = []

    for node_a in children_of.keys():
        too_few_childs = False

        more_childs_than_possible = len(children_of[node_a]) > node_a.fanout
        if node_a.level <= (max_level-1):
            too_few_childs = len(children_of[node_a]) < node_a.fanout

        if more_childs_than_possible or too_few_childs:
            # TODO: Include this check again after fixing the data querying for this
            # valid = False
            invalid_parent_nodes.append(node_a)

    return valid, invalid_parent_nodes


def validate_children_positions_of_parent(children_of):
    """
        Checking if the children positions of a given parent are possible.

        Example with fanout 2:
            Parent 1:0 may only have 2:0 and 2:1 as children, but no other position.
    """

    valid = True
    invalid_pairs = []

    for node_a in children_of.keys():
        children_of_a = children_of[node_a]
        first_child_number = (node_a.number)*node_a.fanout
        possible_child_numbers = list(
            range(first_child_number, first_child_number+node_a.fanout))
        for child in children_of_a:
            child_level_correct = child.level == node_a.level+1
            child_number_correct = child.number in possible_child_numbers
            if not (child_level_correct and child_number_correct):
                valid = False
                invalid_pairs.append([node_a, child])

    return valid, invalid_pairs


def validate_parent_positions_of_child(parent_of):
    """
        Checking if the parent position of a given child are possible.

        Example with fanout 2:
            Child 2:2 may only have 1:1 as a parent.
    """
    valid = True
    invalid_pairs = []

    for node_a in parent_of.keys():
        parent = parent_of[node_a]
        parent_level_correct = parent.level == (node_a.level)-1
        parent_number_correct = parent.number == int(
            (node_a.number)/node_a.fanout)

        if not (parent_level_correct and parent_number_correct):
            valid = False
            invalid_pairs.append([parent, node_a])

    return valid, invalid_pairs


def get_deepest_level_for_event_id(event_id, sql_connector, general_db_information):

    existing_nodes = get_existing_nodes_for_event_id(event_id, sql_connector, general_db_information)
    deepest_level = 0

    for node in existing_nodes:
        if node.level > deepest_level:
            deepest_level = node.level

    return deepest_level

    # # id for event_id
    # id_statement = """
    #     SELECT MAX(id)
    #     FROM routing_infos
    #     WHERE event_id = {}
    # """.format(event_id[0])
    # id = sql_connector.fetch_one(id_statement)

    # id_condition = ""
    # if id:
    #     id_condition = "<="+str(id)

    # statement = """
    #     SELECT max(level)
    #     from routing_infos 
    #     WHERE id {}
    #     AND (level, number, ip_address) in(
    #         SELECT level, number, ip_address
    #         From peers
    #         where status = 1 and id in(
    #             SELECT max(id) from peers
    #             WHERE id {}
    #             group by level, number, ip_address
    #         )
    #     )
    #     AND (neighbor_level, neighbor_number, neighbor_ip_address) in(
    #         SELECT level, number, ip_address
    #         From peers
    #         where status = 1 and id in(
    #             SELECT max(id) from peers
    #             WHERE id {}
    #             group by level, number, ip_address
    #         ) 
    #     )
    # """.format(id_condition, id_condition, id_condition)

    # return sql_connector.fetch_one(statement)


def validate_parent_child(event_id, sql_connector, general_db_information):
    """
        Check if the parent-child relationships are correct
        for each node in the network for a given event id.

        Returns four boolean values, representing the correctness of:
            - bidirectionality between parent and child
            - the number of children a parent node provides
            - the position of the children for a parent node
            - the position of the parent for a child node
    """

    # getting raw data from database
    children_data = (get_children_data_for_event_id(event_id, sql_connector))
    parent_data = (get_parents_data_for_event_id(event_id, sql_connector))
    max_level = get_deepest_level_for_event_id(event_id, sql_connector, general_db_information)

    # check if parents / children nodes exist for the specific timestamp
    if len(children_data) == 0:
        return [True, ], [True, ], [True, ], [True, ]

    # creating the relationship dictionaries
    parent_of = get_parent_relationship_dict(
        parent_data, general_db_information.fanout)
    children_of = get_children_relationship_dict(
        children_data, general_db_information.fanout)

    # validating
    valid_bidirectionality = validate_parent_children_bidirectionality(
        parent_of, children_of)
    valid_children_number = validate_number_of_children_of_parent(
        children_of, max_level)
    valid_children_positions = validate_children_positions_of_parent(
        children_of)
    valid_parent_positions = validate_parent_positions_of_child(
        parent_of)

    return valid_bidirectionality, valid_children_number, valid_children_positions, valid_parent_positions
