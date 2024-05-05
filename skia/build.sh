#!/bin/bash
set -ev
here=$(dirname "$(readlink -f "$BASH_SOURCE")")
cd "$here"
./build_go.sh
./build_cpp.sh

mkdir -p .vscode
./vscode.dhall
