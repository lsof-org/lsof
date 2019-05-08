set -ex

echo $1
echo $BASH_VERSION
shopt
export -p

./lsof -v

#
# check that the version numbers are updated
#
expected_version=$(sed '/VN/s/.ds VN \([0-9.a-z]*\)/\1/' ./version)
actual_version=$(./lsof -v 2>&1 | sed -ne 's/^ *revision: *\([0-9.a-z]*\)/\1/p')
dist_version=$(sed -ne 's/^\([0-9][0-9.a-z]*\)		.*$/\1/p' 00DIST | tail -1)
test "${expected_version}" = "${actual_version}"
test "${expected_version}" = "${dist_version}"
