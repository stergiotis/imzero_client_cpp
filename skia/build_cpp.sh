#!/bin/bash
set -ev
here=$(dirname "$(readlink -f "$BASH_SOURCE")")
cd "$here"
flatc="../../contrib/flatbuffers/flatc"
"$flatc" -o main --cpp ../spec/ImZeroFB.fbs --reflect-types --reflect-names --filename-suffix .out
xxd -i ../spec/ImZeroFB.fbs main/ImZeroFB.fbs.gen.h

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
