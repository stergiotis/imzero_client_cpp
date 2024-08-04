#!/bin/bash
set -v
here=$(dirname "$(readlink -f "$BASH_SOURCE")")
cd "$here/../../contrib/skia/"
echo "applying patches to skia m124"
sed -i -e "s/class SkTypeface_FreeType : public SkTypeface /class SK_API SkTypeface_FreeType : public SkTypeface /" src/ports/SkTypeface_FreeType.h
sed -i -e "s/-fvisibility=hidden//g" out/Shared/obj/modules/skparagraph/skparagraph.ninja || true
