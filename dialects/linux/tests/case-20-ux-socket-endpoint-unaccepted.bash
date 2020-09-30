#!/intentionally/invalid/path/to/bash

name=$( basename "$0" .bash )
lsof=$1
report=$2
tdir=$3

[[ -n $report && $report != - ]] && exec >> "$report"

# shellcheck disable=SC1037
tmp=/tmp/$name-$$-
__cleanup() { rm -f -- "$tmp"* ; }
trap __cleanup EXIT

out="$tmp"out

target=$tdir/ux

"$target" | {
    #
    # An example of expect output:
    #
    # pid=1451661
    # connect=4
    # connect2=5
    # ppid=1451659
    # listen=3
    # accept=5
    # path=/tmp/lsof-test-ux-1451659.s
    # end
    echo "target output:"
    while read k v ; do
	if [[ $k = end ]] ; then
	    break;
            # If we didn't stop now, we'd stall on EOF
	fi
	echo "$k=$v"
	eval "$k=\$v"
    done
    #
    # An exapmle of lsof output:
    #
    # COMMAND     PID USER   FD   TYPE             DEVICE SIZE/OFF     NODE NAME
    # ux      1445917  jet    3u  unix 0x000000002d21092b      0t0 75849115 /tmp/lsof-test-ux-1445917.s type=STREAM ->INO=75843930 1445919,ux,5u
    listen_sock_pat="^ux \+$ppid \+.* \+${listen}u \+unix \+0x[0-9a-f]\+ \+0t0 \+[0-9]\+ \+$path \+type=STREAM ->INO=[0-9]\+ \+$pid,ux,${connect2}u"'$'
    # ux      1445917  jet    5u  unix 0x00000000335230a5      0t0 75849117 /tmp/lsof-test-ux-1445917.s type=STREAM ->INO=75843929 1445919,ux,4u
    accepted_sock_pat="^ux \+$ppid \+.* \+${accept}u \+unix \+0x[0-9a-f]\+ \+0t0 \+[0-9]\+ \+$path \+type=STREAM ->INO=[0-9]\+ \+$pid,ux,${connect}u"'$'
    # ux      1445919  jet    4u  unix 0x00000000627d8ccc      0t0 75843929 type=STREAM ->INO=75849117 1445917,ux,5u
    client_sock_pat="^ux \+$pid \+.* \+${connect}u \+unix \+0x[0-9a-f]\+ \+0t0 \+[0-9]\+ \+type=STREAM ->INO=[0-9]\+ \+$ppid,ux,${accept}u"'$'
    # ux      1445919  jet    5u  unix 0x00000000cc000ead      0t0 75843930 type=STREAM
    client2_sock_pat="^ux \+$pid \+.* \+${connect2}u \+unix \+0x[0-9a-f]\+ \+0t0 \+[0-9]\+ \+type=STREAM"'$'
    # ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
    # The last line reflects what unix-diagnose netlink socket reports. The counter part just queued in the listen socket,
    # and is not accepted yet.
    #

    if "$lsof" +E "$path" > "$out" ; then
	kill -CONT "$ppid"
        num=$( wc -l < "$out" )
	if (( num != 1+4 )) ; then
	    printf 'Too many file descriptors are found (expected %u but got %u):\n' 4 "$num"
	    cat "$out"
	    exit 1
	elif ! grep -q "$listen_sock_pat" < "$out" ; then
	    printf "Doesn't match the pattern for listen socket\n"
	    printf "expected pattern: %s\n" "$listen_sock_pat"
	    cat "$out"
	    exit 1
	elif ! grep -q "$accepted_sock_pat" < "$out" ; then
	    printf "Doesn't match the pattern for accepted socket\n"
	    printf "expected pattern: %s\n" "$accepted_sock_pat"
	    cat "$out"
	    exit 1
	elif ! grep -q "$client_sock_pat" < "$out" ; then
	    printf "Doesn't match the pattern for the 1st client socket\n"
	    printf "expected pattern: %s\n" "$client_sock_pat"
	    cat "$out"
	    exit 1
	elif ! grep -q "$client2_sock_pat" < "$out" ; then
	    printf "Doesn't match the pattern for the 2nd client socket\n"
	    printf "expected pattern: %s\n" "$client2_sock_pat"
	    cat "$out"
	    exit 1
	fi
    else
	echo "failed to run $lsof: $?"
	exit 1
    fi
}
exit 0
