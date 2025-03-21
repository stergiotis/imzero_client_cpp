#!/bin/bash
set -ev
here=$(dirname "$(readlink -f "$BASH_SOURCE")")

# relevant issues (on ubuntu 22.10):
# * -Wpsabi with no avx512f in march
#   - https://groups.google.com/a/skia.org/g/bugs/c/4GuY7jjfCzY
#   - https://bugs.chromium.org/p/chromium/issues/detail?id=1112929&no_tracker_redirect=1
# * harfbuzz 2.x not having hb-subset.h for pdf support:
#   - https://groups.google.com/a/skia.org/g/bugs/c/4GuY7jjfCzY
# * missing SK_API declarations for defitions in SkParagraph:
#   - https://skia-review.googlesource.com/c/skia/+/878977
# * missing include https://skia-review.googlesource.com/c/skia/+/878977
#"$here/patch_skia_m124.sh"
"$here/patch_skia_m134.sh"

cd "$here/../../contrib/skia"
export GIT_SYNC_DEPS_SKIP_EMSDK="true"
python3 tools/git-sync-deps

mkdir -p out/Shared
cat > out/Shared/args.gn <<- EOF
    cc = "clang"
    cxx = "clang++"
    extra_cflags = ["-Wno-psabi"]
    is_official_build=true
    is_debug=false
    is_trivial_abi=false
    is_component_build=true
    skia_use_gl=true
    sanitize=""
    skia_build_for_debugger=false
    skia_build_fuzzers=false
    skia_build_rust_targets=false
    skia_compile_modules=false
    skia_compile_sksl_tests=false
    skia_disable_tracing=true
    skia_disable_vma_stl_shared_mutex=false
    skia_enable_android_utils=false
    skia_enable_api_available_macro=true
    skia_enable_bentleyottmann=true
    skia_enable_direct3d_debug_layer=false
    skia_enable_discrete_gpu=true
    skia_enable_fontmgr_FontConfigInterface=true
    skia_enable_fontmgr_android=false
    skia_enable_fontmgr_custom_directory=true
    skia_enable_fontmgr_custom_embedded=true
    skia_enable_fontmgr_custom_empty=true
    skia_enable_fontmgr_empty=false
    skia_enable_fontmgr_fontconfig=true
    skia_enable_fontmgr_fuchsia=false
    skia_enable_fontmgr_win=false
    skia_enable_fontmgr_win_gdi=false
    skia_enable_ganesh=true
    skia_enable_gpu=true
    skia_enable_gpu_debug_layers=false
    skia_enable_graphite=false
    skia_enable_metal_debug_info=false
    skia_enable_optimize_size=false
    skia_enable_pdf=true
    skia_enable_precompile=true
    skia_enable_skottie=false
    skia_enable_skparagraph=true
    skia_enable_skshaper=true
    skia_enable_skshaper_tests=false
    skia_enable_sksl_tracing=false
    skia_enable_skunicode=true
    skia_enable_spirv_validation=false
    skia_enable_svg=true
    skia_enable_tools=false
    skia_enable_vello_shaders=false
    skia_enable_vulkan_debug_layers=false
    skia_enable_winuwp=false
    skia_generate_workarounds=false
    skia_gl_standard=""
    skia_include_multiframe_procs=false
    skia_lex=false
    skia_pdf_subset_harfbuzz=true
    skia_print_native_shaders=false
    skia_print_sksl_shaders=false
    skia_use_angle=false
    skia_use_client_icu=false
    skia_use_dawn=false
    skia_use_direct3d=false
    skia_use_dng_sdk=true
    skia_use_egl=false
    skia_use_epoxy_egl=false
    skia_use_expat=true
    skia_use_ffmpeg=false
    skia_use_fixed_gamma_text=false
    skia_use_fontations=false
    skia_use_fontconfig=true
    skia_use_fonthost_mac=false
    skia_use_freetype=true
    skia_use_freetype_svg=true
    skia_use_freetype_woff2=false
    skia_use_freetype_zlib=true
    skia_use_freetype_zlib_bundled=false
    skia_use_gl=true
    skia_use_harfbuzz=true
    skia_use_icu=true
    skia_use_icu4x=true
    skia_use_jpeg_gainmaps=false
    skia_use_libavif=false
    skia_use_libfuzzer_defaults=true
    skia_use_libgrapheme=false
    skia_use_libheif=false
    skia_use_libjpeg_turbo_decode=true
    skia_use_libjpeg_turbo_encode=true
    skia_use_libjxl_decode=false
    skia_use_libpng_decode=false
    skia_use_libpng_encode=true
    skia_use_libwebp_decode=true
    skia_use_libwebp_encode=true
    skia_use_lua=false
    skia_use_metal=false
    skia_use_ndk_images=false
    skia_use_no_jpeg_encode=false
    skia_use_no_png_encode=false
    skia_use_no_webp_encode=false
    skia_use_perfetto=false
    skia_use_piex=true
    skia_use_runtime_icu=false
    skia_use_safe_libcxx=false
    skia_use_sfml=false
    skia_use_system_expat=true
    skia_use_system_freetype2=true
    skia_use_system_harfbuzz=false
    skia_use_system_icu=true
    skia_use_system_libjpeg_turbo=true
    skia_use_system_libpng=true
    skia_use_system_libwebp=true
    skia_use_system_zlib=true
    skia_use_vma=false
    skia_use_vulkan=false
    skia_use_webgl=false
    skia_use_webgpu=false
    skia_use_wuffs=true
    skia_use_x11=true
    skia_use_xps=true
    skia_use_zlib=true
    skunicode_tests_enabled=false
    werror=false
EOF
#extra_ldflags = [ "-fuse-ld=lld", "-Wl,-rpath,${clangdir}/lib/x86_64-unknown-linux-gnu" ]

./bin/fetch-ninja
#bin/gn args out/Shared --list
#./bin/gn args out/Shared --list --short
./bin/gn gen out/Shared

./third_party/ninja/ninja -v -d keeprsp -C out/Shared
#"$here/patch_skia_m124.sh"
#./third_party/ninja/ninja -v -d keeprsp -C out/Shared
