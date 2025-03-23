#!/bin/bash
set -ev
here=$(dirname "$(readlink -f "$BASH_SOURCE")")
cd "$here"
source "env.sh"
export IMZERO_CLIENT_CPP_ROOT="$(realpath "$here")"
export IMGUI_SKIA_CPP_ROOT="$(realpath "$here/..")"
#./copy_libs_windows.sh
./build_cpp_windows.sh
./build_go_windows.sh
rm -f bin/*.exe
mkdir -p bin
mv build/imzeroClientSkiaSdl3Impl.exe bin/imgui_skia.exe
