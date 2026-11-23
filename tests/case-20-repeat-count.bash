#!/usr/bin/env bash
source tests/common.bash
# need gnu grep for -e in solaris
export PATH=/usr/gnu/bin:$PATH
base=$(pwd)

output=$(${lsof} -r 1c1 -p $$)
echo "$output" >> $report
if [ $(echo "$output" | grep -e '=======' | wc -l) != 1 ]; then
    exit 1
fi

output=$(${lsof} -r 1c5 -p $$)
echo "$output" >> $report
if [ $(echo "$output" | grep -e '=======' | wc -l) != 5 ]; then
    exit 1
fi

# The count must not be parsed again as a -c option
output=$(${lsof} -r1c2 -a -p $$ -Fp)
echo "$output" >> $report
if [ $(echo "$output" | grep -e "^p$$\$" | wc -l) != 2 ]; then
    exit 1
fi

# The final pass must not wait for an interval before exiting.
if command -v python3 >/dev/null 2>&1; then
    python3 - "$lsof" "$$" <<'PY' >> "$report" 2>&1 || exit 1
import subprocess
import sys

try:
    result = subprocess.run([sys.argv[1], "-r30c1", "-p", sys.argv[2]],
                            stdout=subprocess.PIPE, stderr=subprocess.PIPE,
                            timeout=5)
except subprocess.TimeoutExpired:
    sys.exit("FAIL: lsof slept after its final repeat pass")
if result.returncode != 0 or result.stdout.count(b"=======") != 1:
    sys.exit("FAIL: unexpected output or status from single repeat pass")
PY
fi

exit 0
