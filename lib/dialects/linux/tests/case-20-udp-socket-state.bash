#!/bin/bash
source tests/common.bash

if [ -z "$(nc -h 2>&1 | grep '\s\-4')" ]; then
    echo "nc does not support -4 option, skipping" >> $report
    exit 77
fi

if [ -z "$(nc -h 2>&1 | grep '\s\-u')" ]; then
    echo "nc does not support -u option, skipping" >> $report
    exit 77
fi

# Use sleep as stdin to keep nc alive without flooding data.
# /dev/zero causes "Message too long" with ncat (nmap) on UDP.
sleep 60 | nc -l -u -4 127.0.0.1 10001 > /dev/null &
server=$!
sleep 1
sleep 60 | nc -u -4 127.0.0.1 10001 > /dev/null &
client=$!

sleep 1

killBoth()
{
    kill -9 $1
    sleep 1
    kill -9 $2
} 2> /dev/null > /dev/null

fclient=/tmp/${name}-client-$$
$lsof -n -P -p $client -a -i UDP > $fclient
if ! cat $fclient | grep -q "UDP.*127.0.0.1:.*->127.0.0.1:10001 (ESTABLISHED)"; then
    echo "connected UDP socket missing ESTABLISHED state" >> $report
    cat $fclient >> $report
    killBoth $client $server
    rm -f $fclient
    exit 1
fi

rm -f $fclient
killBoth $client $server

exit 0
