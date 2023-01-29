#!/usr/bin/env bash
source tests/common.bash

file=$(mktemp /tmp/case-23-c-option.XXXXXX)

# lsof -clsof finds itself
$lsof -n -clsof > $file &
pid=$!
wait $pid
cat "$file" | grep "^lsof *${pid}" || exit 1

# lsof -c^lsof excludes itself
$lsof -n -c^lsof > $file &
pid=$!
wait $pid
cat "$file" | grep "^lsof *${pid}" && exit 1

# lsof -c/lsof/ includes itself
$lsof -n -c/lsof/ > $file &
pid=$!
wait $pid
cat "$file" | grep "^lsof *${pid}" || exit 1

# try multiple filters
$lsof -n -clsof -cbash -c/lsof/ > $file &
pid=$!
wait $pid
cat "$file" | grep "^lsof *${pid}" || exit 1

# terse output
$lsof -n -clsof -t > $file &
pid=$!
wait $pid
cat "$file" | grep "^${pid}$" || exit 1