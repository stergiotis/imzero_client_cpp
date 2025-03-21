#!/bin/bash
set -ev
here=$(dirname "$(readlink -f "$BASH_SOURCE")")
cd "$here"
flatc="$here/../../contrib/flatbuffers/flatc"
"$flatc" -o imgui_skia_impl --cpp ../spec/ImZeroFB.fbs --reflect-types --reflect-names --filename-suffix .out

./cmakelists_linux.dhall

mkdir -p build
cd build
cmake -G Ninja .. -DCMAKE_CXX_COMPILER=clang++ \
	          -DCMAKE_C_COMPILER=clang
cmake --build . -j
