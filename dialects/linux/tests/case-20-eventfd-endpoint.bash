#!/intentionally/invalid/path/to/bash

# name=$( basename "$0" .bash ) <= IGNORED
lsof=$1
report=$2
tdir=$3

[[ -n $report && $report != - ]] && exec >> "$report"

uname -r | {
    if  IFS=. read -r major minor _ &&
        (( major * 1000 + minor < 5002 ))
    then
	echo "event endpoint features doesn't work on Linux $major.$minor"
	exit 2
    fi
}

target=$tdir/eventfd
if [[ ! -x $target ]]; then
    echo "target executable ($target) is not found"
    exit 1
fi

{ "$target" & } | {
    if ! read -r parent child fd ; then
        echo "no output from target ($target)"
        exit 1
    fi

    if [ -z "$parent" ] || [ -z "$child" ] || [ -z "$fd" ]; then
	echo "unexpected output from target ($target)"
	exit 1
    fi
    {
	echo "parent: $parent"
	echo "child: $child"
	echo "fd: $fd"
	echo "cmdline: '$lsof' +E -p '$parent'"
	echo
	echo PARENT
	echo
	"$lsof" +E -p "$parent"
	echo
	echo CHILD
	echo
	"$lsof" +E -p "$child"
    }
    {
	{
	    echo From the parent side
	    # eventfd 23685 yamato    3u  a_inode   0,13        0    11217 [eventfd:29] 23686,eventfd,3u
	    echo expected pattern: "eventfd *$parent .* ${fd}u *a_inode .* \[eventfd:[0-9]*\] *$child,eventfd,${fd}u"
	    "$lsof" +E -p "$parent" |
		grep -q "eventfd *$parent .* ${fd}u *a_inode .* \[eventfd:[0-9]*\] *$child,eventfd,${fd}u"
	} && {
	    echo From the parent side
	    # eventfd 23686 yamato    3u  a_inode   0,13        0    11217 [eventfd:29] 23685,eventfd,3u
	    echo expected pattern: "eventfd *$child .* ${fd}u *a_inode .* \[eventfd:[0-9]*\] *$parent,eventfd,${fd}u"
	    "$lsof" +E -p "$parent" |
		grep -q "eventfd *$child .* ${fd}u *a_inode .* \[eventfd:[0-9]*\] *$parent,eventfd,${fd}u"

	} && {
	    echo From the child side
	    # eventfd 23685 yamato    3u  a_inode   0,13        0    11217 [eventfd:29] 23686,eventfd,3u
	    echo expected pattern: "eventfd *$parent .* ${fd}u *a_inode .* \[eventfd:[0-9]*\] *$child,eventfd,${fd}u"
	    "$lsof" +E -p "$parent" |
		grep -q  "eventfd *$parent .* ${fd}u *a_inode .* \[eventfd:[0-9]*\] *$child,eventfd,${fd}u"
	} && {
	    echo From the child side
	    # eventfd 23686 yamato    3u  a_inode   0,13        0    11217 [eventfd:29] 23685,eventfd,3u
	    echo expected pattern: "eventfd *$child .* ${fd}u *a_inode .* \[eventfd:[0-9]*\] *$parent,eventfd,${fd}u"
	    "$lsof" +E -p "$parent" |
		grep -q  "eventfd *$child .* ${fd}u *a_inode .* \[eventfd:[0-9]*\] *$parent,eventfd,${fd}u"

	} && {
	    kill "$child"
	    exit 0
	}
    }
    kill "$child"
    exit 1
}
