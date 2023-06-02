# Copyright The SOLA Contributors
#
# Licensed under the MIT License.
# For details on the licensing terms, see the LICENSE file.
# SPDX-License-Identifier: MIT

from math import log
import sys
import random
from minhton.helper import CIEvaluationHelper
from minhton.analysis.parent_child import validate_parent_child
from minhton.analysis.adjacents import validate_adjacents
from minhton.analysis.routing_tables import validate_routing_table_information

THRESHOLD_NODES_SKIP_EVENTS = 2000


class RelationshipTestCollection():
    """
        Collection of general join tests for the ci_evaluation.
    """

    def __init__(self, sql_connector, general_information):
        self.sql_connector = sql_connector
        self.general_db_information = general_information
        self.evaluation_helpers = CIEvaluationHelper(self.sql_connector)

    def run_relationship_validation(self):
        """
            validate the correctness of the nodes relationships.
            Check if:
                - parent / child connections
                - adjacents
                - routing table neighbors and routing table neighbor children
            are correct for each node in the network and for each ref event id.

            Returns True if no errors occure or if so, 
            an error message with information about the ref event id and the error type. 
        """

        event_ids = self.evaluation_helpers.get_event_ids()

        for i in range(len(event_ids)):
            event_id = event_ids[i]

            # Randomly skip some events for larger databases (50% for 2000 events, 75% for 14.000 events)
            threshold_probability = 0.5 * log(len(event_ids) / 40, 50)
            if len(event_ids) > THRESHOLD_NODES_SKIP_EVENTS and random.random() < threshold_probability:
                continue

            valid_parent_child_relationships = self.run_parent_child_validation(
                event_id)

            valid_adjacent_relationships = self.run_adjacent_validation(
                event_id)

            valid_routing_table_relationships = self.run_routing_table_validation(
                event_id)

            if not (valid_parent_child_relationships and valid_adjacent_relationships
                    and valid_routing_table_relationships):
                print("Failed for event with id " +
                      str(event_ids[i][0]))
                return False

        return True

    def run_parent_child_validation(self, event_id):
        parent_child_results = validate_parent_child(
            event_id, self.sql_connector, self.general_db_information)

        valid_bidirectionality = parent_child_results[0]
        valid_children_number = parent_child_results[1]
        valid_children_positions = parent_child_results[2]
        valid_parent_positions = parent_child_results[3]

        valid_parent_child_relationships = True

        if not valid_bidirectionality[0]:
            valid_parent_child_relationships = False
            print("Bidirectional condition for parent-child relationship attached for event id",
                  event_id[0], file=sys.stderr)
            self.print_invalid_pairs(valid_bidirectionality[1])

        if not valid_children_number[0]:
            valid_parent_child_relationships = False
            print("At least one node has a invalid number of children for event id",
                  event_id[0], file=sys.stderr)
            self.print_invalid_pairs(valid_children_number[1])

        if not valid_children_positions[0]:
            valid_parent_child_relationships = False
            print("At least one node has children on invalid positions for event id",
                  event_id[0], file=sys.stderr)
            self.print_invalid_pairs(valid_children_positions[1])

        if not valid_parent_positions[0]:
            valid_parent_child_relationships = False
            print("At least one node has a parent on a invalid position for event id",
                  event_id[0], file=sys.stderr)
            self.print_invalid_pairs(valid_parent_positions[1])

        return valid_parent_child_relationships

    def run_adjacent_validation(self, event_id):
        adjacent_results = validate_adjacents(
            event_id, self.sql_connector, self.general_db_information)

        valid_bidirectionality = adjacent_results[0]
        valid_locations = adjacent_results[1]
        valid_boundaries = adjacent_results[2]

        valid_adjacents = True

        if not valid_bidirectionality[0]:
            valid_adjacents = False
            print("Bidirectional condition for adjacents attached for event id",
                  event_id[0] if event_id else "full_db", file=sys.stderr)
            self.print_invalid_pairs(valid_bidirectionality[1])

        if not valid_locations[0]:
            valid_adjacents = False
            print("Location condition for adjacents attached for event id",
                  event_id[0] if event_id else "full_db", file=sys.stderr)
            self.print_invalid_pairs(valid_locations[1])

        if not valid_boundaries[0]:
            valid_adjacents = False
            print("Boundary condition for adjacents attached for event id",
                  event_id[0] if event_id else "full_db", file=sys.stderr)
            self.print_invalid_pairs(valid_boundaries[1])

        return valid_adjacents

    def run_routing_table_validation(self, event_id):
        routing_table_results = validate_routing_table_information(
            event_id, self.sql_connector, self.general_db_information)

        valid_rt_neighbor_positions = routing_table_results[0]
        valid_bidirectionality = routing_table_results[1]
        valid_rt_neighbor_children_positions = routing_table_results[2]
        valid_rt_neighbor_children = routing_table_results[3]

        valid_routing_tables = True

        if not valid_bidirectionality[0]:
            valid_routing_tables = False
            print("Bidirectional condition for routing table neighbors attached for event id",
                  event_id[0], file=sys.stderr)
            self.print_invalid_pairs(valid_bidirectionality[1])

        if not valid_rt_neighbor_positions[0]:
            valid_routing_tables = False
            print("At least one node has a routing table neighbor on a invalid position for event id",
                  event_id[0], file=sys.stderr)
            self.print_invalid_pairs(valid_rt_neighbor_positions[1])

        if not valid_rt_neighbor_children_positions[0]:
            valid_routing_tables = False
            print(
                "At least one node has a routing_table_neighbor_children on a invalid position for event id", event_id[0], file=sys.stderr)
            self.print_invalid_pairs(valid_rt_neighbor_children_positions[1])

        if not valid_rt_neighbor_children[0]:
            valid_routing_tables = False
            print(
                "At least one node has not exactly all children of its rt_neighbors as rt_neighbor_children for event id", event_id[0], file=sys.stderr)
            self.print_invalid_pairs(valid_rt_neighbor_children[1])

        return valid_routing_tables

    def print_invalid_pairs(self, invalid_pairs):
        print("Invalid pairs:")
        for invalid_pair in invalid_pairs:
            print("{")
            try:
                iter(invalid_pair)
                for node in invalid_pair:
                    try:
                        iter(node)
                        print("children of node b:")
                        for entry in node:
                            print("        "+str(entry))
                    except:
                        print("    "+str(node))
            except:
                print("    "+str(invalid_pair))
            print("}")
