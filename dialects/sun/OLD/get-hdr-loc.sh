#!/bin/sh
#
# get-hdr-loc.sh -- get Solaris ZFS kernel header files location
#
# Interactively requests the specification of the path to the host's Solaris
# ZFS kernel header files.  Checks that path and returns it to the caller.
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
#		     If a NULL path ("") is returned, lsof is to use
#		     it's internal ZFS structure definitions.
#
#		   1 if path not found or no ZFS support desired
#		     On error a message is returned on STDOUT;
#		     NULL ("") is returned if no ZFS support is
#		     desired.
#
#set -x	# for DEBUGging

# Check argument count.  There must be at least one argument.

if test $# -lt 1	# {
then
  echo "insufficient arguments: $#"
  exit 1
fi	# }
lst=$*
trap 'stty echo; echo interrupted; exit 1' 1 2 3 15

# See if ZFS support is desired.

echo "----------------------------------------------------------" 1>&2
echo "" 1>&2
echo "The Configure script has detected that the ZFS file system" 1>&2
echo "can be used on this system, but the script cannot find all" 1>&2
echo "the header files that describe the ZFS kernel structures" 1>&2
echo "whose definitions lsof needs to know." 1>&2
echo "" 1>&2
echo "You can choose to build lsof with or without ZFS support." 1>&2
echo "" 1>&2
echo "If you choose to build lsof with ZFS support, you will be" 1>&2
echo "able to choose how the ZFS kernel structure definitions" 1>&2
echo "are provided." 1>&2
echo "" 1>&2
echo "----------------------------------------------------------" 1>&2
END=0
while test $END -eq 0	# {
do
  echo "" 1>&2
  echo "Do you want ZFS support (y|n) [y]? \c" 1>&2
  read ANS EXCESS
  if test "X$ANS" = "Xn" -o "X$ANS" = "XN"	# {
  then
    echo ""
    exit 1
  fi	# }
  if test "X$ANS" = "Xy" -o "X$ANS" = "XY" -o "X$ANS" = "X"	# {
  then
    END=1
  else
    echo ""
    echo "Please answer y|n [y]." 2>&1
  fi	# }
done	# }

# Determine the ZFS action to take -- use internal definitions or use
# the header files in a specified location.

echo "" 1>&2
echo "----------------------------------------------------------" 1>&2
echo "" 1>&2
echo "Lsof can use its own definitions of ZFS kernel structures," 1>&2
echo "but they may not accurately reflect the structures used on" 1>&2
echo "the kernel on this system." 1>&2
echo "" 1>&2
echo "If you have access to header files that accurately reflect" 1>&2
echo "the ZFS kernel structures used on this system, answer 'n'" 1>&2
echo "or 'N' to this first question and supply the path to the" 1>&2
echo "header files as an answer to the later question that asks" 1>&2
echo "for the path." 1>&2
echo "" 1>&2
echo "----------------------------------------------------------" 1>&2
END=0
while test $END -eq 0	# {
do
  echo "" 1>&2
  echo "Should lsof use its own definitions (y|n) [y]? \c" 1>&2
  read ANS EXCESS
  if test "X$ANS" = "Xn" -o "X$ANS" = "XN"	# {
  then
    END=1
  else
    if test "X$ANS" = "Xy" -o "X$ANS" = "XY" -o "X$ANS" = "X"	# {
    then
      echo ""
      exit 0
    else
      echo ""
      echo "BPlease answer y|n [y]." 2>&1
    fi	# }
  fi	# }
done	# }

# Get the path to the ZFS header files.

FOREVER=1
while test $FOREVER -ge 1	# {
do
  END=0
  while test $END -eq 0	# {
  do
    echo "" 1>&2
    echo "What is the path to the ZFS header files? \c" 1>&2
    read ANS EXCESS
    ZP=`echo echo $ANS | /bin/csh -fs`
    if test $? -eq 0	# {
    then
      if test "X$ZP" = "X"	# {
      then
	echo "" 1>&2
	echo "+================================+" 1>&2
	echo "| Please enter a non-empty path. |" 1>&2
	echo "+================================+" 1>&2
	echo "" 1>&2
      else
	END=1
      fi	# }
    else
      echo "" 1>&2
      echo "+============================+" 1>&2
      echo "| Please enter a legal path. |" 1>&2
      echo "+============================+" 1>&2
      echo "" 1>&2
    fi	# }
  done	# }
  
  # See if the header files are available in the specified path.

  MH=""
  for i in $lst	# {
  do
    if test ! -f ${ZP}/sys/$i -o ! -r ${ZP}/sys/$i    # {
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

    echo $ZP
    exit 0
  else
    echo "" 1>&2
    echo "ERROR: not all header files are in:" 1>&2
    echo "" 1>&2
    echo "       ${ZP}/sys" 1>&2
    echo "" 1>&2
    echo " These are missing:" 1>&2
    echo "" 1>&2
    echo "     $MH" 1>&2
  fi	# }
done	# }
echo "unknown error"
exit 1
