#!/bin/bash
set -ev
here=$(dirname "$(readlink -f "$BASH_SOURCE")")
cd "$here"
flatc="../../contrib/flatbuffers/flatc"
"$flatc" -o imgui --cpp imgui/vectorCmd.fbs

# FIXME, needed by dhall
export PKG_CONFIG_OUTPUT_CFLAGS_FREETYPE2=""
export PKG_CONFIG_OUTPUT_LIBS_FREETYPE2=""
export PKG_CONFIG_OUTPUT_CFLAGS_GLFW3=""
export PKG_CONFIG_OUTPUT_LIBS_GLFW3=""
export CLANGDIR="$here/../../contrib/clang"

./cmakelists.dhall

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
generate_buildinfo > src/buildinfo.gen.h

mkdir -p build
cd build
cmake ../CMakeLists.txt
cmake --build . -j
mv *_exe ..
