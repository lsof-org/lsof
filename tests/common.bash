name=$(basename $0 .bash)
if [ "$#" -ne 0 ]; then
	# Legacy
	lsof=$1
	report=$2
	tcasedir=$3
	dialect=$4
else
	# Autotools
	lsof=./lsof
	report=/dev/stdout
	# https://stackoverflow.com/questions/59895/how-do-i-get-the-directory-where-a-bash-script-is-located-from-within-the-script
	tcasedir="$( dirname -- "$( readlink -f -- "$0"; )"; )"
	dialect=dontcare
fi