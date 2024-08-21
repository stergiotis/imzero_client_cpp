#!/bin/bash
here=$(dirname "$(readlink -f "$BASH_SOURCE")")
set -ev
cd ../../contrib/skia
python3 tools/git-sync-deps

clangdir="$here/../../contrib/clang"
mkdir -p $clangdir
#CC= CXX= infra/bots/assets/clang_linux/create.py -t "$clangdir"

mkdir -p out/Static
cat > out/Static/args.gn <<- EOF
    cc = "${clangdir}/bin/clang"
    cxx = "${clangdir}/bin/clang++"
    extra_ldflags = [ "-fuse-ld=lld", "-Wl,-rpath,${clangdir}/lib/x86_64-unknown-linux-gnu" ]
EOF

./bin/fetch-ninja
#ninja -t clean
#bin/gn args out/Static --list
#./bin/gn args out/Static --list --short
./bin/gn gen out/Static --args="is_official_build=false cc=\"clang\" cxx=\"clang++\" is_debug=false is_trivial_abi=false is_component_build=false skia_use_gl=true skia_use_system_expat=false skia_use_system_freetype2=false skia_use_system_harfbuzz=false skia_use_system_icu=false skia_use_system_libjpeg_turbo=false skia_use_system_libpng=false skia_use_system_libwebp=false skia_use_system_zlib=false"

./third_party/ninja/ninja -v -d keeprsp -C out/Static
#./out/Static/viewer
