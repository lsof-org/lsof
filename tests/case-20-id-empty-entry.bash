#!/usr/bin/env bash
# An empty entry in a -p or -g list must not be taken as ID 0, and an empty
# -p list must not select every process.
source tests/common.bash

errors=$(mktemp "${TMPDIR:-/tmp}/lsof-id-errors.XXXXXX") || exit 1
trap 'rm -f "$errors"' EXIT

check_error() {
    local output status
    output=$("$lsof" "$1" "$2" -F p 2>"$errors")
    status=$?
    if [ "$status" -ne 1 ] || [ -n "$output" ] ||
        ! grep -Fq ": $3" "$errors"; then
        echo "FAIL: lsof $1 '$2': expected '$3', status 1, and no stdout"
        echo "status: $status; stdout: $output"
        cat "$errors"
        exit 1
    fi
    echo "PASS: lsof $1 '$2' reports '$3'"
}

check_valid() {
    local output status pids
    # Limit successful searches to this process and its cwd, not the whole system.
    output=$("$lsof" -w -a -p "$$" -d cwd -Fpg "$@" 2>"$errors")
    status=$?
    pids=$(echo "$output" | grep '^p')
    if [ "$status" -ne 0 ] || [ "$pids" != "p$$" ] ||
        ! echo "$output" | grep -qx "g$pgid"; then
        echo "FAIL: lsof $*: expected this process and its PGID, status 0"
        echo "status: $status; stdout: $output"
        cat "$errors"
        exit 1
    fi
    echo "PASS: lsof $* remains valid"
}

{
    # Not from ps: BusyBox ps has no -p option.
    pgid=$("$lsof" -w -a -p "$$" -d cwd -Fg | sed -n 's/^g//p')
    case "$pgid" in
        ''|*[!0-9]*) echo "FAIL: cannot determine process group ID"; exit 1 ;;
    esac

    check_error -p "" "no process ID specified"
    for spec in "," ",$$" "$$,,$$" "^" "^,$$" "$$,^"; do
        check_error -p "$spec" "empty process ID: $spec"
    done
    for spec in "," ",$pgid" "$pgid,,$pgid" "^" "^,$pgid" "$pgid,^"; do
        check_error -g "$spec" "empty process group ID: $spec"
    done

    # Preserve valid IDs, exclusions, and the accepted trailing comma.
    for spec in "$$" "$$,$$" "$$," "^$PPID" "^0"; do
        check_valid -p "$spec"
    done
    check_valid -g
    check_valid -g ""
    for spec in "$pgid" "$pgid,$pgid" "$pgid," "^$((pgid + 1))" "^0"; do
        check_valid -g "$spec"
    done
    # Explicit zero is an ID, not an empty entry. -Q permits an unmatched ID 0.
    check_valid -Q -g "$pgid,0"
    exit 0
} > "$report" 2>&1
