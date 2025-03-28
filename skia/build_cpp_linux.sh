#!/bin/bash
set -ev
here=$(dirname "$(readlink -f "$BASH_SOURCE")")
cd "$here"

./cmakelists_linux.dhall

generate_buildinfo() {
   echo -en "#pragma once\nnamespace buildinfo {\n static const char *gitCommit=\""
   git log -1  --pretty=format:"%H" | tr -d "\n"
   echo -en "\";\n static const bool gitDirty="
   if [[ $(git diff --stat) != '' ]]; then
     echo -n "true"
   else
     echo -n "false"
   fi
   echo -en ";\n}\n"
}
generate_buildinfo > "$here/imzero_client_skia_sdl3_impl/buildinfo.gen.h"

mkdir -p build
cd build
cmake -G Ninja .. -DCMAKE_CXX_COMPILER=clang++ \
	          -DCMAKE_C_COMPILER=clang
cmake --build . -j
