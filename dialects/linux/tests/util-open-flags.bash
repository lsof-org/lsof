lsof=$1
report=$2
tdir=$3
dialect=$4
pat=$5
tfile=$6

shift 6

TARGET=$tdir/open_with_flags
if ! [ -x $TARGET ]; then
    echo "target executable ( $TARGET ) is not found" >> $report
    exit 1
fi

export LC_ALL=C

$TARGET $tfile "$@" 2>> $report | {
    read pid

    if  [[ -z "$pid" ]]; then
	if grep -q 'open: Operation not supported' $report; then
	    echo "a flag passed to open is not supported on this platform" >> $report
	    exit 2
	fi
	echo "unexpected output form target ( $TARGET )" >> $report
	exit 1
    fi
    if ! [ -e "/proc/$pid" ]; then
	echo "the target process dead unexpectedly" >> $report
	exit 1
    fi

    echo "expected: $pat" >> $report
    echo "lsof output:" >> $report
    $lsof +fg -p $pid | tee -a $report | grep -q "$pat"
    result=$?

    kill $pid
    exit $result
}
