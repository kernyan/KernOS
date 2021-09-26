#!/usr/bin/env bash

sudo apt install libpixman-1-dev libglib2.0-dev

git clone https://git.qemu.org/git/qemu.git
cd qemu
git checkout stable-5.0

cd ..
mkdir Build
cd Build
../qemu/configure --target-list=i386-softmmu
make -j$(nproc)
make install
