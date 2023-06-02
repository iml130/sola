# Copyright The SOLA Contributors
#
# Licensed under the MIT License.
# For details on the licensing terms, see the LICENSE file.
# SPDX-License-Identifier: MIT

import sqlite3
import os.path


class SqliteDb():
    """
        Opens a sql-connection and contains methods to easily execute sql-commands.
    """

    def __init__(self, filename):
        self.filename = filename
        self.is_open = False
        if not self.filename.endswith(".db"):
            raise TypeError("File type must end with *.db")
        if not os.path.isfile(self.filename):
            raise FileNotFoundError

    def __enter__(self):
        self.conn = sqlite3.connect(self.filename)
        self.cursor = self.conn.cursor()
        self.is_open = True
        return self

    def __exit__(self, exc_type, exc_value, traceback):
        if self.is_open:
            self.conn.close()
            self.is_open = False

    def execute_internal(self, sql_cmd, cursor_fct):
        if not self.is_open:
            raise ConnectionError("no open sql_connection")
        try:
            self.cursor.execute(sql_cmd)
            return cursor_fct()
        except AttributeError:
            raise AttributeError("no sql_cursor")
        except ValueError:
            raise ValueError("no sql_command")

    def fetch_one(self, sql_cmd):
        """
            Returns one line from a sql database.
        """
        return self.execute_internal(sql_cmd, self.cursor.fetchone)

    def fetch_all(self, sql_cmd):
        """
            Returns multiple lines from a sql database.
        """
        return self.execute_internal(sql_cmd, self.cursor.fetchall)


def get_all_db(path):
    if os.path.isfile(path):
        return [path]

    db_list = []
    for filename in os.listdir(path):
        if filename.endswith(".db"):
            db_list.append(os.path.join(path, filename))

    return db_list
