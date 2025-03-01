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
)
include_directories("${CMAKE_CURRENT_LIST_DIR}/./contrib/flatbuffers"
"${CMAKE_CURRENT_LIST_DIR}/./imgui"
"${CMAKE_CURRENT_LIST_DIR}/./contrib/skia"
"${CMAKE_CURRENT_LIST_DIR}/./contrib/tracy/public"
)
add_compile_options(-std=c++20
-Wall
-Wformat
-Wextra
-Wno-unused-parameter
-fno-omit-frame-pointer
-O3
# sdl3Shared
-I/home/deck/repo/imzero_client_cpp/skia_minimal/./../../contrib/sdl/include
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
-DNDEBUG
# sdl3Shared
-L/home/deck/repo/imzero_client_cpp/skia_minimal/./../../contrib/sdl/build
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
-L/home/deck/repo/imzero_client_cpp/skia_minimal/./../../contrib/skia/out/Shared
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
# mainSkiaSdl3
-ldl
# mainSkiaSdl3
-lpthread
# mainSkiaSdl3
-lfreetype
# mainSkiaSdl3
-lz
# mainSkiaSdl3
-lfontconfig
# mainSkiaSdl3
-lwebpmux
# mainSkiaSdl3
-lwebpdemux
# mainSkiaSdl3
-lX11
# mainSkiaSdl3
-lGLU
# mainSkiaSdl3
-lGL
)

# flatbuffers

# imguiWithSkia
add_library(imguiWithSkia OBJECT "${CMAKE_CURRENT_LIST_DIR}/./imgui/imzero_hooks.cpp"
"${CMAKE_CURRENT_LIST_DIR}/./imgui/imzero_extensions.cpp"
"${CMAKE_CURRENT_LIST_DIR}/./imgui/imgui.cpp"
"${CMAKE_CURRENT_LIST_DIR}/./imgui/imgui_demo.cpp"
"${CMAKE_CURRENT_LIST_DIR}/./imgui/imgui_draw.cpp"
"${CMAKE_CURRENT_LIST_DIR}/./imgui/imgui_tables.cpp"
"${CMAKE_CURRENT_LIST_DIR}/./imgui/imgui_widgets.cpp")

# sdl3Shared

# skiaShared

# mainSkiaSdl3
add_library(mainSkiaSdl3 OBJECT "${CMAKE_CURRENT_LIST_DIR}/./main/sdl3/../../imgui/imgui_impl_sdl3.cpp"
"${CMAKE_CURRENT_LIST_DIR}/./main/sdl3/main.cpp"
"${CMAKE_CURRENT_LIST_DIR}/./main/sdl3/app.cpp"
"${CMAKE_CURRENT_LIST_DIR}/./main/sdl3/../paragraph.cpp"
"${CMAKE_CURRENT_LIST_DIR}/./main/sdl3/../cliOptions.cpp"
"${CMAKE_CURRENT_LIST_DIR}/./main/sdl3/../setupUI.cpp"
"${CMAKE_CURRENT_LIST_DIR}/./main/sdl3/../vectorCmdSkiaRenderer.cpp")
target_compile_options(mainSkiaSdl3 PUBLIC -Wno-unused-parameter)
target_include_directories(mainSkiaSdl3 PUBLIC "${CMAKE_CURRENT_LIST_DIR}/./main/sdl3"
"${CMAKE_CURRENT_LIST_DIR}/./imgui"
"${CMAKE_CURRENT_LIST_DIR}/./src/widgets/imgui_implot"
"${CMAKE_CURRENT_LIST_DIR}/./src")

# tracyDisabled
add_library(tracyDisabled OBJECT "${CMAKE_CURRENT_LIST_DIR}/./contrib/tracy/public/TracyClient.cpp")
add_executable(imgui_skia_exe $<TARGET_OBJECTS:imguiWithSkia>
$<TARGET_OBJECTS:mainSkiaSdl3>
$<TARGET_OBJECTS:tracyDisabled>)

