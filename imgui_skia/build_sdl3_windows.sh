#!/bin/bash
set -ev
here=$(dirname "$(readlink -f "$BASH_SOURCE")")
cd "$here/../../contrib/sdl"
mkdir -p build
cd build || exit 1
cmake -DSDL_STATIC=on \
      -DSDL_SHARED=off \
      -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_C_COMPILER=clang \
      -DCMAKE_CXX_COMPILER=clang++ \
      -DSDL_HIDAPI=off \
      -G "Ninja" ..
cmake --build . -j
