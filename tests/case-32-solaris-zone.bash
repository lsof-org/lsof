#!/usr/bin/env bash
# Solaris/illumos zone filtering smoke test.
source tests/common.bash

case "${dialect:-}" in
    solaris) ;;
    *)
        echo "SKIP: Solaris zone test is only for the solaris dialect" \
            >> "$report"
        exit 77
        ;;
esac

{
    echo "zone filter: global"
    $lsof -a -z global -p $$ >/dev/null 2>&1
    status=$?
    if [ "$status" -ne 0 ]; then
        echo "FAIL: lsof -a -z global -p $$ exited with status $status"
        exit 1
    fi

    echo "zone filter: nonexistent zone"
    $lsof -a -z lsof-no-such-zone -p $$ >/dev/null 2>&1
    status=$?
    if [ "$status" -ne 1 ]; then
        echo "FAIL: nonexistent zone exited with status $status, expected 1"
        exit 1
    fi
    echo "PASS: zone filtering"
} >> "$report" 2>&1
