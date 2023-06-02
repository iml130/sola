# Copyright The SOLA Contributors
#
# Licensed under the MIT License.
# For details on the licensing terms, see the LICENSE file.
# SPDX-License-Identifier: MIT

from utils.sqlite_db import SqliteDb

class GeneralDbInformation():
    """
        Collects general information about the network,
        more precisely the networks fanout and size.
    """

    def __init__(self, db: SqliteDb):
        self.total_number_of_nodes = self.get_network_size(db)
        self.fanout = self.get_fanout(db)
        self.max_height = self.get_max_height(db)
        self.max_number = self.get_max_number(db)

    def get_network_size(self, db):
        statement = """
        SELECT NumberOfNodes 
        FROM General"""
        return db.fetch_one(statement)[0]

    def get_fanout(self, db):
        statement = """
        SELECT Fanout
        FROM General"""
        return db.fetch_one(statement)[0]

    def get_max_height(self, db):
        statement = """
        SELECT MAX(Level)
        FROM viewNode"""
        return db.fetch_one(statement)[0]

    def get_max_number(self, db):
        statement = """
        SELECT MAX(Number)
        FROM viewNode"""
        return db.fetch_one(statement)[0]
