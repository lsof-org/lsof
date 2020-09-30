#!/intentionally/invalid/path/to/bash

# name=$( basename "$0" .bash ) <= IGNORED
lsof=$1
report=$2
tdir=$3

[[ -n $report && $report != - ]] && exec >> "$report"

target=$tdir/pipe
if [[ ! -x $target ]] ; then
    echo "target executable ($target) is not found"
    exit 1
fi

{ "$target" no-close & } | {
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
        # pipe-no-c 25113 yamato    3r  FIFO   0,12      0t0     616532 pipe 25113,pipe-no-c,4w 25114,pipe-no-c,3r 25114,pipe-no-c,4w
        echo expected pattern: ".* $parent .* ${fdr}r *FIFO .* pipe $parent,p[-a-z]*,${fdw}w $child,p[-a-z]*,${fdr}r $child,p[-a-z]*,${fdw}w"
        "$lsof" +E -p "$parent" |
            grep -q ".* $parent .* ${fdr}r *FIFO .* pipe $parent,p[-a-z]*,${fdw}w $child,p[-a-z]*,${fdr}r $child,p[-a-z]*,${fdw}w"
    } && {
        # pipe-no-c 25113 yamato    4w  FIFO   0,12      0t0     616532 pipe 25113,pipe-no-c,3r 25114,pipe-no-c,3r 25114,pipe-no-c,4w
        echo expected pattern: ".* $parent .* ${fdw}w *FIFO .* pipe $parent,p[-a-z]*,${fdr}r $child,p[-a-z]*,${fdr}r $child,p[-a-z]*,${fdw}w"
        "$lsof" +E -p "$parent" |
            grep -q ".* $parent .* ${fdw}w *FIFO .* pipe $parent,p[-a-z]*,${fdr}r $child,p[-a-z]*,${fdr}r $child,p[-a-z]*,${fdw}w"
    } && {
        # pipe-no-c 25114 yamato    3r  FIFO   0,12      0t0     616532 pipe 25113,pipe-no-c,3r 25113,pipe-no-c,4w 25114,pipe-no-c,4w
        echo expected pattern: ".* $child .* ${fdr}r *FIFO .* pipe $parent,p[-a-z]*,${fdr}r $parent,p[-a-z]*,${fdw}w $child,p[-a-z]*,${fdw}w"
        "$lsof" +E -p "$parent" |
            grep -q ".* $child .* ${fdr}r *FIFO .* pipe $parent,p[-a-z]*,${fdr}r $parent,p[-a-z]*,${fdw}w $child,p[-a-z]*,${fdw}w"
    } && {
        # pipe-no-c 25114 yamato    4w  FIFO   0,12      0t0     616532 pipe 25113,pipe-no-c,3r 25113,pipe-no-c,4w 25114,pipe-no-c,3r
        echo expected parent: ".* $child .* ${fdw}w *FIFO .* pipe $parent,p[-a-z]*,${fdr}r $parent,p[-a-z]*,${fdw}w $child,p[-a-z]*,${fdr}r"
        "$lsof" +E -p "$parent" |
            grep -q ".* $child .* ${fdw}w *FIFO .* pipe $parent,p[-a-z]*,${fdr}r $parent,p[-a-z]*,${fdw}w $child,p[-a-z]*,${fdr}r"
    } && {
        kill "$child"
        exit 0
    }

    kill "$child"
    exit 1
}
