#!/bin/bash
set -ev
here=$(dirname "$(readlink -f "$BASH_SOURCE")")
cd "$here"
flatc="$here/contrib/flatbuffers/flatc"
"$flatc" -o imgui_skia_impl --cpp ../spec/ImZeroFB.fbs --reflect-types --reflect-names --filename-suffix .out

./cmakelists.dhall

mkdir -p build
cd build
cmake ../CMakeLists.txt -G Ninja
cmake --build . -j
mv *_exe ..
