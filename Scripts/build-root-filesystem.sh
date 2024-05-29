#!/bin/sh

#set -e

die() {
    echo "die: $*"
    exit 1
}

if [ "$(id -u)" != 0 ]; then
    die "this script needs to run as root"
fi

umask 0022

echo "installing base system... "
if command -v rsync >/dev/null; then
    rsync -aH --inplace ../Disk/ mnt/
else
    echo "Please install rsync"
fi

chmod -R g+rX,o+rX ../Disk/* mnt/
