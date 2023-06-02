# Copyright The SOLA Contributors
#
# Licensed under the MIT License.
# For details on the licensing terms, see the LICENSE file.
# SPDX-License-Identifier: MIT

from minhton.model.general_db_information import GeneralDbInformation
from utils.sqlite_db import SqliteDb
from minhton.analysis.general_join import GeneralJoinCollection
from minhton.analysis.relationships import RelationshipTestCollection


def evaluate_database(path):
    db = SqliteDb(path)

    with db:
        general_information = GeneralDbInformation(db)
        general_join_collection = GeneralJoinCollection(
            db, general_information)
        relationship_collection = RelationshipTestCollection(
            db, general_information)

        valid_network = True

        # filter only events until the tree is completely build up
        built_correctly = general_join_collection.check_for_join_plausability_during_build_process()

        if general_join_collection.has_leave_events():
            # for validate-leave, there are some additional join procedures to check
            correct_with_leave = general_join_collection.check_for_general_join_plausability()
        else:
            # no leave events
            correct_with_leave = True

        if not (correct_with_leave and built_correctly):
            valid_network = False

        if not relationship_collection.run_relationship_validation():
            valid_network = False

        if not general_join_collection.analyse_tree():
            valid_network = False

    return valid_network
