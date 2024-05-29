#!/usr/bin/env bash

set -e

if command -v pacman &> /dev/null
then
    echo "Detected Arch Linux"
    sudo pacman -Syu --needed base-devel bison flex gmp mpc mpfr texinfo
elif command -v apt &> /dev/null
then
    echo "Detected Ubuntu"
    sudo apt update
    sudo apt install build-essential bison flex libgmp3-dev libmpc-dev libmpfr-dev texinfo
else
    echo "Unsupported OS"
    exit 1
fi

DIR="$PWD"
echo "$DIR"
ARCH="i686"
echo "--Building toolchain for $ARCH in $DIR"

PREFIX="$HOME/opt/cross"
TARGET=i686-elf
PATH="$PREFIX/bin:$PATH"

echo "--Configuring binutils-2.37 for $TARGET in $PREFIX"

wget "https://ftp.gnu.org/gnu/binutils/binutils-2.37.tar.gz"
tar -xzf binutils-2.37.tar.gz
rm binutils-2.37.tar.gz
mkdir build-binutils

cd build-binutils
../binutils-2.37/configure --target=$TARGET --prefix="$PREFIX" \
   --with-sysroot --disable-nls --disable-werror

make -j$(nproc)
make install

which -- $TARGET-as || echo $TARGET-as is not in the PATH

cd ..

wget "https://ftp.gnu.org/gnu/gcc/gcc-11.2.0/gcc-11.2.0.tar.gz"
tar -xzf gcc-11.2.0.tar.gz
rm gcc-11.2.0.tar.gz

echo "--Configuring gcc-11.2.0 for $TARGET in $PREFIX"

mkdir build-gcc
cd build-gcc
../gcc-11.2.0/configure --target=$TARGET --prefix="$PREFIX" \
   --disable-nls --enable-languages=c,c++ --without-headers

make all-gcc -j$(nproc)
make all-target-libgcc -j$(nproc)
make install-gcc
make install-target-libgcc
