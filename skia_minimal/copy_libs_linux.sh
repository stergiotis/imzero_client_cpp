#!/bin/bash
set -ev
here=$(dirname "$(readlink -f "$BASH_SOURCE")")
cd "$here"
rm -rf lib
mkdir -p lib
copy() {
   ln -v "$(realpath "$1")" "lib/$(basename "$1")"
}
copy "$here/contrib/sdl/build/libSDL3.so.0"
copy "$here/contrib/skia/out/Shared/libskparagraph.so"
copy "$here/contrib/skia/out/Shared/libskia.so"
copy "$here/contrib/skia/out/Shared/libskunicode.so"
copy "$here/contrib/skia/out/Shared/libbentleyottmann.so"
copy "$here/contrib/skia/out/Shared/libskshaper.so"