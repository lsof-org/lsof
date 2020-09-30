#!/intentionally/invalid/path/to/bash

#name=$( basename "$0" .bash )  <= IGNORED
lsof=$1
report=$2
tdir=$3

[[ -n $report && $report != - ]] && exec >> "$report"

target=$tdir/pipe
if [[ ! -x $target ]] ; then
    echo "target executable ($target) is not found"
    exit 1
fi

{ "$target" & } | {
    read parent child fdr fdw;
    if [ -z "$parent" ] || [ -z "$child" ] || [ -z "$fdr" ] || [ -z "$fdw" ]; then
	echo "unexpected output form target ($target)"
	exit 1
    fi
    echo parent: "$parent"
    echo child:  "$child"
    echo fdr:    "$fdr"
    echo fdw:    "$fdw"
    echo cmdline: "$lsof" +E -p "$parent"
    "$lsof" +E -p "$parent"

    {
        echo expected pattern: ".* $parent .* ${fdr}r *FIFO .* pipe $child,p[-a-z]*,${fdw}w"
        "$lsof" +E -p "$parent" |
            grep -q ".* $parent .* ${fdr}r *FIFO .* pipe $child,p[-a-z]*,${fdw}w"
    } && {
        echo expected parent: ".* $child .* ${fdw}w *FIFO .* pipe $parent,p[-a-z]*,${fdr}r"
        "$lsof" +E -p "$parent" |
            grep -q ".* $child .* ${fdw}w *FIFO .* pipe $parent,p[-a-z]*,${fdr}r"
    } && {
        kill "$child"
        exit 0
    }

    kill "$child"
    exit 1
}
