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
# machines. Five seconds is a pragmatic prerequisite lookup limit on all
# platforms; a timeout skips the test rather than using partial results.
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
if resolver.is_alive():
    print("timeout", flush=True)
    os._exit(0)
print(count[0], flush=True)
os._exit(0)
PY
)

if [ "$naddr" = timeout ]; then
    echo "SKIP: hostname resolution timed out after 5 seconds" >> "$report"
    exit 77
fi

if [ "${naddr:-0}" -lt 2 ]; then
    echo "SKIP: $host resolves to fewer than two addresses in one family" \
        >> "$report"
    exit 77
fi

{
    echo "host name: $host ($naddr addresses in one address family)"
    # Bound lsof's own lookup as well. Once the prerequisite succeeds, a hang
    # is a test failure. subprocess.run kills and reaps the child on timeout.
    python3 - "$lsof" "$host" "$$" <<'PY'
import subprocess
import sys

try:
    result = subprocess.run(
        [sys.argv[1], "-a", "-i@" + sys.argv[2], "-p", sys.argv[3]],
        stdout=subprocess.DEVNULL,
        stderr=subprocess.DEVNULL,
        timeout=15,
    )
except subprocess.TimeoutExpired:
    print("FAIL: lsof timed out after 15 seconds", flush=True)
    sys.exit(124)

# Preserve signal failures rather than mapping a negative return code to a
# shell status that could be mistaken for success or no matching files.
sys.exit(result.returncode if result.returncode >= 0 else 128 - result.returncode)
PY
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
