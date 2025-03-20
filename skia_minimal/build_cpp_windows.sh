#!/bin/bash
set -ev
here=$(dirname "$(readlink -f "$BASH_SOURCE")")
cd "$here"
flatc="$here/contrib/flatbuffers/flatc"
"$flatc" -o imgui_skia_impl --cpp ../spec/ImZeroFB.fbs --reflect-types --reflect-names --filename-suffix .out

export PATH="$PATH:$HOME/bin"
./cmakelists.dhall

mkdir -p build
cd build
cmake -DCMAKE_BUILD_TYPE=Release -G "Ninja" ../CMakeLists.txt
#cmake --build . -j
#cmake -DCMAKE_BUILD_TYPE=Release -G "Unix Makefiles" ../CMakeLists.txt
#make -j
mv *_exe ..
