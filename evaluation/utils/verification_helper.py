# Copyright The SOLA Contributors
#
# Licensed under the MIT License.
# For details on the licensing terms, see the LICENSE file.
# SPDX-License-Identifier: MIT

from utils.sqlite_db import get_all_db
import sys


def evaluate_databases(evaluate_fct):

    if (len(sys.argv) != 2):
        print("Pass database path as argument!")
        exit(1)
    databases = get_all_db(sys.argv[1])

    print("Evaluation...")
    result = dict.fromkeys(databases, False)
    success = True
    for database in databases:
        result[database] = evaluate_fct(database)
        if not result[database]:
            print(database + ": failed")
        else:
            print(database + ": success")
        success &= result[database]
    print("No errors!" if success else "Errors!")
    return success
