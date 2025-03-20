#!/bin/bash
set -ev
here=$(dirname "$(readlink -f "$BASH_SOURCE")")
cd "$here/../../contrib/sdl"
cmake -S . -B build -DSDL_STATIC=off \
	            -DSDL_SHARED=on \
		    -DCMAKE_BUILD_TYPE=Release \
		    -DCMAKE_C_COMPILER=clang \
		    -DCMAKE_CXX_COMPILER=clang++ \
		    -DSDL_HIDAPI=off \
		    -DCMAKE_CXX_COMPILER=clang++ \
		    -DCMAKE_C_COMPILER=clang
cmake --build build --config Release -j
