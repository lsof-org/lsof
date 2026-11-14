#!/usr/bin/env bash
# An empty entry in a -p or -g list must not be taken as ID 0, and an empty
# -p list must not select every process.
source tests/common.bash

check() {
    output=$($lsof "$1" "$2" -F p 2>/dev/null)
    status=$?
    if echo "$output" | grep -q '^p'; then
        echo "lsof $1 '$2' listed a process"
        exit 1
    fi
    if [ $status -eq 0 ]; then
        echo "lsof $1 '$2' should fail"
        exit 1
    fi
}

{
    pgid=$(ps -o pgid= -p $$ | tr -d ' ')
    for spec in "" ",$$" "$$,,$$" "^"; do
        check -p "$spec"
    done
    for spec in ",$pgid" "$pgid,,$pgid" "^"; do
        check -g "$spec"
    done
    exit 0
} > $report 2>&1
