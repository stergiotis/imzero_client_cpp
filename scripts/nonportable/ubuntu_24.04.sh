#!/bin/bash
echo "installing dependencies for ubuntu noble (24.04)"
if ! [ -x "$(command -v tar)" ]; then
	sudo apt install -y tar
fi
if ! [ -x "$(command -v bzip2)" ]; then
	sudo apt install -y bzip2
fi
if ! [ -x "$(command -v wget)" ]; then
	sudo apt install -y wget
fi
if ! [ -x "$(command -v cmake)" ]; then
	sudo apt install -y cmake
fi
if ! [ -x "$(command -v clang)" ]; then
	sudo apt install -y clang
fi
if ! [ -x "$(command -v xsltproc)" ]; then
	sudo apt install -y xsltproc
fi
# freetype
sudo apt install -y libfreetype-dev

# glfw
sudo apt install -y libglfw3-dev

# opengl
sudo apt install -y libgl-dev

#libc++
sudo apt install -y libc++-dev libc++abi-dev

#skia deps
sudo apt install -y clang libjpeg-dev libicu-dev libwebp-dev libfontconfig-dev libglu1-mesa-dev

#clang (non-OS version) deps
sudo apt install libtinfo6
