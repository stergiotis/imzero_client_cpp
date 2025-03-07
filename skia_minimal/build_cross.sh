#!/bin/bash
set -ev
here=$(dirname "$(readlink -f "$BASH_SOURCE")")
cd "$here" || exit 1

base="$here/cross_compile/windows/amd64"
"$base/run.sh"
#export PATH="$base/:$PATH"
#export CMAKE_TOOLCHAIN_FILE="$base/cmake.toolchain"
./build.sh
