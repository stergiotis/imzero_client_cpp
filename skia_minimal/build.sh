#!/bin/bash
set -ev
here=$(dirname "$(readlink -f "$BASH_SOURCE")")
cd "$here"
export IMGUI_SKIA_CPP_ROOT="$(realpath "$here")/.."
./copy_libs.sh
./build_cpp.sh
rm -rf bin
mkdir -p bin
mv imgui_skia_exe bin
