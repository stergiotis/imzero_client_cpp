#!/bin/bash
set -ev
here=$(dirname "$(readlink -f "$BASH_SOURCE")")
cd "$here"
source "env.sh"
export IMZERO_CLIENT_CPP_ROOT="$(realpath "$here")"
export IMGUI_SKIA_CPP_ROOT="$(realpath "$here/..")"
./copy_libs.sh
./build_cpp.sh
./build_go.sh
rm -rf bin
mkdir -p bin
mv build/imzeroClientSkiaSdl3Impl* bin/imgui_skia_exe

mkdir -p .vscode
./vscode.dhall
