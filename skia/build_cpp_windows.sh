#!/bin/bash
set -ev
here=$(dirname "$(readlink -f "$BASH_SOURCE")")
cd "$here"

./cmakelists_windows.dhall

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
cmake  -G "Ninja" ..

# Patch ninja file: Use static linked msvc runtime
find . -name "*.ninja" | xargs -n 1 sed -i "s/-D_DLL / /g"
find . -name "*.ninja" | xargs -n 1 sed -i "s/-MD / /g"

# build
cmake --build . -j
