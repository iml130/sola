# Copyright The SOLA Contributors
#
# Licensed under the MIT License.
# For details on the licensing terms, see the LICENSE file.
# SPDX-License-Identifier: MIT

from utils.verification_helper import evaluate_databases
import path_planning.verification as verification


if __name__ == '__main__':
    sucess = evaluate_databases(evaluate_fct=verification.evaluate_database)
    exit(0 if sucess else 1)
