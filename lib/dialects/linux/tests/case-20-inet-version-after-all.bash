#!/bin/bash
source tests/common.bash

if [ -z "$(nc -h 2>&1 | grep '\s-6')" ]; then
    echo "nc does not support -6 option, skipping" >> $report
    exit 77
fi

# -i selects every IP version, so a following -i4 must not drop IPv6 files.
nc -l -6 ::1 10002 > /dev/null < /dev/zero 2>> $report &
server=$!
sleep 1

if ! kill -0 $server 2>/dev/null; then
    echo "Maybe ipv6 stack is not available on this system, skipping" >> $report
    exit 77
fi

fserver=/tmp/${name}-server-$$
$lsof -n -P -p $server -a -i -i4 > $fserver
kill -9 $server 2> /dev/null > /dev/null
if ! grep -q "IPv6 .*TCP \[::1\]:10002 (LISTEN)" $fserver; then
    echo "IPv6 socket missing with -i -i4" >> $report
    cat $fserver >> $report
    rm -f $fserver
    exit 1
fi

rm -f $fserver
exit 0
