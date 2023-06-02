# Copyright The SOLA Contributors
#
# Licensed under the MIT License.
# For details on the licensing terms, see the LICENSE file.
# SPDX-License-Identifier: MIT

from minhton.model.general_db_information import *
from minhton.helper import CIEvaluationHelper
from minhton.analysis.general_helpers import get_existing_nodes_for_event_id
from minhton.analysis.tree import validate_height_balanced_structure, validate_tree_structure
import sys


class GeneralJoinCollection():
    """
        Collection of general join tests for the ci_evaluation.
    """

    def __init__(self, sql_connector, general_information):
        self.sql_connector = sql_connector
        self.general_information = general_information
        self.evaluation_helpers = CIEvaluationHelper(self.sql_connector)

    def has_leave_events(self):
        """
            Determine wheter leave operations are executed on the database.
        """
        return self.evaluation_helpers.get_number_of_leave_events() > 0

    def check_for_join_plausability_during_build_process(self):
        """
            Executes a series of tests to determine whether
            the join processes during the build procedure were executed correctly.
        """

        if not (self.has_leave_events()):
            # don't execute these tests if there are leave events,
            # because the correctness can't be guaranteed

            if not self.check_number_of_initial_join_events_equals_nodes_in_network(build=True):
                return False
            if not self.check_joined_nodes_equals_nodes_in_network(build=True):
                return False
        if not self.check_target_of_join_accept_ack(build=True):
            return False
        if not self.compare_entries_of_peers_with_joined_nodes(build=True):
            return False

        return True

    def check_for_general_join_plausability(self):
        """
            Executes a series of tests, to check if the join procedures worked also correctly
            in connection with leave operations.
        """

        if self.has_leave_events():
            if not self.check_number_of_join_events_less_leave_events_is_not_negative():
                return False
            if not self.check_number_of_joined_nodes_less_leave_events_is_not_negative():
                return False
        if not self.check_target_of_join_accept_ack():
            return False
        if not self.compare_entries_of_peers_with_joined_nodes():
            return False
        return True

    def check_number_of_join_events_less_leave_events_is_not_negative(self):
        number_of_join_events = self.evaluation_helpers.get_number_of_initial_join_events(
            build=False)
        number_of_leave_events = self.evaluation_helpers.get_number_of_leave_events()

        number_of_nodes = number_of_join_events-number_of_leave_events

        if number_of_nodes < 0:
            print("""Join with leave Test 1 failed:
                the number of join-processes less the leave-processes
                is less than 0, what should be impossible!
            """)
            return False
        return True

    def check_number_of_joined_nodes_less_leave_events_is_not_negative(self):
        number_of_joined_nodes = self.evaluation_helpers.get_number_of_joined_nodes(
            build=False)
        number_of_leave_events = self.evaluation_helpers.get_number_of_leave_events()

        number_of_nodes = number_of_joined_nodes-number_of_leave_events

        if number_of_nodes < 0:
            print("""Join with leave Test 2 failed:
                the number of successfully joined nodes less the leave-processes
                is less than 0, what should be impossible!
            """)
            return False
        return True

    def check_number_of_initial_join_events_equals_nodes_in_network(self, build):
        number_of_initial_join_events = self.evaluation_helpers.get_number_of_initial_join_events(
            build)
        if number_of_initial_join_events != self.general_information.total_number_of_nodes-1:
            print("""Join build Test 1 failed:
            \nthe number of join-processes to build up the network
             is not equal to the number of nodes in the network (except root)!
            """)
            return False
        return True

    def check_joined_nodes_equals_nodes_in_network(self, build):
        number_of_joined_nodes = self.evaluation_helpers.get_number_of_joined_nodes(
            build)
        if number_of_joined_nodes != self.general_information.total_number_of_nodes-1:
            print("""Join build Test 2 failed:
            \nthe number of joined nodes during the build process 
            is not equal to the number of nodes in the network (exept root)!
            """)
            return False
        return True

    def check_target_of_join_accept_ack(self, build=False):
        # concretize the error message
        if build:
            test_mode = "build"
        else:
            test_mode = "with leave"

        joining_nodes = self.evaluation_helpers.get_adresses_of_requesting_nodes(
            build)
        joined_nodes = self.evaluation_helpers.get_adresses_of_joined_nodes(
            build)
        if joining_nodes != joined_nodes:
            print("Join " + test_mode + """ Test 3 failed:
            \nthe node that wants to join the network is not always the one that sends the join_accept_ack!
            """)
            return False
        return True

    def compare_entries_of_peers_with_joined_nodes(self, build=False):
        entries_in_peers = self.evaluation_helpers.get_nodes_in_peers(build)
        joined_nodes = self.evaluation_helpers.get_joined_nodes(build)
        if build:
            if entries_in_peers[1:] != joined_nodes:
                print("""Join build Test 4 failed:
                \nthe position of the nodes in peers (level, number)
                are not equal to the position of the nodes that joined the network!
                """)
        else:
            for node in joined_nodes:
                if node not in entries_in_peers:
                    print("""Join with leave Test 4 failed:
                    \nnot each join procedure was noted in the peers table!""")
                    return False
        return True

    def analyse_tree(self):
        event_ids = self.evaluation_helpers.get_event_ids()

        for i in range(len(event_ids)):
            if i < len(event_ids)-1:
                event_id = event_ids[i+1]
            else:
                event_id = None

            existing_nodes = get_existing_nodes_for_event_id(
                event_id, self.sql_connector, self.general_information)

            correctly_balanced = validate_tree_structure(
                event_id, existing_nodes, self.sql_connector)
            if not correctly_balanced:
                return False

        return True

    def analyse_tree_height_balanced(self):
        event_ids = self.evaluation_helpers.get_event_ids()

        for i in range(len(event_ids)):
            if i < len(event_ids)-1:
                event_id = event_ids[i+1]
            else:
                event_id = None

            existing_nodes = get_existing_nodes_for_event_id(
                event_id, self.sql_connector, self.general_information)

            height_balanced = validate_height_balanced_structure(
                existing_nodes)

            if not height_balanced:
                print("Not height balanced for event id",
                      event_id[0], file=sys.stderr)
                return False

        return True
