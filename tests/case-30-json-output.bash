#!/usr/bin/env bash
source tests/common.bash

# Skip if python3 not available (needed for JSON validation)
if ! command -v python3 &>/dev/null; then
    echo "SKIP: python3 not available" >> $report
    exit 77
fi

{
    # Test 1: -J produces valid JSON
    output=$($lsof -J -p $$ 2>/dev/null)
    if ! echo "$output" | python3 -m json.tool > /dev/null 2>&1; then
        echo "FAIL: -J output is not valid JSON" >> $report
        exit 1
    fi
    echo "PASS: -J produces valid JSON" >> $report

    # Test 2: -J output has lsof_version and processes keys
    echo "$output" | python3 -c "
import sys, json
d = json.load(sys.stdin)
assert 'lsof_version' in d, 'missing lsof_version'
assert 'processes' in d, 'missing processes'
assert isinstance(d['processes'], list), 'processes is not a list'
assert len(d['processes']) > 0, 'no processes found'
" 2>&1 || { echo "FAIL: -J output structure invalid" >> $report; exit 1; }
    echo "PASS: -J output structure valid" >> $report

    # Test 3: -J -Fpcfn only includes selected fields
    output_sel=$($lsof -J -Fpcfn -p $$ 2>/dev/null)
    echo "$output_sel" | python3 -c "
import sys, json
d = json.load(sys.stdin)
p = d['processes'][0]
assert 'pid' in p, 'missing pid'
assert 'command' in p, 'missing command'
f = p['files'][0]
allowed_file_keys = {'fd', 'name'}
actual_file_keys = set(f.keys())
extra_keys = actual_file_keys - allowed_file_keys
assert not extra_keys, 'unexpected file keys with -Fpcfn: %s' % extra_keys
assert 'uid' not in p, 'uid should not be present with -Fpcfn'
" 2>&1 || { echo "FAIL: -J field selection not working" >> $report; exit 1; }
    echo "PASS: -J field selection works" >> $report

    # Test 4: -J empty result produces valid JSON
    output_empty=$($lsof -J -p 999999 2>/dev/null)
    echo "$output_empty" | python3 -c "
import sys, json
d = json.load(sys.stdin)
assert d['processes'] == [], 'empty result should have empty processes array'
" 2>&1 || { echo "FAIL: -J empty result is not valid JSON" >> $report; exit 1; }
    echo "PASS: -J empty result is valid JSON" >> $report

    # Test 5: -J and -j are mutually exclusive
    if $lsof -J -j 2>/dev/null; then
        echo "FAIL: -J -j should error" >> $report
        exit 1
    fi
    echo "PASS: -J -j mutual exclusivity" >> $report

    # Test 6: -J and -t are mutually exclusive
    if $lsof -J -t 2>/dev/null; then
        echo "FAIL: -J -t should error" >> $report
        exit 1
    fi
    echo "PASS: -J -t mutual exclusivity" >> $report

    # Network names must include the same endpoints as -Fn, in both JSON modes.
    python3 - "$lsof" <<'PY' || exit 1
import json
import os
import socket
import subprocess
import sys

listener = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
listener.bind(("127.0.0.1", 0))
listener.listen(1)
client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
client.connect(listener.getsockname())
accepted, _ = listener.accept()
udp = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
udp.bind(("0.0.0.0", 0))
sockets = [listener, client, accepted, udp]
ipv6 = None
try:
    ipv6 = socket.socket(socket.AF_INET6, socket.SOCK_STREAM)
    ipv6.bind(("::1", 0))
    ipv6.listen(1)
except OSError as e:
    if ipv6 is not None:
        ipv6.close()
    print(f"SKIP: IPv6 endpoint coverage unavailable: {e}")
else:
    sockets.append(ipv6)
base = [sys.argv[1], "-nP", "-a", "-p", str(os.getpid()), "-i"]

def run(options):
    result = subprocess.run(base + options, capture_output=True, text=True,
                            timeout=10)
    assert result.returncode == 0, result.stderr
    return result.stdout

names = {}
for line in run(["-Ffn"]).splitlines():
    if line.startswith("f"):
        fd = line[1:]
    elif line.startswith("n"):
        names[fd] = line[1:]
assert set(names) == {str(s.fileno()) for s in sockets}, names
assert names[str(listener.fileno())].startswith(
    f"127.0.0.1:{listener.getsockname()[1]}"), names
assert names[str(udp.fileno())].startswith(f"*:{udp.getsockname()[1]}"), names
for mode in ("-J", "-j"):
    for fields in ([], ["-Ffn"]):
        output = run([mode] + fields)
        files = (json.loads(output)["processes"][0]["files"] if mode == "-J"
                 else [json.loads(line) for line in output.splitlines()])
        actual = {f["fd"]: f.get("name") for f in files}
        assert actual == names, (mode, fields, actual, names)
    output = run([mode, "-Ff"])
    files = (json.loads(output)["processes"][0]["files"] if mode == "-J"
             else [json.loads(line) for line in output.splitlines()])
    assert all("name" not in f for f in files), (mode, files)
print("PASS: JSON/JSONL socket endpoints match -Fn and honor field selection")
PY

    exit 0
} > $report 2>&1
