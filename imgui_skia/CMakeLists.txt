# generated using cmakelists.dhall
cmake_minimum_required(VERSION 3.24)
project(imgui_skia)
set(CMAKE_LINKER clang++)
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_BUILD_TYPE Release)

add_compile_definitions(_CRT_SECURE_NO_WARNINGS
_HAS_EXCEPTIONS=0
WIN32_LEAN_AND_MEAN
NOMINMAX
NDEBUG
SK_CODEC_DECODES_BMP
SK_CODEC_DECODES_WBMP
SK_ENABLE_PRECOMPILE
SK_GANESH
SK_DISABLE_TRACING
SK_GAMMA_APPLY_TO_A8
SK_ENABLE_AVX512_OPTS
SKIA_IMPLEMENTATION=1
SK_FONTMGR_FREETYPE_DIRECTORY_AVAILABLE
SK_TYPEFACE_FACTORY_FREETYPE
SK_FONTMGR_FREETYPE_EMBEDDED_AVAILABLE
SK_FONTMGR_FREETYPE_EMPTY_AVAILABLE
SK_TYPEFACE_FACTORY_DIRECTWRITE
SK_FONTMGR_DIRECTWRITE_AVAILABLE
SK_FONTMGR_GDI_AVAILABLE
SK_GL
SK_CODEC_ENCODES_JPEG
SK_SUPPORT_PDF
SK_CODEC_DECODES_JPEG
SK_CODEC_ENCODES_PNG
SK_CODEC_ENCODES_PNG_WITH_LIBPNG
SK_CODEC_ENCODES_WEBP
SK_SUPPORT_XPS
SK_CODEC_DECODES_ICO
SK_CODEC_DECODES_PNG
SK_CODEC_DECODES_PNG_WITH_LIBPNG
SK_CODEC_DECODES_RAW
SK_CODEC_DECODES_WEBP
SK_HAS_WUFFS_LIBRARY
SK_CODEC_DECODES_GIF
SK_XML
)
include_directories(C:/Users/User/repo/imzero_client_cpp/imgui_skia/../imgui_skia/./../../contrib/flatbuffers/include
C:/Users/User/repo/imzero_client_cpp/imgui_skia/../imgui_skia/./imgui_w_hooks_1.91.9_wip
C:/Users/User/repo/imzero_client_cpp/imgui_skia/../imgui_skia/./imgui_skia_impl
C:/Users/User/repo/imzero_client_cpp/imgui_skia/../imgui_skia/./../../contrib/sdl/include
C:/Users/User/repo/imzero_client_cpp/imgui_skia/../imgui_skia/./../../contrib/skia
C:/Users/User/repo/imzero_client_cpp/imgui_skia/../imgui_skia/./imgui_skia_driver_impl
C:/Users/User/repo/imzero_client_cpp/imgui_skia/../imgui_skia/./../../contrib/tracy/public
)
add_compile_options(-std=c++20
-Wall
-Wformat
-Wextra
-Wno-unused-parameter
-fno-omit-frame-pointer
-O3
# skiaShared
-funwind-tables
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
# sdl3Shared
-LC:/Users/User/repo/imzero_client_cpp/imgui_skia/../imgui_skia/./../../contrib/sdl/build
-lSDL3-static
-lwinmm
-limm32
-lole32
-loleaut32
-lversion
-luuid
-ladvapi32
-lsetupapi
-lshell32
-ldinput8
# skiaShared
-LC:/Users/User/repo/imzero_client_cpp/imgui_skia/../imgui_skia/./../../contrib/skia/out/Static
-lskparagraph
-lskia
-lskunicode_core
-lskunicode_icu
-lbentleyottmann
-lskshaper
-lOpenGL32
)

# empty target systemFlags

# empty target flatbuffers

# begin imguiWithHooks1919Wip
add_library(imguiWithHooks1919Wip STATIC C:/Users/User/repo/imzero_client_cpp/imgui_skia/../imgui_skia/./imgui_w_hooks_1.91.9_wip/imgui.cpp
C:/Users/User/repo/imzero_client_cpp/imgui_skia/../imgui_skia/./imgui_w_hooks_1.91.9_wip/imgui_demo.cpp
C:/Users/User/repo/imzero_client_cpp/imgui_skia/../imgui_skia/./imgui_w_hooks_1.91.9_wip/imgui_draw.cpp
C:/Users/User/repo/imzero_client_cpp/imgui_skia/../imgui_skia/./imgui_w_hooks_1.91.9_wip/imgui_tables.cpp
C:/Users/User/repo/imzero_client_cpp/imgui_skia/../imgui_skia/./imgui_w_hooks_1.91.9_wip/imgui_widgets.cpp
C:/Users/User/repo/imzero_client_cpp/imgui_skia/../imgui_skia/./imgui_w_hooks_1.91.9_wip/imgui_impl_sdl3.cpp
C:/Users/User/repo/imzero_client_cpp/imgui_skia/../imgui_skia/./imgui_w_hooks_1.91.9_wip/imgui_impl_sdl3.cpp)
# end imguiWithHooks1919Wip

# begin imguiSkiaImpl
add_library(imguiSkiaImpl STATIC C:/Users/User/repo/imzero_client_cpp/imgui_skia/../imgui_skia/./imgui_skia_impl/imgui_skia_extensions.cpp
C:/Users/User/repo/imzero_client_cpp/imgui_skia/../imgui_skia/./imgui_skia_impl/imgui_skia_hooks_impl.cpp
C:/Users/User/repo/imzero_client_cpp/imgui_skia/../imgui_skia/./imgui_skia_impl/imgui_skia_imzero_cmd_render.cpp
C:/Users/User/repo/imzero_client_cpp/imgui_skia/../imgui_skia/./imgui_skia_impl/imgui_skia_paragraph.cpp
C:/Users/User/repo/imzero_client_cpp/imgui_skia/../imgui_skia/./imgui_skia_impl/imgui_skia_setup_ui.cpp
C:/Users/User/repo/imzero_client_cpp/imgui_skia/../imgui_skia/./imgui_skia_impl/imgui_skia_tracy_support.cpp)
# end imguiSkiaImpl

# empty target sdl3Shared

# empty target skiaShared

# begin imguiSkiaDriverImpl
add_library(imguiSkiaDriverImpl STATIC C:/Users/User/repo/imzero_client_cpp/imgui_skia/../imgui_skia/./imgui_skia_driver_impl/imgui_skia_app_sdl3.cpp
C:/Users/User/repo/imzero_client_cpp/imgui_skia/../imgui_skia/./imgui_skia_driver_impl/imgui_skia_cli_options.cpp)
# end imguiSkiaDriverImpl

# begin tracyDisabled
add_library(tracyDisabled STATIC C:/Users/User/repo/imzero_client_cpp/imgui_skia/../imgui_skia/./../../contrib/tracy/public/TracyClient.cpp)
# end tracyDisabled
add_library(imgui_skia STATIC $<TARGET_OBJECTS:imguiWithHooks1919Wip>
$<TARGET_OBJECTS:imguiSkiaImpl>
$<TARGET_OBJECTS:imguiSkiaDriverImpl>
$<TARGET_OBJECTS:tracyDisabled>)
