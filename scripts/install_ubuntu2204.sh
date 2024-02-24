#!/bin/bash
if ! [ -x "$(command -v tar)" ]; then
	sudo apt install -y tar
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
if ! [ -x "$(command -v gcc)" ]; then
	sudo apt install -y gcc
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
