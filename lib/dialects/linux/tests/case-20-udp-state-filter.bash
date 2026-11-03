#!/usr/bin/env bash
source tests/common.bash

work=$(mktemp -d "${TMPDIR:-/tmp}/lsof-udp-state.XXXXXX") || exit 1
pid=
failed=0
cleanup() {
    if [[ -n $pid ]]; then
        kill "$pid" 2>/dev/null
        wait "$pid" 2>/dev/null
    fi
    rm -rf "$work"
}
trap cleanup EXIT

check_fds() {
    local label=$1 expected=$2 status actual
    shift 2
    "$lsof" -n -P -a -p "$pid" -Ff "$@" > "$work/output" 2> "$work/error"
    status=$?
    expected=$(printf '%s\n' "$expected" | tr ' ' '\n' | sort -n)
    actual=$(sed -n 's/^f\([0-9][0-9]*\)$/\1/p' "$work/output" | sort -n)
    if [[ $status -ne 0 || $actual != "$expected" ]]; then
        echo "IPv$family $label: exit $status; expected FDs [$expected], got [$actual]"
        cat "$work/output" "$work/error"
        return 1
    fi
}

{
    for family in 4 6; do
        : > "$work/ready"
        "$tcasedir/udp_state" "$family" > "$work/ready" 2> "$work/setup-error" &
        pid=$!
        for ((attempt = 0; attempt < 100; attempt++)); do
            [[ -s $work/ready ]] && break
            kill -0 "$pid" 2>/dev/null || break
            sleep 0.05
        done
        if [[ ! -s $work/ready ]]; then
            if kill -0 "$pid" 2>/dev/null; then
                echo "IPv$family socket setup timed out"
                exit 1
            fi
            wait "$pid"
            status=$?
            pid=
            cat "$work/setup-error"
            if [[ $family == 6 && $status == 77 ]]; then
                echo "SKIP: IPv6 unavailable (IPv4 coverage passed)"
                continue
            fi
            exit 1
        fi
        read -r idle connected listener client server < "$work/ready"

        check_fds baseline "$idle $connected" -i "${family}UDP" || failed=1
        check_fds inclusion "$connected" -i "${family}UDP" -sUDP:ESTABLISHED || failed=1
        check_fds exclusion "$idle" -i "${family}UDP" '-sUDP:^ESTABLISHED' || failed=1
        check_fds mixed-inclusion "$listener $connected" -i "$family" \
            -sTCP:LISTEN -sUDP:ESTABLISHED || failed=1
        check_fds tcp-only "$listener" -i "$family" -sTCP:LISTEN || failed=1
        check_fds udp-only "$connected" -i "$family" -sUDP:ESTABLISHED || failed=1
        check_fds tcp-exclusion-only "$idle $connected $listener" -i "$family" \
            '-sTCP:^ESTABLISHED' || failed=1
        check_fds udp-exclusion-only "$idle $listener $client $server" -i "$family" \
            '-sUDP:^ESTABLISHED' || failed=1
        check_fds mixed-exclusion "$idle $listener" -i "$family" \
            '-sTCP:^ESTABLISHED' '-sUDP:^ESTABLISHED' || failed=1

        kill "$pid"
        wait "$pid" 2>/dev/null
        pid=
        echo "IPv$family UDP state filter checks completed"
    done
    exit "$failed"
} > "$report" 2>&1
