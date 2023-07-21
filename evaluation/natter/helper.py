# Copyright The SOLA Contributors
#
# Licensed under the MIT License.
# For details on the licensing terms, see the LICENSE file.
# SPDX-License-Identifier: MIT

from utils.sqlite_db import SqliteDb


def get_node_count(db: SqliteDb):
    return db.fetch_one("SELECT NumberOfNodes FROM General")[0]


def get_number_publish_events(db: SqliteDb):
    return db.fetch_one("SELECT COUNT(*) FROM Event WHERE Type=258")[0]


def get_number_received_msgs(db: SqliteDb):
    return db.fetch_one("SELECT COUNT(*) FROM NatterDeliveredTopicMessage")[0]


def get_unique_msgs(db: SqliteDb):
    return db.fetch_all("SELECT DISTINCT MessageId, InitialSenderNodeId FROM NatterDeliveredTopicMessage")


def get_unique_msg_ids_from_traffic(db: SqliteDb):
    return db.fetch_one("SELECT COUNT(DISTINCT MessageId) FROM NatterControlMessage")[0]


def get_unique_msg_ids_from_publish_events(db: SqliteDb):
    return db.fetch_one("SELECT COUNT(*) FROM NatterMessage")[0]


def get_received_node_ids(db: SqliteDb, message_id):
    return db.fetch_all("SELECT DISTINCT NodeId FROM NatterDeliveredTopicMessage WHERE MessageId={}".format(
        message_id))
