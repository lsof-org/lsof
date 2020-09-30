#!/intentionally/invalid/path/to/bash

name=$( basename "$0" .bash )
#lsof=$1    <= IGNORED
report=$2
dialect=$4

[[ -n $report && $report != - ]] && exec >> "$report"

f=/tmp/$name-$$.
__cleanup() { rm -f -- "$f"* ; } ; trap __cleanup EXIT

cd tests

make opt > "$f" 2>&1
s=$?

if ! grep -q "LTbigf \.\.\. OK" < "$f"; then
    echo '"LTbigf ... OK" is not found in the output'
    s=1
fi

# TODO: don't ignore "OK".
if ! grep -q "LTdnlc \.\.\. .*/tests found: 100.00%" < "$f"; then
    echo '"LTdnlc ... .*/tests found: 100.00%" is not found in the output'
    s=1
fi

if [[ -n "$CI" && "$dialect" = darwin ]] ; then
    :			# TODO: temporarily skip this
elif ! grep -q "LTlock \.\.\. OK" < "$f"; then
    echo '"LTlock ... OK" is not found in the output'
    s=1
fi

# TODO: LTnfs

echo
echo "output"
echo .............................................................................
cat "$f"

exit "$s"
