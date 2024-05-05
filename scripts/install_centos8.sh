#!/bin/bash
if ! [ -x "$(command -v tar)" ]; then
	sudo yum install -y tar
fi
if ! [ -x "$(command -v wget)" ]; then
	sudo yum install -y wget
fi
if ! [ -x "$(command -v cmake)" ]; then
	sudo yum install -y cmake
fi
if ! [ -x "$(command -v clang)" ]; then
	sudo yum install -y clang
fi
if ! [ -x "$(command -v gcc)" ]; then
	sudo yum install -y gcc
fi
if ! [ -x "$(command -v xsltproc)" ]; then
	sudo yum install -y xsltproc
fi
# freetype
sudo yum install -y freetype-devel

# glfw
sudo yum install -y epel-release
sudo yum install -y glfw glfw-devel

# opengl
sudo yum install -y libGL libGL-devel

# skia dependencies
sudo yum install -y clang libjpeg-devel libicu-devel libwebp-devel fontconfig-devel
