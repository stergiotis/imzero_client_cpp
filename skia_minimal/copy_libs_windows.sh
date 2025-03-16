#!/bin/bash
set -ev
here=$(dirname "$(readlink -f "$BASH_SOURCE")")
cd "$here"
rm -rf lib
mkdir -p lib
copy() {
   cp "$(realpath "$1")" "lib/$(basename "$1")"
}
copy "$here/contrib/sdl/build/RelWithDebInfo/SDL3.dll"
copy "$here/../../contrib/skia/out/Shared/skparagraph.dll"
copy "$here/../../contrib/skia/out/Shared/skparagraph.dll.lib"
copy "$here/../../contrib/skia/out/Shared/skia.dll"
copy "$here/../../contrib/skia/out/Shared/skia.dll.lib"
copy "$here/../../contrib/skia/out/Shared/skunicode_core.dll"
copy "$here/../../contrib/skia/out/Shared/skunicode_core.dll.lib"
copy "$here/../../contrib/skia/out/Shared/skunicode_icu.dll"
copy "$here/../../contrib/skia/out/Shared/skunicode_icu.dll.lib"
copy "$here/../../contrib/skia/out/Shared/bentleyottmann.dll"
copy "$here/../../contrib/skia/out/Shared/bentleyottmann.dll.lib"
copy "$here/../../contrib/skia/out/Shared/skshaper.dll"
copy "$here/../../contrib/skia/out/Shared/skshaper.dll.lib"
