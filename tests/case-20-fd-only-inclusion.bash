#!/intentionally/invalid/path/to/bash

#name=$( basename "$0" .bash )  <= IGNORED
lsof=$1
report=$2

[[ -n $report && $report != - ]] && exec >> "$report"

do1test() {
    local test_type=$1 request_pattern=$2 failure_pattern=$3 desc=$4
    printf '### %s test ###\n' "$test_type"
    if
        IFS= read -r line
    then
	printf 'Unexpectedly, %s is included:\n%s\n' "$desc" "$line"
        # 'cat' will read the remaining output from 'grep',
        # and thus show any other erroneous lines...
        cat

	printf '\n## whole output for debugging (%s):\n' \
                        "-d '$request_pattern' -F fd"
	"$lsof" -p $$ -a -d "$request_pattern" -F fd

	printf '## whole output for debugging (%s):\n' \
                        "-d '$request_pattern'"
	"$lsof" -p $$ -a -d "$request_pattern"

	printf '## whole output for debugging (%s):\n' "no -d"
	"$lsof" -p $$

        # And now give up!
	exit 1
    fi < <(
            "$lsof" -p $$ -a -d "$request_pattern" -F fd |
            grep -E "$failure_pattern"
        )
}

# TODO: consider simply inverting the test, effectively "grep -v ^f[0-9]"

do1test Inclusion 'fd' '^f[^0-9]' 'a non-filedescriptor'

do1test Exclusion '^fd' '^f[0-9]' 'a filedescriptor'

echo '### Success ###'

exit 0
