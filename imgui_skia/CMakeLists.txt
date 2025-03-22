# generated using cmakelists.dhall
cmake_minimum_required(VERSION 3.24)
project(imgui_skia)
set(CMAKE_LINKER clang++)
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_BUILD_TYPE Release)

add_compile_definitions(SK_RELEASE
SK_GAMMA_APPLY_TO_A8
SK_ALLOW_STATIC_GLOBAL_INITIALIZERS=1
SK_TYPEFACE_FACTORY_FREETYPE
SK_FONTMGR_FREETYPE_EMBEDDED_AVAILABLE
SK_FONTMGR_FONTCONFIG_AVAILABLE
SK_FONTMGR_FREETYPE_DIRECTORY_AVAILABLE
SK_FONTMGR_FREETYPE_EMPTY_AVAILABLE
SK_GL
SK_SUPPORT_PDF
SK_CODEC_DECODES_JPEG
SK_CODEC_DECODES_JPEG_GAINMAPS
SK_XML
SK_CODEC_DECODES_PNG
SK_CODEC_DECODES_RAW
SK_CODEC_DECODES_WEBP
SK_DEFAULT_TYPEFACE_IS_EMPTY
SK_DISABLE_LEGACY_DEFAULT_TYPEFACE
SK_R32_SHIFT=16
SK_ENABLE_PRECOMPILE
SK_GANESH
SK_ENABLE_PARAGRAPH
SK_UNICODE_AVAILABLE
SK_UNICODE_ICU_IMPLEMENTATION
SK_SHAPER_PRIMITIVE_AVAILABLE
SK_SHAPER_HARFBUZZ_AVAILABLE
SK_SHAPER_UNICODE_AVAILABLE
SK_ENABLE_SVG
SK_BUILD_FOR_UNIX
)
include_directories(/home/deck/repo/imzero_client_cpp/imgui_skia/../imgui_skia/./../../contrib/flatbuffers/include
/home/deck/repo/imzero_client_cpp/imgui_skia/../imgui_skia/./imgui_w_hooks_1.91.9_wip
/home/deck/repo/imzero_client_cpp/imgui_skia/../imgui_skia/./imgui_skia_impl
/home/deck/repo/imzero_client_cpp/imgui_skia/../imgui_skia/./../../contrib/sdl/include
/home/deck/repo/imzero_client_cpp/imgui_skia/../imgui_skia/./../../contrib/skia
/home/deck/repo/imzero_client_cpp/imgui_skia/../imgui_skia/./imgui_skia_driver_impl
/home/deck/repo/imzero_client_cpp/imgui_skia/../imgui_skia/./../../contrib/tracy/public
)
add_compile_options(-std=c++20
-Wall
-Wformat
-Wextra
-Wno-unused-parameter
-fno-omit-frame-pointer
-O3
# systemFlags
-fPIC
# skia
-ffp-contract=off
-fstrict-aliasing
-fvisibility=hidden
-fdata-sections
-ffunction-sections
-fvisibility-inlines-hidden
-fno-exceptions
-fno-rtti
)
link_libraries(-DNDEBUG
# systemFlags
-Wl,--enable-new-dtags
-Wl,-rpath,'$ORIGIN/../lib' -Wl,-z,origin
# sdl3Shared
-L/home/deck/repo/imzero_client_cpp/imgui_skia/../imgui_skia/./../../contrib/sdl/build
-lSDL3
# skia
-L/home/deck/repo/imzero_client_cpp/imgui_skia/../imgui_skia/./../../contrib/skia/out/Static
-ldl
-lpthread
-lfreetype
-lz
-lfontconfig
-lwebpmux
-lwebpdemux
-lX11
-lGLU
-lGL
-L/home/deck/repo/imzero_client_cpp/imgui_skia/../imgui_skia/./../../contrib/skia/out/Shared
-lskparagraph
-lskia
-lbentleyottmann
-lskshaper
-lskunicode_core
-lskunicode_icu
-lskunicode_icu4x
)

# empty target systemFlags

# empty target flatbuffers

# begin imguiWithHooks1919Wip
add_library(imguiWithHooks1919Wip STATIC /home/deck/repo/imzero_client_cpp/imgui_skia/../imgui_skia/./imgui_w_hooks_1.91.9_wip/imgui.cpp
/home/deck/repo/imzero_client_cpp/imgui_skia/../imgui_skia/./imgui_w_hooks_1.91.9_wip/imgui_demo.cpp
/home/deck/repo/imzero_client_cpp/imgui_skia/../imgui_skia/./imgui_w_hooks_1.91.9_wip/imgui_draw.cpp
/home/deck/repo/imzero_client_cpp/imgui_skia/../imgui_skia/./imgui_w_hooks_1.91.9_wip/imgui_tables.cpp
/home/deck/repo/imzero_client_cpp/imgui_skia/../imgui_skia/./imgui_w_hooks_1.91.9_wip/imgui_widgets.cpp
/home/deck/repo/imzero_client_cpp/imgui_skia/../imgui_skia/./imgui_w_hooks_1.91.9_wip/imgui_impl_sdl3.cpp
/home/deck/repo/imzero_client_cpp/imgui_skia/../imgui_skia/./imgui_w_hooks_1.91.9_wip/imgui_impl_sdl3.cpp)
# end imguiWithHooks1919Wip

# begin imguiSkiaImpl
add_library(imguiSkiaImpl STATIC /home/deck/repo/imzero_client_cpp/imgui_skia/../imgui_skia/./imgui_skia_impl/imgui_skia_extensions.cpp
/home/deck/repo/imzero_client_cpp/imgui_skia/../imgui_skia/./imgui_skia_impl/imgui_skia_hooks_impl.cpp
/home/deck/repo/imzero_client_cpp/imgui_skia/../imgui_skia/./imgui_skia_impl/imgui_skia_imzero_cmd_render.cpp
/home/deck/repo/imzero_client_cpp/imgui_skia/../imgui_skia/./imgui_skia_impl/imgui_skia_paragraph.cpp
/home/deck/repo/imzero_client_cpp/imgui_skia/../imgui_skia/./imgui_skia_impl/imgui_skia_setup_ui.cpp
/home/deck/repo/imzero_client_cpp/imgui_skia/../imgui_skia/./imgui_skia_impl/imgui_skia_tracy_support.cpp)
# end imguiSkiaImpl

# empty target sdl3Shared

# empty target skia

# begin imguiSkiaDriverImpl
add_library(imguiSkiaDriverImpl STATIC /home/deck/repo/imzero_client_cpp/imgui_skia/../imgui_skia/./imgui_skia_driver_impl/imgui_skia_app_sdl3.cpp
/home/deck/repo/imzero_client_cpp/imgui_skia/../imgui_skia/./imgui_skia_driver_impl/imgui_skia_cli_options.cpp)
# end imguiSkiaDriverImpl

# begin tracyDisabled
add_library(tracyDisabled STATIC /home/deck/repo/imzero_client_cpp/imgui_skia/../imgui_skia/./../../contrib/tracy/public/TracyClient.cpp)
# end tracyDisabled
add_library(imgui_skia STATIC $<TARGET_OBJECTS:imguiWithHooks1919Wip>
$<TARGET_OBJECTS:imguiSkiaImpl>
$<TARGET_OBJECTS:imguiSkiaDriverImpl>
$<TARGET_OBJECTS:tracyDisabled>)
