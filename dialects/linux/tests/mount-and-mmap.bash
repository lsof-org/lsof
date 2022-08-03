#!/bin/sh
set -e

lsof=$1
report=$2
tdir=$3

TARGET=$tdir/mmap
if ! [ -x $TARGET ]; then
    echo "target executable ( $TARGET ) is not found" >> $report
    exit 1
fi

mkdir /tmp/TEST
dd if=/dev/zero of=/tmp/ext4.img bs=40MB count=1
mkfs.ext4 /tmp/ext4.img
mount --make-private /tmp/ext4.img /tmp/TEST

dd if=/dev/zero of=/tmp/TEST/MMAP bs=2MB count=1
$TARGET /tmp/TEST/MMAP &
