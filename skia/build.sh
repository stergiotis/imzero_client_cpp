#!/bin/bash
set -ev
here=$(dirname "$(readlink -f "$BASH_SOURCE")")
cd "$here"
source "env.sh"
export IMZERO_CLIENT_CPP_ROOT=$(realpath $here)
./build_go.sh
./build_cpp.sh

mkdir -p .vscode
./vscode.dhall
