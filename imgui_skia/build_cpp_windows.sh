#!/bin/bash
set -ev
here=$(dirname "$(readlink -f "$BASH_SOURCE")")
cd "$here"
flatc="$here/../../contrib/flatbuffers/flatc"
"$flatc" -o imgui_skia_impl --cpp ../spec/ImZeroFB.fbs --reflect-types --reflect-names --filename-suffix .out

export PATH="$PATH:$HOME/bin"
./cmakelists_windows.dhall

mkdir -p build
cd build
cmake -G "Ninja" ..
# Patch ninja file: Use static linked msvc runtime
find . -name "*.ninja" | xargs -n 1 sed -i "s/-D_DLL / /g"
find . -name "*.ninja" | xargs -n 1 sed -i "s/-MD / /g"

# build
cmake --build . -j
