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

# Every port in a hostname filter must search both address families.
python3 - "$lsof" <<'PY' >> "$report" 2>&1 || exit 1
import os
import socket
import subprocess
import sys

try:
    sockets = []
    for family, address in ((socket.AF_INET, "127.0.0.1"),
                            (socket.AF_INET6, "::1")):
        if address not in {x[4][0] for x in socket.getaddrinfo(
                "localhost", None, family, socket.SOCK_STREAM)}:
            raise OSError(f"localhost does not resolve to {address}")
        s = socket.socket(family, socket.SOCK_STREAM)
        s.bind((address, 0))
        s.listen(1)
        sockets.append(s)
except OSError as e:
    print(f"SKIP: dual-family localhost unavailable: {e}")
else:
    ports = [str(s.getsockname()[1]) for s in sockets]
    expected = {str(s.fileno()) for s in sockets}
    for order in (["0"] + ports, ports[::-1]):
        result = subprocess.run(
            [sys.argv[1], "-nP", "-a", "-p", str(os.getpid()), "-Ff",
             "-iTCP@localhost:" + ",".join(order)],
            capture_output=True, text=True, timeout=10)
        found = {line[1:] for line in result.stdout.splitlines()
                 if line.startswith("f")}
        assert result.returncode == 0 and found == expected, (
            order, result.returncode, found, expected, result.stderr)
    print("PASS: hostname port lists search both families in either order")
PY

host=$(hostname)
naddr=$(python3 - "$host" <<'PY'
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
