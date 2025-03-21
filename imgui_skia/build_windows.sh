#!/bin/bash
set -ev
here=$(dirname "$(readlink -f "$BASH_SOURCE")")
cd "$here"
export IMGUI_SKIA_CPP_ROOT="$(realpath "$here")/.."
./copy_libs_windows.sh
./build_cpp_windows.sh
#rm -rf bin
#mkdir -p bin
#mv imgui_skia_exe bin
