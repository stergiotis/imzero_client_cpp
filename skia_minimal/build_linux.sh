#!/bin/bash
set -ev
here=$(dirname "$(readlink -f "$BASH_SOURCE")")
cd "$here"
export IMGUI_SKIA_CPP_ROOT="$(realpath "$here")/.."
./copy_libs_linux.sh
./build_cpp_linux.sh