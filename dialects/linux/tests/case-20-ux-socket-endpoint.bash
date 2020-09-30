#!/intentionally/invalid/path/to/bash

name=$( basename "$0" .bash )
lsof=$1
report=$2

ux=/tmp/$name-$$.sock
nc -l -U "$ux" > /dev/null < /dev/zero &
server=$!
sleep 1
nc -U "$ux" < /dev/zero  > /dev/null &
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
if ! grep -q "^.* unix 0x[0-9a-f]\+ \+0t0 \+[0-9]\+ type=STREAM ->INO=[0-9]\+ $server,nc,[0-9]\+u" < "$fclient" ; then
    echo "failed in client side" >> "$report"
    cat "$fclient" >> "$report"
    killBoth "$client" "$server"
    exit 1
fi

fserver=/tmp/$name-server-$$
"$lsof" -n -E -P -p "$server" > "$fserver"
if ! grep -q "^.* unix 0x[0-9a-f]\+ \+0t0 \+[0-9]\+ $ux type=STREAM ->INO=[0-9]\+ $client,nc,[0-9]\+u" < "$fserver" ; then
    echo "failed in server side" >> "$report"
    cat "$fserver" >> "$report"
    killBoth "$client" "$server"
    exit 1
fi

killBoth "$client" "$server"

exit 0
