#!/bin/bash
set -ev
here=$(dirname "$(readlink -f "$BASH_SOURCE")")
cd "$here"
rm -rf lib
mkdir -p lib
copy() {
   ln -v $(realpath "$1") lib/$(basename "$1")
}
#copy ../../contrib/sdl/build/libSDL3.so.0
copy ../../contrib/skia/out/Shared/libskparagraph.so
copy ../../contrib/skia/out/Shared/libskia.so
copy ../../contrib/skia/out/Shared/libskunicode.so
copy ../../contrib/skia/out/Shared/libbentleyottmann.so
copy ../../contrib/skia/out/Shared/libskshaper.so
copy ../../contrib/skia/out/Shared/libskunicode_core.so
copy ../../contrib/skia/out/Shared/libskunicode_icu.so
