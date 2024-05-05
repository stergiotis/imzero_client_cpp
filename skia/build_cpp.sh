#!/bin/bash
set -ev
here=$(dirname "$(readlink -f "$BASH_SOURCE")")
cd "$here"
flatc="../../contrib/flatbuffers/flatc"
"$flatc" -o imgui --cpp imgui/vectorCmd.fbs

./cmakelists.dhall

mkdir -p build
cd build
cmake ../CMakeLists.txt
cmake --build . -j
mv *_exe ..
