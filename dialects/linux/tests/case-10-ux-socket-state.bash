#!/intentionally/invalid/path/to/bash

name=$( basename "$0" .bash )
lsof=$1
report=$2

[[ -n $report && $report != - ]] && exec >> "$report"

# shellcheck disable=SC1037
tmp=/tmp/$name-$$-
__cleanup() { rm -f -- "$tmp"* ; }
trap __cleanup EXIT

ux="$tmp"sock
nc -l -U "$ux" > /dev/null < /dev/zero &
server=$!

killBoth()
{
    kill -9 "$1"
    wait "$1"
    kill -9 "$2"
    wait "$2"
} 2> /dev/null > /dev/null

waitForSyscall()
{
    local pid=$1
    local pat=$2
    local niterations=$3    # TODO "seq 0 $niterations" seems broken, but we've replicated it anyway
    local i

    for (( i=0 ; i<=niterations; ++i)); do
	sleep 1
	if grep -q "$pat" "/proc/$pid/stack" ; then
	    break
	fi
    done
}

waitForSyscall "$server" 'select\|poll' 10

fserver="$tmp"server-before
"$lsof" -n -Ts -P -U -a -p "$server" > "$fserver"
# nc        22512 yamato    3u  unix 0x000000008f6993b8      0t0     470697 /tmp/a type=STREAM (LISTEN)
if ! grep -q "^.* unix 0x[0-9a-f]\+ \+0t0 \+[0-9]\+ $ux type=STREAM (LISTEN)" < "$fserver" ; then
    echo "failed in server side (before connecting)"
    cat "$fserver"
    kill -9 "$server"
    exit 1
fi

nc -U "$ux" < /dev/zero > /dev/null &
client=$!
sleep 1
fserver="$tmp"server-after
"$lsof" -n -Ts -P -U -a -p "$server" > "$fserver"
# nc      22512 yamato    4u  unix 0x00000000deffde05      0t0 472699 /tmp/a type=STREAM (CONNECTED)
if ! grep -q "^.* unix 0x[0-9a-f]\+ \+0t0 \+[0-9]\+ $ux type=STREAM (CONNECTED)" < "$fserver" ; then
    echo "failed in server side (after connecting)"
    cat "$fserver"
    killBoth "$client" "$server"
    exit 1
fi

fclient="$tmp"client
"$lsof" -n -Ts -P -U -a -p "$client" -FT | grep ^TST > "$fclient"
# TST=CONNECTED
if ! grep -q "^TST=CONNECTED" < "$fclient" ; then
    echo "failed in client side"
    cat "$fclient"
    killBoth "$client" "$server"
    exit 1
fi

killBoth "$client" "$server"

exit 0
