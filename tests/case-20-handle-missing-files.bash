#!/intentionally/invalid/path/to/bash
# See https://github.com/lsof-org/lsof/issues/90

#name=$( basename "$0" .bash )  <=  IGNORED
lsof=$1
report=$2

[[ -n $report && $report != - ]] && exec >> "$report"

if 
    msg=$( "$lsof" /NO-SUCH-FILE 2>&1 )
    [[ "$msg" = *': status error on /NO-SUCH-FILE: No such file or directory'* ]]
then
    exit 0
else
    echo "unexpected output: "
    echo "$msg"
    exit 1
fi
