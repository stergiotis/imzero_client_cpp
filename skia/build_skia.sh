#!/bin/bash
here=$(dirname "$(readlink -f "$BASH_SOURCE")")
set -ev
cd ../../contrib/skia
#sudo apt install clang libjpeg-dev libicu-dev libwebp-dev libfontconfig-dev
./tools/install_dependencies.sh
python3 tools/git-sync-deps
./bin/fetch-ninja
#ninja -t clean
#bin/gn args out/Static --list
#./bin/gn args out/Static --list --short
./bin/gn gen out/Static --args="is_official_build=false cc=\"clang\" cxx=\"clang++\" is_debug=false is_component_build=false skia_use_gl=true skia_use_system_expat=false skia_use_system_freetype2=false skia_use_system_harfbuzz=false skia_use_system_icu=false skia_use_system_libjpeg_turbo=false skia_use_system_libpng=false skia_use_system_libwebp=false skia_use_system_zlib=false"

./third_party/ninja/ninja -v -d keeprsp -C out/Static
#./out/Static/viewer
