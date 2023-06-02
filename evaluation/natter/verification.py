# Copyright The SOLA Contributors
#
# Licensed under the MIT License.
# For details on the licensing terms, see the LICENSE file.
# SPDX-License-Identifier: MIT

from utils.sqlite_db import SqliteDb
import natter.helper as helper


def check(result, error_msg):
    if not result:
        print(error_msg)
    return result


def evaluate_database(path):
    result = True
    db = SqliteDb(path)
    with db:
        number_nodes = helper.get_node_count(db)
        unique_msgs = helper.get_unique_msgs(db)

        # Check if all nodes received the message
        for msg_id in unique_msgs:
            res = helper.get_received_node_ids(db, msg_id[0])
            nodes_received = set([node[0] for node in res])
            result &= check(msg_id[1] not in nodes_received,
                            "Sender received own message")
            result &= check(len(nodes_received) == number_nodes -
                            1, "Not all nodes received message")

        unique_senders = helper.get_unique_msg_ids_from_traffic(db)

        number_msg_ids = helper.get_unique_msg_ids_from_publish_events(db)

        result &= check(unique_senders == number_nodes,
                        "N nodes did not send N messages")
        result &= check(number_msg_ids == number_nodes,
                        "N nodes did not send N messages")

    return result
