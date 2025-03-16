#!/bin/bash
set -ev
here=$(dirname "$(readlink -f "$BASH_SOURCE")")
cd "$here/../../contrib/skia/"
echo "applying patches to skia m124"
sed -i -e "s/class SkTypeface_FreeType : public SkTypeface /class SK_API SkTypeface_FreeType : public SkTypeface /" src/ports/SkTypeface_FreeType.h
skparaninja="out/Shared/obj/modules/skparagraph/skparagraph.ninja"
if [ -f $skparaninja ]; then
  sed -i -e "s/-fvisibility=hidden//g" $skparaninja || true
else
  echo "careful, file $skparaninja does not yet exist --> compile twice".
fi
sed -i -e "s,[a-z]+[.]googlesource[.]com/external/,,g" DEPS || true
sed -i -z 's,#include \"include/core/SkTypes.h\"\n#include \"include/private/base/SkDebug.h\",#include \"include/core/SkTypes.h\"\n#include \"include/ports/SkFontMgr_fontconfig.h\"\n#include \"include/private/base/SkDebug.h\",' "src/ports/SkFontMgr_fontconfig.cpp" || true
