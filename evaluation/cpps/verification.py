# Copyright The SOLA Contributors
#
# Licensed under the MIT License.
# For details on the licensing terms, see the LICENSE file.
# SPDX-License-Identifier: MIT

from utils.sqlite_db import SqliteDb


def evaluate_database(path):
    db = SqliteDb(path)
    valid = True
    with db:
        result = db.fetch_all(
            "SELECT COUNT(*) FROM TransportOrderHistory GROUP by TransportOrderId")
        for res in result:
            valid &= (res[0] == 12)

    return valid
