#!/bin/bash
set -ev
here=$(dirname "$(readlink -f "$BASH_SOURCE")")
cd "$here"
flatc="../../contrib/flatbuffers/flatc"
"$flatc" -o imgui --cpp ../spec/ImZeroFB.fbs --reflect-types --reflect-names --filename-suffix .out

./cmakelists.dhall

mkdir -p build
cd build
cmake ../CMakeLists.txt
cmake --build . -j
mv *_exe ..
