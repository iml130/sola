# Copyright The SOLA Contributors
#
# Licensed under the MIT License.
# For details on the licensing terms, see the LICENSE file.
# SPDX-License-Identifier: MIT

from utils.sqlite_db import SqliteDb
import re


def get_ordered_intersections_with_times(db: SqliteDb):
    return db.fetch_all("SELECT IntersectX, IntersectY, Time_s FROM IntersectOccupancy ORDER BY IntersectX, IntersectY, Time_s")


def get_multi_occupancy_intersects(db: SqliteDb):
    return db.fetch_all("SELECT IntersectX, IntersectY FROM IntersectOccupancy GROUP BY IntersectX, IntersectY HAVING COUNT(*) > 1 ")


def get_times_intersection(db: SqliteDb, inters):
    query = "SELECT Time_s FROM IntersectOccupancy WHERE IntersectX = {} AND IntersectY = {} ORDER by Time_s".format(
        inters[0], inters[1])
    return db.fetch_all(query)


def get_delta_time(db: SqliteDb) -> float:
    content = db.fetch_one("SELECT Config FROM General")
    return float(re.findall("(?<=timeBetweenIntersections: )[0-9.]*", content[0])[0])


def is_paxos(db: SqliteDb) -> bool:
    content = db.fetch_one("SELECT Config FROM General")[0]
    return "paxos" == re.findall("(?<=consensus: )[A-Za-z]*", content)[0]


def replication_active(db: SqliteDb) -> bool:
    content = db.fetch_one("SELECT Config FROM General")[0]
    return int(re.findall("(?<=paxosReplication: )[0-1]", content)[0]) == 1


def get_to_state_count(db: SqliteDb) -> bool:
    return db.fetch_all("SELECT COUNT(*) FROM TransportOrderHistory WHERE State=0 OR State=2 OR State=3 OR State=4 GROUP BY State")


def get_number_replications(db: SqliteDb) -> bool:
    return db.fetch_one("SELECT COUNT(*) FROM PathPlanningReplication")[0]


def get_count_unique_instances(db: SqliteDb) -> bool:
    return db.fetch_one("SELECT COUNT(DISTINCT Instance) FROM PathPlanningReplication")[0]


def get_last_instance_id_replication(db: SqliteDb) -> bool:
    return db.fetch_one("SELECT DISTINCT Instance FROM PathPlanningReplication ORDER BY Instance DESC LIMIT 1")[0]


def get_last_instance_id_finished(db: SqliteDb) -> bool:
    return db.fetch_one("SELECT DISTINCT Instance FROM PathPlanningFinishedConsensus ORDER BY Instance DESC LIMIT 1")[0]


def get_count_replication_per_instance(db: SqliteDb) -> bool:
    return db.fetch_all("SELECT COUNT(*) FROM PathPlanningReplication GROUP BY Instance, ProposalId, StationId;")


def get_number_tos(db: SqliteDb) -> int:
    return db.fetch_one("SELECT COUNT(*) FROM PathPlanningFinishedConsensus")[0]


def get_number_finished_consensus(db: SqliteDb) -> int:
    return db.fetch_one("SELECT COUNT(*) FROM TransportOrder")[0]


def get_consensus(db: SqliteDb):
    content = db.fetch_one("SELECT Config FROM General")[0]
    consensus = re.findall("(?<=consensus: )[A-Za-z]*", content)[0]
    agv = int(re.findall("(?<=agvs: )[0-9]*", content)[0])
    return consensus, agv


def get_number_delivery_stations(db: SqliteDb):
    content = db.fetch_one("SELECT Config FROM general")[0]
    return int(re.findall("(?<=deliveryStations: )[1-9]*[0-9]*", content)[0])
