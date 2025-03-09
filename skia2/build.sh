#!/bin/bash
set -ev
here=$(dirname "$(readlink -f "$BASH_SOURCE")")
cd "$here"
source "env.sh"
export IMZERO_CLIENT_CPP_ROOT="$(realpath "$here")"
export IMGUI_SKIA_CPP_ROOT="$(realpath "$here/../skia_minimal")"
./copy_libs.sh
./build_cpp.sh
./build_go.sh
rm -rf bin
mkdir -p bin
mv imgui_skia_exe bin

mkdir -p .vscode
./vscode.dhall
