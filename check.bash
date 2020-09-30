set -e

die() { e=$1 ; shift ; printf >&2 '%s\n' "$*" ; exit $e ; }

(( $# )) ||
    die 1 "Usage: $0 DIALECT"

tmp=/tmp/check-$$-
__cleanup() { rm -f -- "$tmp"* ; }
trap __cleanup EXIT

dialect=$1
ddir=dialects/$dialect
lsof=$(pwd)/lsof

[[ -d $ddir ]] ||
    die 1 "No such dialect: $dialect"

printf '\nRUNTIME ENVIRONMENT INFORMATION\n=============================================================================\n'
printf '%s\n' "$dialect"
printf '%s\n' "$BASH_VERSION"
export

printf '\nOTHER RUNTIME INFORMATION\n'
shopt
uname

printf '\nDETAILS OF lsof PROGRAM\n'
ls -ld "$lsof"
"$lsof" -v

export CI=1

nfailed=0
nsuccessful=0
nskipped=0
ncases=0
failing_reports=()
skipped_reports=()

run_one()
{
    local x=$1
    local d=${1%/*}
    local name
    local prefix
    local report
    local s r

    chmod a+x $x
    name=$(basename $x .bash)
    if [[ $x = dialects/* ]]; then
	prefix=$dialect-
    fi
    report=$tmp$prefix$name.report

    printf "%s ... " $name

    ncases=$((ncases + 1))
    if
        bash "$x" "$lsof" "$report" "$d" "$dialect"
    then
        r=$?
	s=ok
	((nsuccessful++))
	rm -f "$report"
    else
        r=$?
        if (( r == 2 )); then
            s=skipped
            skipped_reports[nskipped++]=$report
        else
            s=failed
            failing_reports[nfailed++]=$report
        fi
    fi

    printf "%s\n" "$s"
}

echo
echo "STARTING TEST (dialect-neutral)"
echo "============================================================================="
for x in 'tests/case-'*.bash; do
    run_one "$x"
done

echo
echo "STARTING TEST ($dialect-specific)"
echo "============================================================================="

for x in "$ddir/tests/case-"*.bash; do
    run_one "$x"
done

report()
{
    for r do
	printf '\n%s\n-----------------------------------------------------------------------------\n' "$r"
	cat "$r"
    done
    rm -f -- "$@"
}

printf '\nTEST SUMMARY\n=============================================================================\n'
printf "successful: %d\n" "$nsuccessful"
printf "skipped: %d\n" "$nskipped"
printf "failed: %d\n" "$nfailed"

is_are=( are is )
has_have=( have has )
was_were=( were was )
s_s=( s '' )

if (( nfailed == 0 )); then
    printf "All %d test case%s %s passed successfully\n" $((ncases)) "${s_s[ncases]-$s_s}" "${has_have[ncases]-$has_have}"
    if (( nskipped != 0 )); then
        printf "but %d case%s %s skipped\n" $((nskipped)) "${s_s[nskipped]-$s_s}" "${was_were[nskipped]-$was_were}"
	report "${skipped_reports[@]}"
    fi
    exit 0
else
    printf "%d of %d case%s %s failed\n" $((nfailed)) $((ncases)) "${s_s[ncases]-$s_s}" "${has_have[nfailed]-$has_have}"
    if (( nskipped != 0 )); then
        printf "and %d case%s %s skipped\n" $((nskipped)) "${s_s[nskipped]-$s_s}" "${was_were[nskipped]-$was_were}"
    fi
    report "${failing_reports[@]}" "${skipped_reports[@]}"
    exit 1
fi
