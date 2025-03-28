#!/bin/bash
set -ev
here=$(dirname "$(readlink -f "$BASH_SOURCE")")
cd "$here"
source ../imgui_skia/build_ab_initio.sh
./build_linux.sh
