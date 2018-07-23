#!/bin/sh
#
# get-hdr-loc.sh -- get XNU kernel header file location
#
# Interactively requests the specification of the path to the host's
# XNU kernel header files.  Checks that path and returns it to the
# caller.
#
# Usage: file1 file2 ... fileN
#
# Where: file1	first header file needed
#	 file2	second header file needed
#	 ...
#	 fileN	last header file needed
#
# Exit:
#
#	Exit code: 0 if path found; path returned on STDOUT
#
#		   1 if path not found: error message returned
#					on STDOUT
#
#set -x	# for DEBUGging

# Check argument count.  There must be at least one argument.

if test $# -lt 1	# {
then
  echo "insufficient arguments: $#"
  exit 1
fi	# }

# Request the path to the XNU kernel header files.

trap 'stty echo; echo interrupted; exit 1' 1 2 3 15
FOREVER=1
while test $FOREVER -ge 1	# {
do
  if test $FOREVER -eq 1	# {
  then
    cat << .CAT_MARK

------------------------------------------------------------------

Lsof makes use of XNU kernel header files.  They must have already
been downloaded from:

    http://www.opensource.apple.com/darwinsource/index.html

Now you must specify the path to the place to which they were
downloaded.

------------------------------------------------------------------
.CAT_MARK
  fi	# }

  END=0
  while test $END = 0	# {
  do
    echo ""
    echo -n "What is the path? "
    read HP EXCESS
    if test "X$HP" = "X"	# {
    then
      echo ""
      echo "+================================+"
      echo "| Please enter a non-empty path. |"
      echo "+================================+"
      echo ""
    else
      END=1
    fi	# }
  done	# }

  # See if the header files are available in the specified path.

  MH=""
  for i in $*	# {
  do
    if test ! -r ${HP}/$i	# {
    then
      if test "X$MH" = "X"	# {
      then
        MH=$i
      else
        MH="$MH $i"
      fi	# }
    fi	# }
  done	# }
  if test "X$MH" = "X"	# {
  then

    # All header files are available, so return the path and exit cleanly.

    echo $HP
    exit 0
  else
    echo ""
    echo "ERROR: not all header files are in:"
    echo ""
    echo "     ${HP}"
    echo ""
    echo " These are missing:"
    echo ""
    echo "     $MH"
    FOREVER=2
  fi	# }
done	# }
echo "unknown error"
exit 1
