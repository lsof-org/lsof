#!/intentionally/invalid/path/to/bash
report=$2
tdir=$3

[[ -n $report && $report != - ]] && exec >> "$report"

# This make invocaiton is needed to
# run test cases locally, not CI environment.
make -C "$tdir" || exit 1

exit 0
