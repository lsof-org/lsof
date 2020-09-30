#!/sourced/by/bash

lsof=$1
report=$2
tdir=$3
# dialect=$4    <= IGNORED
pat=$5
tfile=$6

shift 6

[[ -n $report && $report != - ]] && exec >> "$report"

target=$tdir/open_with_flags
if [[ ! -x $target ]]; then
    echo "target executable ($target) is not found"
    exit 1
fi

"$target" "$tfile" "$@" &
pid=$!
__cleanup() { kill "$pid" ; }
trap __cleanup EXIT

if  msg=$( "$lsof" +fg -p "$pid" ) &&
    grep -q "$pat" <<< "$msg"
then
    :
else
    r=$?
    printf 'Expected pattern: %s\n' "$pat"
    printf 'lsof output:\n%s\n' "$msg"
    exit $r
fi
