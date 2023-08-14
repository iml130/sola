# Copyright The SOLA Contributors
#
# Licensed under the MIT License.
# For details on the licensing terms, see the LICENSE file.
# SPDX-License-Identifier: MIT

from utils.sqlite_db import SqliteDb


def evaluate_database(path):
    try:
        db = SqliteDb(path)
        valid = True
        with db:
            # check if there are 12 states for every mf
            result = db.fetch_all(
                "SELECT COUNT(*) FROM MaterialFlowOrderHistory GROUP by MaterialFlowOrderId")
            for res in result:
                valid &= (res[0] == 12)
            # check if all states of all mfs are ordered correctly
            [[mf_count]] = db.fetch_all(
                "SELECT COUNT(DISTINCT MaterialFlowOrderId) FROM MaterialFlowOrderHistory")
            for i in range(1, mf_count+1):
                sql_query = "SELECT Timestamp_ms, State FROM MaterialFlowOrderHistory WHERE MaterialFlowOrderId = " + \
                    str(i) + " ORDER BY Timestamp_ms, State"
                result = db.fetch_all(sql_query)
                # check if all states are met
                j = 0
                element_valid = True

                for res in result:
                    # allow multiple load actions
                    if j == 7 and res[1] == 3:
                        j = 3
                    else:
                        element_valid &= (res[1] == j)
                    if j > 0:
                        element_valid &= last_timestamp <= res[0]
                    j += 1
                    last_timestamp = res[0]
                # check if last state was kFinished(11)
                element_valid &= j == 12
                if not element_valid:
                    print('\tCondition not met for: "' + sql_query + ';"')
                valid &= element_valid
    except Exception as e:
        print(e)
        return False
    return valid
