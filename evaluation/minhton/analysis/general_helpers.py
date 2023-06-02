# Copyright The SOLA Contributors
#
# Licensed under the MIT License.
# For details on the licensing terms, see the LICENSE file.
# SPDX-License-Identifier: MIT

from minhton.analysis.node import Node


def get_existing_nodes_for_event_id(event_id, sql_connector, general_db_information):
    """
    Getting all nodes that are currently in the network
    """
    nodes_that_left = get_nodes_that_left_the_network_up_to_event_id(
        event_id, sql_connector)
    nodes_in_peers = get_all_joined_nodes_up_to_event_id(
        event_id, sql_connector)

    for node in nodes_that_left:
        nodes_in_peers.remove(
            (next(existing_node for existing_node in nodes_in_peers if existing_node == node)))

    existing_nodes = []
    for node in nodes_in_peers:
        existing_nodes.append(
            Node(node[0], node[1], node[2], node[3], general_db_information.fanout))
    return existing_nodes


def get_nodes_that_left_the_network_up_to_event_id(event_id, sql_connector):
    if event_id is None:
        id = None
    else:
        id_statement = """
            SELECT MAX(id)
            FROM viewMinhtonNodeState
            WHERE EventId = {}
            """.format(event_id[0])
        id = sql_connector.fetch_one(id_statement)[0]

    id_condition = ""
    if id:
        id_condition = "<=" + str(id)

    statement = """
        SELECT Level, Number, Ip, Port 
        FROM viewMinhtonNodeState 
        WHERE State = 'LEFT' AND Id {}""".format(id_condition)

    return sql_connector.fetch_all(statement)


def get_all_joined_nodes_up_to_event_id(event_id, sql_connector):
    if event_id is None:
        id = None
    else:
        id_statement = """
            SELECT MAX(Id)
            FROM viewMinhtonNodeState
            WHERE EventId = {}
            """.format(event_id[0])
        id = sql_connector.fetch_one(id_statement)[0]

    id_condition = ""
    if id:
        id_condition = "<=" + str(id)

    statement = """
        SELECT Level, Number, Ip, Port
        FROM viewMinhtonNodeState
        WHERE State = 'RUNNING' AND Id {}""".format(id_condition)

    nodes = sql_connector.fetch_all(statement)
    return nodes


def get_sql_data(sql_connector, specific_statement, timestamp=None):
    """
        Executes a given SQL-statement with an addition
        so the database only returns data of nodes that exist at the passed timestamp.
    """

    timestamp_condition = ""
    if timestamp:
        timestamp_condition = "<"+str(timestamp[0])

    statement = specific_statement + """
        AND (NodeLevel, NodeNumber, NodeIp, NodePort) in(
            SELECT Level, Number, Ip, Port 
            FROM viewMinhtonNodeState 
            WHERE Timestamp_ms in(
                SELECT max(Timestamp_ms) 
                FROM viewMinhtonNodeState
                WHERE Timestamp_ms {} group by Level, Number, Ip, Port) and State = 'RUNNING')
        AND (NeighborLevel, NeighborNumber, NeighborIp, NeighborPort) in(
            SELECT Level, Number, Ip, Port 
            FROM viewMinhtonNodeState 
            WHERE Timestamp_ms in(
                SELECT max(Timestamp_ms) 
                FROM viewMinhtonNodeState
                WHERE Timestamp_ms {} group by Level, Number, Ip, Port) and State = 'RUNNING')
        GROUP BY NodeLevel, NodeNumber, NeighborLevel, NeighborNumber
    """.format(timestamp_condition, timestamp_condition)

    sql_data = sql_connector.fetch_all(statement)

    return sql_data


def get_sql_data_unfiltered(sql_connector, specific_statement):
    """
        Executes a given SQL-statement
    """
    sql_data = sql_connector.fetch_all(specific_statement)

    return sql_data
