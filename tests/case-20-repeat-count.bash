#!/intentionally/invalid/path/to/bash

#name=$( basename "$0" .bash )  <= IGNORED
lsof=$1
report=$2

[[ -n $report && $report != - ]] && exec >> "$report"

#[[ -n $report && $report != - ]] || report=/dev/stderr
#exec 2>&1

if msg=$( "$lsof" -r 1c1 -p $$ ) &&
   num=$( <<<"$msg" grep -cFx ======= ) &&
   (( num == 1 )); then
    :
else
    printf 'Expected %u reports (each terminated by "======="), got %u\n' 1 "$num"
    printf '%s\n' "$msg"
    exit 1
fi

if msg=$( "$lsof" -r 1c5 -p $$ ) &&
   num=$( <<<"$msg" grep -cFx ======= ) &&
   (( num == 5 )); then
    :
else
    printf 'Expected %u reports (each terminated by "======="), got %u\n' 5 "$num"
    printf '%s\n' "$msg"
    exit 1
fi

exit 0
