# Copyright The SOLA Contributors
#
# Licensed under the MIT License.
# For details on the licensing terms, see the LICENSE file.
# SPDX-License-Identifier: MIT

from utils.sqlite_db import SqliteDb
import path_planning.helper as helper

import math


def evaluate_database(path) -> bool:
    result = True
    db = SqliteDb(path)
    with db:
        # intersections = get_multi_occupancy_intersects(db) # TODO Check start/stop points occupied accordingly
        result = check_intersection_occupancy(db)

        result &= check_tos_finished(db)
        if helper.is_paxos(db):
            result &= check_number_instances(db)
            if helper.replication_active(db):
                result &= check_replication(db)
    return result


def check_replication(db):
    _, number_agvs = helper.get_consensus(db)
    number_delivery = helper.get_number_delivery_stations(db)
    target_replication_number = helper.get_number_finished_consensus(
        db) * (number_agvs + number_delivery)
    is_replication_number = helper.get_number_replications(db)
    correctly_replicated = target_replication_number == is_replication_number

    # Check if unique instances
    number_unique_instances = helper.get_count_unique_instances(db)
    last_instance_id_replication = helper.get_last_instance_id_replication(db)
    last_instance_id_finished = helper.get_last_instance_id_finished(db)
    correct_instance_ids = last_instance_id_replication == last_instance_id_finished == (
        number_unique_instances-1)

    number_replication_received_per_instance = helper.get_count_replication_per_instance(
        db)
    result = True
    target = number_agvs + number_delivery
    for rep in number_replication_received_per_instance:
        result &= rep[0] == target
    result &= number_unique_instances == len(
        number_replication_received_per_instance)
    return correctly_replicated and correct_instance_ids and result


def check_tos_finished(db) -> bool:
    number_tos = helper.get_number_tos(db)
    states = helper.get_to_state_count(db)
    return number_tos == states[0][0] == states[1][0] == states[2][0] == states[3][0]


def check_number_instances(db) -> bool:
    return helper.get_number_finished_consensus(db) == helper.get_number_tos(db)


def check_intersection_occupancy(db) -> bool:
    result = True
    delta_time = helper.get_delta_time(db)
    intersections = helper.get_ordered_intersections_with_times(db)
    for index, _ in enumerate(intersections):
        if index == len(intersections)-1:
            break
        if intersections[index][0] == intersections[index+1][0] and intersections[index][1] == intersections[index+1][1]:
            diff = intersections[index+1][2] - intersections[index][2]
            # Use math.isclose to avoid floating point precision errors
            valid = (diff >= delta_time or math.isclose(diff, delta_time))
            if not valid:
                print("ERROR at intersection {}:{} at times {} and {}".format(
                    intersections[index][0], intersections[index][1], intersections[index][2], intersections[index+1][2]))
            result &= valid
    return result
