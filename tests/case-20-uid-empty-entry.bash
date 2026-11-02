#!/usr/bin/env bash
# An empty entry in a -u list must not be taken as UID 0.
source tests/common.bash

{
    for spec in ",99999" "^"; do
        output=$($lsof -u "$spec" -F u 2>/dev/null)
        status=$?
        if echo "$output" | grep -q '^u0$'; then
            echo "lsof -u $spec selected UID 0" >> $report
            exit 1
        fi
        if [ $status -eq 0 ]; then
            echo "lsof -u $spec should fail" >> $report
            exit 1
        fi
    done
    exit 0
} > $report 2>&1
