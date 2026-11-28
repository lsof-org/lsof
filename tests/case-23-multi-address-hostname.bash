#!/usr/bin/env bash
# Regression test for https://github.com/lsof-org/lsof/issues/370
#
# A host name can resolve to several addresses.  lsof creates one nwad entry
# for each address, and lsof_destroy() frees the argument and protocol strings
# of every entry.  If those strings are shared between entries, lsof frees the
# same allocation more than once and aborts before it can flush its output.
source tests/common.bash

if ! command -v python3 >/dev/null 2>&1; then
    echo "SKIP: python3 not available" >> "$report"
    exit 77
fi

count_addresses() {
    python3 - "$1" <<'PY'
import socket
import sys

host = sys.argv[1]
count = 0
for family in (socket.AF_INET, socket.AF_INET6):
    try:
        infos = socket.getaddrinfo(host, None, family, socket.SOCK_STREAM)
    except OSError:
        continue
    count = max(count, len({info[4][0] for info in infos}))
print(count)
PY
}

host=$(hostname)
naddr=$(count_addresses "$host")
hosts_backup=
restore_hosts() {
    if [ -n "$hosts_backup" ]; then
        cat "$hosts_backup" > /etc/hosts
        rm -f "$hosts_backup"
    fi
}
trap restore_hosts EXIT HUP INT TERM

if [ "${naddr:-0}" -lt 2 ] && [ "$(id -u)" -eq 0 ] && [ -w /etc/hosts ]; then
    # Make the regression deterministic without relying on external DNS.
    # The temporary name has two IPv4 addresses; only name resolution is
    # needed by this test, not connectivity.
    hosts_backup=$(mktemp) || hosts_backup=
    if [ -n "$hosts_backup" ] && cp /etc/hosts "$hosts_backup"; then
        host=lsof-multi-address.test
        printf '127.0.0.1 %s\n127.0.0.2 %s\n' "$host" "$host" \
            >> /etc/hosts
        naddr=$(count_addresses "$host")
    else
        restore_hosts
    fi
fi

if [ "${naddr:-0}" -lt 2 ]; then
    echo "SKIP: $host resolves to fewer than two addresses in one family" \
        >> "$report"
    exit 77
fi

{
    echo "host name: $host ($naddr addresses in one address family)"
    $lsof -a -i@"$host" -p $$ >/dev/null 2>&1
    status=$?
    case $status in
    0|1)
        echo "PASS: lsof -a -i@$host -p $$ exited with status $status"
        ;;
    *)
        echo "FAIL: lsof -a -i@$host -p $$ exited with status $status"
        exit 1
        ;;
    esac
} >> "$report" 2>&1
