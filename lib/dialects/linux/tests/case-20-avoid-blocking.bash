#!/bin/bash
source tests/common.bash

for tool in python3 strace; do
    if ! command -v "$tool" >/dev/null 2>&1; then
        echo "SKIP: $tool not available" >> "$report"
        exit 77
    fi
done

python3 - "$lsof" >> "$report" 2>&1 <<'PY'
import os
import pathlib
import re
import socket
import subprocess
import sys
import tempfile

with tempfile.TemporaryDirectory() as directory:
    trace = pathlib.Path(directory) / "trace"
    with tempfile.TemporaryFile() as file, socket.socket() as listener:
        listener.bind(("127.0.0.1", 0))
        listener.listen()
        pid = str(os.getpid())
        base = [sys.argv[1], "-b", "-w", "-nP", "-p", pid]
        result = subprocess.run(
            ["strace", "-f", "-e", "trace=%file", "-o", str(trace)] + base,
            capture_output=True, text=True, timeout=20,
        )
        if "ptrace" in result.stderr and "Operation not permitted" in result.stderr:
            print("SKIP: tracing is not permitted")
            sys.exit(77)
        assert result.returncode == 0, result.stderr
        target = re.compile(
            r'"/proc/' + pid + r'/(?:cwd|root|exe|fd/\d+|map_files/[^"]+)"'
        )
        stat_call = re.compile(r'\b(?:l?stat(?:64)?|newfstatat|fstatat64|statx)\(')
        calls = [line for line in trace.read_text().splitlines()
                 if stat_call.search(line) and target.search(line)]
        assert not calls, "-b still stats process targets:\n" + "\n".join(calls)

        port = str(listener.getsockname()[1])
        result = subprocess.run(
            base + ["-a", "-iTCP:" + port, "-Ffn"],
            capture_output=True, text=True, timeout=20,
        )
        assert result.returncode == 0, result.stderr
        assert "f" + str(listener.fileno()) in result.stdout.splitlines(), result.stdout
        assert ":" + port in result.stdout, result.stdout
        print("PASS: -b avoids process-target stat calls and still finds sockets")
PY
