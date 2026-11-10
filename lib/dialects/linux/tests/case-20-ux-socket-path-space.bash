#!/bin/bash
source tests/common.bash

if [ -z "$(nc -h 2>&1 | grep '\-U')" ]; then
    echo "nc does not support unix socket, skipping" >> $report
    exit 77
fi

# A path with blanks, a TAB and a ':' in it must be shown in full.
# lsof prints the TAB as \t.
ux="/tmp/$name-$$ a:b  c"$'\t'"d.sock"
shown="${ux//$'\t'/\\t}"
nc -l -U "$ux" > /dev/null < /dev/zero &
server=$!
sleep 1

fserver=/tmp/${name}-server-$$
$lsof -n -P -p $server -a -U > $fserver
kill -9 $server 2> /dev/null > /dev/null
rm -f "$ux"
if ! grep -q -F " $shown type=STREAM" $fserver; then
    echo "unix socket path with blanks or a TAB is not shown in full" >> $report
    cat $fserver >> $report
    rm -f $fserver
    exit 1
fi

rm -f $fserver
exit 0
