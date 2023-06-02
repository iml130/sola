# Copyright The SOLA Contributors
#
# Licensed under the MIT License.
# For details on the licensing terms, see the LICENSE file.
# SPDX-License-Identifier: MIT

from minhton.model.constants import MessageType, EventTypes


class CIEvaluationHelper:
    """
        Collection of sql-commands to support the tests of the ci-evaluation
    """

    def __init__(self, sql_connector):
        self.sql_connector = sql_connector
        # TODO Workaround by excluding find-queries (event type 3)
        # as they do not conflict with a join-sequence
        self.additional_statement_for_build = """
            AND (
                CASE
                    WHEN (SELECT COUNT(Id) from Event where Type != 0 AND Type != 3) > 0
                    THEN Timestamp_ms <
                        (SELECT MIN(Timestamp_ms) from Event where Type != 0 AND Type != 3)
                    ELSE Timestamp_ms
                    END
		    )"""

    def get_number_of_leave_events(self):
        # Only count successful leave events
        number_of_leave_events_statement = """
            SELECT COUNT(*) 
            FROM Event 
            WHERE Type = {}
            AND Uuid IN (
                SELECT RefEventId 
                FROM MinhtonTraffic 
                WHERE MsgType = {} 
                GROUP BY RefEventId
            )
        """.format(EventTypes.LEAVE.value, MessageType.REMOVE_NEIGHBOR.value)

        return self.sql_connector.fetch_one(number_of_leave_events_statement)[0]

    def get_number_of_leave_operations(self):
        """
        returns the number of nodes that left the network, according to the MinhtonNodeState table
        """
        statement = """
            SELECT COUNT(*)
            FROM MinhtonNodeState
            WHERE State = 2
            """
        return self.sql_connector.fetch_one(statement)[0]

    def get_number_of_join_operations(self):
        """
        returns the number of nodes that joined the network (root excluded), according to the MinhtonNodeState table
        """
        statement = """
            SELECT COUNT(*)
            FROM MinhtonNodeState
            WHERE State = 1
            """

        return self.sql_connector.fetch_one(statement)[0]-1

    def get_number_of_initial_join_events(self, build=False):
        """
        returns the number of nodes that try to join during join test,
        which is equal to the number of different ref_event_ids.
        """

        additional_statement = ""
        if build:
            additional_statement = self.additional_statement_for_build

        statement = """
            SELECT COUNT(*)
            FROM Event 
            WHERE Type = {} {}
            """.format(EventTypes.JOIN.value, additional_statement)
        return self.sql_connector.fetch_one(statement)[0]

    def get_number_of_joined_nodes(self, build=False):
        """
        returns the number of sucessfully joined nodes during join test,
        representated by sending a message with cmd_type=14
        """

        additional_statement = ""
        if build:
            additional_statement = self.additional_statement_for_build

        statement = """
            SELECT COUNT(MsgType) 
            from MinhtonTraffic
            where MsgType = {} {}
            """.format(MessageType.JOIN_ACCEPT_ACK.value, additional_statement)

        return self.sql_connector.fetch_one(statement)[0]/2

    def get_adresses_of_requesting_nodes(self, build=False):

        additional_statement = ""
        if build:
            additional_statement = self.additional_statement_for_build

        statement = """
            SELECT SIp
            From viewMinhtonTraffic
            Where RefEventId=0 AND MessageType = '{}' {}
            Group by EventId
            Order by Id
        """.format(MessageType.JOIN.name, additional_statement)
        MessageType.JOIN
        return self.sql_connector.fetch_all(statement)

    def get_adresses_of_joined_nodes(self, build=False):

        additional_statement = ""
        if build:
            additional_statement = self.additional_statement_for_build

        statement = """
            SELECT SIp from(
                SELECT MIN(Id), SIp
                from viewMinhtonTraffic
                WHERE MessageType = '{}' {}
                group by RefEventId
                order by Id
            )""".format(MessageType.JOIN_ACCEPT_ACK.name, additional_statement)
        return self.sql_connector.fetch_all(statement)

    def get_nodes_in_peers(self, build=False):

        additional_statement = ""
        if build:
            additional_statement = self.additional_statement_for_build

        statement = """
        SELECT Level, Number, Ip from viewMinhtonNodeState 
        WHERE State='RUNNING' {}""".format(additional_statement)
        return self.sql_connector.fetch_all(statement)

    def get_joined_nodes(self, build=False):

        additional_statement = ""
        if build:
            additional_statement = self.additional_statement_for_build

        statement = """
            SELECT SLevel, SNumber, SIp
            FROM viewMinhtonTraffic
            WHERE MessageType='{}' and Mode=1 {}
        """.format(MessageType.JOIN_ACCEPT_ACK.name, additional_statement)
        return self.sql_connector.fetch_all(statement)

    def get_event_ids(self):
        """
        Getting the event ids of each join and leave event from the event table.
        """
        statement = """
            SELECT Uuid, Type
            FROM Event
        """

        return self.sql_connector.fetch_all(statement)

    def get_event_timestamps(self):
        """
        Getting the timestamps of each join and leave event from the event table.
        """
        statement = """
            SELECT Timestamp_ms, Type
            FROM Event
        """

        return self.sql_connector.fetch_all(statement)

    def get_operations_for_event_at_timestamp(self, timestamp):
        """
        Returns the join and leave operations for one event_timestamp
        """
        statement = """
            SELECT COUNT(*)
            FROM MinhtonNodeState
            WHERE Timestamp_ms >= {}
            AND CASE
                    WHEN (SELECT COUNT(Id) from Event where Timestamp_ms > {}) > 0
                    THEN Timestamp_ms <
                        (SELECT min(Timestamp_ms) from Event where Timestamp_ms > {})
                    ELSE Timestamp_ms
                    END
            GROUP BY State
        """.format(timestamp, timestamp, timestamp)

        return self.sql_connector.fetch_all(statement)

    def get_entrys_in_peers_for_event_at_timestamp(self, timestamp):
        statement = """
            SELECT Level, Number, Ip, Port, State
            FROM viewMinhtonNodeState
            WHERE Timestamp_ms >= {}
            AND CASE
                    WHEN (SELECT COUNT(Id) from Event where Timestamp_ms > {}) > 0
                    THEN Timestamp_ms <
                        (SELECT min(Timestamp_ms) from Event where Timestamp_ms > {})
                    ELSE Timestamp_ms
                    END
        """.format(timestamp, timestamp, timestamp)

        return self.sql_connector.fetch_all(statement)
