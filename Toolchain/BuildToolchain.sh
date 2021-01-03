#!/usr/bin/env bash

#sudo apt install build-essential bison flex libgmp3-dev libmpc-dev \
#   libmpfr-dev texinfo

DIR="$PWD"
echo "$DIR"
ARCH="i686"
echo "--Building toolchain for $ARCH in $DIR"


PREFIX="$HOME/opt/cross"
TARGET="i686-elf"
PATH="$PREFIX/bin:$PATH"

echo "--Configuring binutils-2.35 for $TARGET in $PREFIX"

wget "https://ftp.gnu.org/gnu/binutils/binutils-2.35.tar.gz"
tar -xzf binutils-2.35.tar.gz
rm binutils-2.35.tar.gz
mkdir build-binutils

cd build-binutils
../binutils-2.35/configure --target=$TARGET --prefix="$PREFIX" \
   --with-sysroot --disable-nls --disable-werror

make -j$(nproc)
make install

which -- $TARGET-as || echo $TARGET-as is not in the PATH

wget "https://ftp.gnu.org/gnu/gcc/gcc-10.2.0/gcc-10.2.0.tar.gz"
tar -xzf gcc-10.2.0.tar.gz
rm gcc-10.2.0.tar.gz

echo "--Configuring gcc-10.2.0 for $TARGET in $PREFIX"

mkdir build-gcc
cd build-gcc
../gcc-10.2.0/configure --target="$TARGET" --prefix="$PREFIX" \
   --disable-nls --enable-languages=c,c++ --without-headers

make all-gcc -j$(nproc)
make all-target-libgcc -j$(nproc)
make install-gcc
make install-target-libgcc







