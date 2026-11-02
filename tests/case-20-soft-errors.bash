#!/usr/bin/env bash
# Warning-level argument failures must not hide unrelated usage errors.
source tests/common.bash

work=$(mktemp -d "${TMPDIR:-/tmp}/lsof-soft-errors.XXXXXX") || exit 1
trap 'rm -rf "$work"' EXIT
touch "$work/file" || exit 1
missing=$work/missing

check_output() {
    local expected=$1
    shift
    local output status expected_status=1
    [[ $expected == help ]] && expected_status=0
    output=$("$lsof" "$@" 2>&1)
    status=$?
    if [[ $status -ne $expected_status ]]; then
        echo "unexpected exit status $status: $*"
        return 1
    fi
    case $expected in
        quiet)
            [[ -z $output ]] && return 0
            ;;
        warning)
            [[ $output == *WARNING:* && $output == *usage:* ]] && return 0
            ;;
        diagnostic)
            [[ $output == *WARNING:* && $output != *usage:* ]] && return 0
            ;;
        usage|help)
            [[ $output == *usage:* ]] && return 0
            ;;
    esac
    echo "expected $expected output: $*"
    echo "$output"
    return 1
}

{
    for option in +d +D; do
        for path in "$missing" "$work/file"; do
            check_output warning "$option" "$path" || exit 1
            for flag in -w -t; do
                check_output quiet "$flag" "$option" "$path" || exit 1
                # A later flag suppresses usage, not an already printed warning.
                check_output diagnostic "$option" "$path" "$flag" || exit 1
                # Illegal FD ranges are portable hard argument errors.
                check_output usage "$flag" -d 3-1 "$option" "$path" || exit 1
                check_output usage "$flag" "$option" "$path" -d 3-1 || exit 1
                check_output help "$flag" "$option" "$path" -h || exit 1
            done
        done
    done
    check_output quiet -w +d "$missing" +D "$missing" || exit 1
    check_output usage -w +D "$missing" +w || exit 1
    # Preserve soft results through enter_fd(), including range entries.
    for list in '0,^1' '0,^1-2'; do
        check_output quiet -w -d "$list" || exit 1
        check_output usage -d "$list" || exit 1
    done
    # Hard errors must win in either order within one FD list.
    check_output usage -w -d '0,^1,3-1' || exit 1
    check_output usage -w -d '0,3-1,^1' || exit 1
    echo "soft-error argument tests passed"
} > "$report" 2>&1
