#!/intentionally/invalid/path/to/bash

name=$( basename "$0" .bash )
#lsof=$1  <= IGNORED
report=$2

[[ -n $report && $report != - ]] && exec >> "$report"

s=0
# shellcheck disable=SC1037
f=/tmp/$name-$$-
cd tests

# TODO: check the return status of 'make'?
make > "$f" 2>&1

for p in "LTbasic ... OK" \
         "LTnlink ... OK" \
         "LTsock ... OK"  \
         "LTszoff ... OK" \
         "LTunix ... OK" 
do
    if ! grep -q -F "$p" < "$f"; then
        printf '"%s" is not found in the output\n' "$p"
        s=1
    fi
done

printf '\noutput\n.............................................................................\n'
cat "$f"

rm -f -- "$f"

exit "$s"
