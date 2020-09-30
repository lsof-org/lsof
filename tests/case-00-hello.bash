#!/intentionally/invalid/path/to/bash
#
# All the test scripts are invoked with
#
#   bash "$path_to_test.bash" "$path_to_lsof" "$path_to_report" "$(dirname "$path_to_test_dir")" "$dialect"
#
# therefore it is misleading to include a valid #! line, and even more
# misleading if it says anything other than "bash".
#


#
# An example of test case.
#

name=$( basename "$0" .bash )   # shellcheck disable=SC2034

#
# The file path for lsof executable.
#
lsof=$1     # shellcheck disable=SC2034

#
# Used only when a test case is failed.
# $report specifies a temporary file.
# Store how the test case is failed to the temporary file.
# The test harness uses this temporary file to make summary messages.
# The test harness removes this temporary file.
#
report=$2   # shellcheck disable=SC2034

#
# Directory where this test case is.
#
tcasedir=$3 # shellcheck disable=SC2034

#
# Dialect of lsof
#
dialect=$4  # shellcheck disable=SC2034

# Return 0 means the case is run successfully.
# Return 1 means the case is run in failure.
# Return 2 means the case is skipped.
exit 0
