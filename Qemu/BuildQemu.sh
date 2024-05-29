#! /bin/bash -ex

if command -v pacman &> /dev/null
then
    echo "Detected Arch Linux"
    sudo pacman -Syu --needed pixman glib2 gtk3 gtk3-demos glu freeglut mesa sdl2
elif command -v apt &> /dev/null
then
    echo "Detected Ubuntu"
    sudo apt update
    sudo apt install libpixman-1-dev libglib2.0-dev libgtk-3-dev gtk-3-examples libglu1-mesa-dev freeglut3-dev mesa-common-dev libsdl2-dev
else
    echo "Unsupported OS"
    exit 1
fi

if [ ! -d "qemu" ]; then
    git clone https://git.qemu.org/git/qemu.git
fi
cd qemu
git checkout stable-8.2

cd ..
rm -rf Build
mkdir Build
cd Build
../qemu/configure --target-list=i386-softmmu # --disable-werror
make -j$(nproc)
echo "Add $(realpath .) to path"
# make install
