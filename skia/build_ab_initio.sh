#!/bin/bash
set -ev
here=$(dirname "$(readlink -f "$BASH_SOURCE")")
cd "$here"

source /etc/os-release

../scripts/install.sh
../scripts/install_nonportable.sh
./build_skia_shared.sh
./build_flatbuffers.sh
./build_sdl3.sh
./build.sh
