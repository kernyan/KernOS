#!/bin/sh

#set -e

die() {
    echo "die: $*"
    exit 1
}

if [ "$(id -u)" != 0 ]; then
    exec sudo -E -- "$0" "$@" || die "this script needs to run as root"
else
    : "${SUDO_UID:=0}" "${SUDO_GID:=0}"
fi

disk_usage() {
  expr "$(du -sk --apparent-size "$1" | cut -f1)" / 1024
}

DISK_SIZE=$(($(disk_usage "../Disk") + 100))
DISK_SIZE_BYTES=$((DISK_SIZE * 1024 * 1024))
unset DISK_SIZE

USE_EXISTING=0

if [ $USE_EXISTING -ne 1 ]; then
    printf "setting up disk image... "
    qemu-img create -q -f raw _disk_image $DISK_SIZE_BYTES || die "could not create disk image"
    chown "$SUDO_UID":"$SUDO_GID" _disk_image || die "could not adjust permissions on disk image"
    echo "done"

    printf "creating new filesystem... "
    if [ -x /sbin/mke2fs ]; then
        /sbin/mke2fs -q -I 128 _disk_image || die "could not create filesystem"
    else
        die "could not create filesystem"
    fi
    echo "done"
fi

printf "mounting filesystem... "
mkdir -p mnt

mount_cmd="mount _disk_image mnt/"

if ! eval "$mount_cmd"; then
  die "could not mount filesystem and genext2fs is missing"
else
  echo "done"
fi
