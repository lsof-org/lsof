#!/intentionally/invalid/path/to/bash

name=$( basename "$0" .bash )
lsof=$1
report=$2

[[ -n $report && $report != - ]] && exec >> "$report"

nc -l -4 127.0.0.1 10000 > /dev/null < /dev/zero &
server=$!
sleep 1
nc -4 -s 127.0.0.2 -p 9999 127.0.0.1 10000 < /dev/zero  > /dev/null &
client=$!

sleep 1

killBoth()
{
    kill -9 "$1"
    wait "$1"
    kill -9 "$2"
    wait "$2"
} 2> /dev/null > /dev/null

fclient=/tmp/$name-client-$$
"$lsof" -n -E -P -p "$client" > "$fclient"
if ! grep -q "TCP 127.0.0.2:9999->127.0.0.1:10000 $server,nc,[0-9]\+u (ESTABLISHED)" < "$fclient" ; then
    echo "failed in client side"
    cat "$fclient"
    killBoth "$client" "$server"
    exit 1
fi

fserver=/tmp/$name-server-$$
"$lsof" -n -E -P -p "$server" > "$fserver"
if ! grep -q "TCP 127.0.0.1:10000->127.0.0.2:9999\+ $client,nc,[0-9]\+u (ESTABLISHED)" < "$fserver" ; then
    echo "failed in server side"
    cat "$fserver"
    killBoth "$client" "$server"
    exit 1
fi

killBoth "$client" "$server"

exit 0
