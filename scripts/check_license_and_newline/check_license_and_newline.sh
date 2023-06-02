#!/bin/bash

# Copyright The SOLA Contributors
#
# Licensed under the MIT License.
# For details on the licensing terms, see the LICENSE file.
# SPDX-License-Identifier: MIT


# Script to check if all source and header files include correct license headers and end with newline
# Should be called from mono repository top-level directory

failed=0

# $1 = File path, $2 = License path, $3 License length
check_license_and_newline () {
    file_failed=0

    # Skip image files
    if [[ `basename $1` =~ \.(png|svg|jpg)$ ]]; then
        return
    fi

    # License check (only for source files)
    if [[ `basename $1` =~ \.(h|cpp)$ ]]; then
        cmp -s <(head -n $3 $1) <(cat $2)
        if [ $? -ne 0 ]; then
            echo MISSING LICENSE: $1
            file_failed=$((file_failed+1))
        fi
    fi

    # Newline check
    last=$(tail $1 -c 1) # Get last char in file
    if [ "$last" !=  "" ]; then
        echo MISSING NEWLINE: $1
        file_failed=$((file_failed+1))
    fi

    # Add to global variable if failed
    if [ $file_failed -ge 1 ]; then
        failed=$((failed+1))
    fi
}

# DAISI
for file in `find daisi -path daisi/third_party -prune -type f -o -name "*.*"`; do
    check_license_and_newline $file "scripts/check_license_and_newline/gpl.txt" 16
done

# natter
for file in `find natter -path natter/third_party -prune -type f -o -name "*.*"` ; do
    check_license_and_newline $file "scripts/check_license_and_newline/mit.txt" 6
done

# MINHTON
for file in `find minhton -path minhton/third_party -prune -type f -o -name "*.*"` ; do
    check_license_and_newline $file "scripts/check_license_and_newline/mit.txt" 6
done

# SOLA
for file in `find sola -path sola/third_party -prune -type f -o -name "*.*"` ; do
    check_license_and_newline $file "scripts/check_license_and_newline/mit.txt" 6
done

# SOLANet
for file in `find solanet -path solanet/third_party -prune -type f -o -name "*.*"` ; do
    check_license_and_newline $file "scripts/check_license_and_newline/mit.txt" 6
done

if [ $failed -ne 0 ]; then
    echo Failed files: $failed
    exit 1
fi
exit 0
