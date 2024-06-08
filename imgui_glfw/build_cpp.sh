#!/bin/bash
set -ev
here=$(dirname "$(readlink -f "$BASH_SOURCE")")
cd "$here"

export PKG_CONFIG_OUTPUT_CFLAGS_FREETYPE2=$(pkg-config --cflags freetype2)
export PKG_CONFIG_OUTPUT_LIBS_FREETYPE2=$(pkg-config --libs freetype2)
export PKG_CONFIG_OUTPUT_CFLAGS_GLFW3=$(pkg-config --cflags glfw3)
export PKG_CONFIG_OUTPUT_LIBS_GLFW3=$(pkg-config --libs glfw3)

./cmakelists.dhall

mkdir -p build
cd build
cmake ../CMakeLists.txt
cmake --build . -j
mv *_exe ..
