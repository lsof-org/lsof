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

host=$(hostname)
# Resolving a .local host name through mDNS can block for a long time on some
# machines, so cap the lookup and treat a timeout as "no addresses".
naddr=$(python3 - "$host" <<'PY'
import os
import socket
import sys
import threading

host = sys.argv[1]
count = [0]

def resolve():
    for family in (socket.AF_INET, socket.AF_INET6):
        try:
            infos = socket.getaddrinfo(host, None, family, socket.SOCK_STREAM)
        except OSError:
            continue
        count[0] = max(count[0], len({info[4][0] for info in infos}))

resolver = threading.Thread(target=resolve, daemon=True)
resolver.start()
resolver.join(5)
print(count[0], flush=True)
os._exit(0)
PY
)

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
