# generated using cmakelists.dhall
cmake_minimum_required(VERSION 3.24)
project(imgui_skia_exe)
set(CMAKE_LINKER clang++)
set(CMAKE_CXX_COMPILER clang++)

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
TRACY_ENABLE
)
include_directories("${CMAKE_CURRENT_LIST_DIR}/./contrib/flatbuffers/include"
"${CMAKE_CURRENT_LIST_DIR}/./imgui_w_hooks_1.91.9_wip"
"${CMAKE_CURRENT_LIST_DIR}/./imgui_skia_impl"
"${CMAKE_CURRENT_LIST_DIR}/./contrib/skia"
"${CMAKE_CURRENT_LIST_DIR}/./contrib/tracy/public"
)
add_compile_options(-std=c++20
-g
-gdwarf-4
-Wall
-Wformat
-Wextra
-Wno-unused-parameter
-O1
-DIMGUI_SKIA_DEBUG_BUILD
-fsanitize=address
# sdl3Shared
-I/home/deck/repo/imzero_client_cpp/skia_minimal/./contrib/sdl/include
# skiaShared
-ffp-contract=off
# skiaShared
-fstrict-aliasing
# skiaShared
-fPIC
# skiaShared
-fvisibility=hidden
# skiaShared
-fdata-sections
# skiaShared
-ffunction-sections
# skiaShared
-fvisibility-inlines-hidden
# skiaShared
-fno-exceptions
# skiaShared
-fno-rtti
)
link_libraries(-Wl,-rpath,'$ORIGIN/../lib' -Wl,-z,origin
-fsanitize=address
-fsanitize=undefined
# sdl3Shared
-L/home/deck/repo/imzero_client_cpp/skia_minimal/./contrib/sdl/build
# sdl3Shared
-Wl,--enable-new-dtags
# sdl3Shared
-lSDL3
# skiaShared
-ldl
# skiaShared
-lpthread
# skiaShared
-lfreetype
# skiaShared
-lz
# skiaShared
-lglfw
# skiaShared
-lfontconfig
# skiaShared
-lwebpmux
# skiaShared
-lwebpdemux
# skiaShared
-lX11
# skiaShared
-lGLU
# skiaShared
-lGL
# skiaShared
-L/home/deck/repo/imzero_client_cpp/skia_minimal/./contrib/skia/out/Shared
# skiaShared
-lskparagraph
# skiaShared
-lskia
# skiaShared
-lskunicode
# skiaShared
-lbentleyottmann
# skiaShared
-lskshaper
# mainSkiaSdl3Minimal
-ldl
# mainSkiaSdl3Minimal
-lpthread
# mainSkiaSdl3Minimal
-lfreetype
# mainSkiaSdl3Minimal
-lz
# mainSkiaSdl3Minimal
-lfontconfig
# mainSkiaSdl3Minimal
-lwebpmux
# mainSkiaSdl3Minimal
-lwebpdemux
# mainSkiaSdl3Minimal
-lX11
# mainSkiaSdl3Minimal
-lGLU
# mainSkiaSdl3Minimal
-lGL
)

# flatbuffers

# imguiWithHooks1919Wip
add_library(imguiWithHooks1919Wip OBJECT "${CMAKE_CURRENT_LIST_DIR}/./imgui_w_hooks_1.91.9_wip/imgui.cpp"
"${CMAKE_CURRENT_LIST_DIR}/./imgui_w_hooks_1.91.9_wip/imgui_demo.cpp"
"${CMAKE_CURRENT_LIST_DIR}/./imgui_w_hooks_1.91.9_wip/imgui_draw.cpp"
"${CMAKE_CURRENT_LIST_DIR}/./imgui_w_hooks_1.91.9_wip/imgui_tables.cpp"
"${CMAKE_CURRENT_LIST_DIR}/./imgui_w_hooks_1.91.9_wip/imgui_widgets.cpp"
"${CMAKE_CURRENT_LIST_DIR}/./imgui_w_hooks_1.91.9_wip/imgui_impl_sdl3.cpp"
"${CMAKE_CURRENT_LIST_DIR}/./imgui_w_hooks_1.91.9_wip/imgui_impl_sdl3.cpp")

# imguiSkiaImpl
add_library(imguiSkiaImpl OBJECT "${CMAKE_CURRENT_LIST_DIR}/./imgui_skia_impl/imgui_skia_app_sdl3.cpp"
"${CMAKE_CURRENT_LIST_DIR}/./imgui_skia_impl/imgui_skia_cli_options.cpp"
"${CMAKE_CURRENT_LIST_DIR}/./imgui_skia_impl/imgui_skia_extensions.cpp"
"${CMAKE_CURRENT_LIST_DIR}/./imgui_skia_impl/imgui_skia_hooks_impl.cpp"
"${CMAKE_CURRENT_LIST_DIR}/./imgui_skia_impl/imgui_skia_imzero_cmd_render.cpp"
"${CMAKE_CURRENT_LIST_DIR}/./imgui_skia_impl/imgui_skia_paragraph.cpp"
"${CMAKE_CURRENT_LIST_DIR}/./imgui_skia_impl/imgui_skia_setup_ui.cpp"
"${CMAKE_CURRENT_LIST_DIR}/./imgui_skia_impl/imgui_skia_tracy_support.cpp")

# sdl3Shared

# skiaShared

# mainSkiaSdl3Minimal
add_library(mainSkiaSdl3Minimal OBJECT "${CMAKE_CURRENT_LIST_DIR}/./main/sdl3/main.cpp")
target_compile_options(mainSkiaSdl3Minimal PUBLIC -Wno-unused-parameter)

# tracyEnabled
add_library(tracyEnabled OBJECT "${CMAKE_CURRENT_LIST_DIR}/./contrib/tracy/public/TracyClient.cpp")
add_executable(imgui_skia_exe $<TARGET_OBJECTS:imguiWithHooks1919Wip>
$<TARGET_OBJECTS:imguiSkiaImpl>
$<TARGET_OBJECTS:mainSkiaSdl3Minimal>
$<TARGET_OBJECTS:tracyEnabled>)

