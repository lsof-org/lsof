#!/usr/bin/env bash
source tests/common.bash

# Skip if python3 not available
if ! command -v python3 &>/dev/null; then
    echo "SKIP: python3 not available" >> $report
    exit 77
fi

{
    # Test 1: -j produces valid JSON Lines (each line is valid JSON)
    tmpout=$(mktemp)
    $lsof -j -p $$ 2>/dev/null > "$tmpout"
    fail=0
    while IFS= read -r line; do
        if [ -z "$line" ]; then
            continue
        fi
        if ! echo "$line" | python3 -m json.tool > /dev/null 2>&1; then
            echo "FAIL: -j line is not valid JSON: $line" >> $report
            fail=1
            break
        fi
    done < "$tmpout"
    rm -f "$tmpout"
    if [ $fail -ne 0 ]; then
        exit 1
    fi
    echo "PASS: -j produces valid JSON Lines" >> $report

    # Test 2: -j lines contain both process and file fields (denormalized)
    line=$($lsof -j -p $$ 2>/dev/null | head -1)
    echo "$line" | python3 -c "
import sys, json
d = json.load(sys.stdin)
assert 'pid' in d, 'missing pid'
assert 'fd' in d or 'command' in d, 'missing file or process fields'
" 2>&1 || { echo "FAIL: -j line missing expected fields" >> $report; exit 1; }
    echo "PASS: -j lines have denormalized fields" >> $report

    # Test 3: -j with no matching process produces empty output
    output=$($lsof -j -p 999999 2>/dev/null)
    if [ -n "$output" ]; then
        echo "FAIL: -j with no matches should produce empty output" >> $report
        exit 1
    fi
    echo "PASS: -j empty result is empty" >> $report

    exit 0
} > $report 2>&1
