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

    exit 0
} > $report 2>&1
