#!/bin/bash
set -ev
here=$(dirname "$(readlink -f "$BASH_SOURCE")")
cd "$here"

source env.sh

flatc="../../contrib/flatbuffers/flatc"
"$flatc" -o src --cpp spec/userInteraction.fbs

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
mv imzero_video_play ..
