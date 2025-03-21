#!/bin/bash
set -ev
here=$(dirname "$(readlink -f "$BASH_SOURCE")")
cd "$here/../../contrib/sdl"
cmake -S . -B build -DSDL_STATIC=off \
	            -DSDL_DYNAMIC=on \
		    -DCMAKE_BUILD_TYPE=RelWithDebInfo
cmake --build build --config RelWithDebInfo -j
