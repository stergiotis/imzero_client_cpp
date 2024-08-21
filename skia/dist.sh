#!/bin/bash
set -ev
version=$(git log -1  --pretty=format:"%H" | tr -d "\n")
if [[ $(git diff --stat) != '' ]]; then
   version+="_dirty"
fi
arch=$(uname -p)
fn="dist/imzero_client_skia_${version:0:8}_${arch}"
echo "dist=${fn}"
rm -rf "$fn"
mkdir -p "$fn"
find . -maxdepth 1 -name "*exe*" -type f -exec cp -v {} "$fn" \;
cp -v ../../contrib/sdl/build/libSDL3.so.0 "$fn"
cp -v ../../contrib/skia/out/Shared/libskparagraph.so "$fn"
cp -v ../../contrib/skia/out/Shared/libskia.so "$fn"
cp -v ../../contrib/skia/out/Shared/libskunicode.so "$fn"
cp -v ../../contrib/skia/out/Shared/libbentleyottmann.so "$fn"
cp -v ../../contrib/skia/out/Shared/libskshaper.so "$fn"

tar cvfJ "${fn}.tar.xz" "$fn"
