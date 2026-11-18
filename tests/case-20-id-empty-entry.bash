#!/usr/bin/env bash
# An empty entry in a -p or -g list is ignored and must not be taken as ID 0.
# -p with no ID left is an error; -g with no ID left only enables the PGID
# display.
source tests/common.bash

errors=$(mktemp "${TMPDIR:-/tmp}/lsof-id-errors.XXXXXX") || exit 1
trap 'rm -f "$errors"' EXIT

check_error() {
    local output status
    output=$("$lsof" "$@" -F p 2>"$errors")
    status=$?
    if [ "$status" -ne 1 ] || [ -n "$output" ] ||
        ! grep -Fq ": no process ID specified" "$errors"; then
        echo "FAIL: lsof $*: expected 'no process ID specified', status 1, and no stdout"
        echo "status: $status; stdout: $output"
        cat "$errors"
        exit 1
    fi
    echo "PASS: lsof $* reports 'no process ID specified'"
}

check_valid() {
    local output status pids
    output=$("$lsof" -w -a -d cwd -Fpg "$@" 2>"$errors")
    status=$?
    pids=$(echo "$output" | grep '^p')
    if [ "$status" -ne 0 ] || [ "$pids" != "p$$" ] ||
        ! echo "$output" | grep -qx "g$pgid"; then
        echo "FAIL: lsof $*: expected only this process and its PGID, status 0"
        echo "status: $status; stdout: $output"
        cat "$errors"
        exit 1
    fi
    echo "PASS: lsof $* lists only this process"
}

# This process is not in the selected process group, so nothing is listed.
check_none() {
    local output status
    # -Q: a PGID that matches no process is not an error.
    output=$("$lsof" -w -Q -a -p "$$" -d cwd -Fp "$@" 2>"$errors")
    status=$?
    if [ "$status" -ne 0 ] || [ -n "$output" ]; then
        echo "FAIL: lsof $*: expected no process, status 0"
        echo "status: $status; stdout: $output"
        cat "$errors"
        exit 1
    fi
    echo "PASS: lsof $* does not list this process"
}

{
    # Not from ps: BusyBox ps has no -p option.
    pgid=$("$lsof" -w -a -p "$$" -d cwd -Fg | sed -n 's/^g//p')
    case "$pgid" in
        ''|*[!0-9]*) echo "FAIL: cannot determine process group ID"; exit 1 ;;
        # e.g. in a PID namespace that the process group leader is not in
        0) echo "this shell has PGID 0, skipping"; exit 77 ;;
    esac

    # -p needs at least one ID, and -Q does not hide that.
    for spec in "" "," "^"; do
        check_error -p "$spec"
    done
    check_error -Q -p ""

    # An empty entry is ignored, and the IDs next to it are selected.
    for spec in "$$" "$$,$$" ",$$" "$$,,$$" "$$," "^,$$" "$$,^"; do
        check_valid -p "$spec"
    done
    # From here on, -p "$$" limits the search to this process. In ",^0" an
    # empty entry that is taken as ID 0 is both included and excluded.
    for spec in "^$PPID" "^0" ",^0"; do
        check_valid -p "$$" -p "$spec"
    done
    for spec in "$pgid" "$pgid,$pgid" "^$((pgid + 1))" "^0" ",^0" \
        ",$pgid" "$pgid,,$pgid" "$pgid," "^,$pgid" "$pgid,^"; do
        check_valid -p "$$" -g "$spec"
    done
    # -g with no ID only enables the PGID display.
    check_valid -p "$$" -g
    for spec in "" "," "^"; do
        check_valid -p "$$" -g "$spec"
    done
    # A PGID next to an empty entry is selected, and so is an explicit 0. In
    # "^,0" an empty entry that is taken as ID 0 is both excluded and included.
    for spec in ",$((pgid + 1))" 0 "^,0"; do
        check_none -g "$spec"
    done
    # Explicit zero is an ID, not an empty entry. -Q permits an unmatched ID 0,
    # and -g keeps a PID 0 that exists out of the output.
    check_valid -Q -p "$$" -g "$pgid,0"
    check_valid -Q -g "$pgid" -p "$$" -p "^,0"
    exit 0
} > "$report" 2>&1
