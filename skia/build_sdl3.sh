#!/bin/bash
here=$(dirname "$(readlink -f "$BASH_SOURCE")")
set -ev
cd ../../contrib/sdl
cmake -S . -B build -DSDL_STATIC=ON \
	            -DSDL_DYNAMIC=off \
		    -DCMAKE_BUILD_TYPE=RelWithDebInfo
cmake --build build --config RelWithDebInfo
