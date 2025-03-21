# generated using cmakelists.dhall
cmake_minimum_required(VERSION 3.24)
project(imgui_skia_exe)
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
include_directories(/data/repo/imzero_client_cpp/skia/./src/marshalling
/data/repo/imzero_client_cpp/skia/./src/arena/simple
/data/repo/imzero_client_cpp/skia/./src/binding
/data/repo/imzero_client_cpp/skia/./src
/data/repo/imzero_client_cpp/skia/./src/widgets/imgui_implot
/data/repo/imzero_client_cpp/skia/./src/widgets/imgui_toggle
/data/repo/imzero_client_cpp/skia/./src/widgets/imgui_knobs
/data/repo/imzero_client_cpp/skia/./src/widgets/imgui_coolbar
/data/repo/imzero_client_cpp/skia/./src/widgets/imgui_flamegraph
/data/repo/imzero_client_cpp/skia/./imcolortextedit
/data/repo/imzero_client_cpp/skia/./src/widgets
/data/repo/imzero_client_cpp/imgui_skia/./../../contrib/flatbuffers/include
/data/repo/imzero_client_cpp/imgui_skia/./../imgui_skia/imgui_w_hooks_1.91.9_wip
/data/repo/imzero_client_cpp/imgui_skia/./../imgui_skia/imgui_skia_impl
/data/repo/imzero_client_cpp/imgui_skia/./../../contrib/sdl/include
/data/repo/imzero_client_cpp/imgui_skia/./../../contrib/skia
/data/repo/imzero_client_cpp/imgui_skia/./../imgui_skia/imgui_skia_driver_impl
/data/repo/imzero_client_cpp/imgui_skia/./../../contrib/tracy/public
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
# skiaShared
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
-L/data/repo/imzero_client_cpp/imgui_skia/./../../contrib/sdl/build
-lSDL3
# skiaShared
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
-L/data/repo/imzero_client_cpp/imgui_skia/./../../contrib/skia/out/Shared
-lskparagraph
-lskia
-lbentleyottmann
-lskshaper
-lskunicode_core
-lskunicode_icu
)
if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND CMAKE_SYSTEM_NAME STREQUAL "Linux")
add_link_options(-Wl,--start-group)
endif()
# begin marshalling
add_library(marshalling STATIC /data/repo/imzero_client_cpp/skia/./src/marshalling/receive.cpp
/data/repo/imzero_client_cpp/skia/./src/marshalling/send.cpp
/data/repo/imzero_client_cpp/skia/./src/marshalling/helper.cpp)
# end marshalling

# begin arena
add_library(arena STATIC /data/repo/imzero_client_cpp/skia/./src/arena/simple/simple.cpp)
# end arena

# empty target binding

# begin render
add_library(render STATIC /data/repo/imzero_client_cpp/skia/./src/render.cpp)
# end render

# begin imguiImplot
add_library(imguiImplot STATIC /data/repo/imzero_client_cpp/skia/./src/widgets/imgui_implot/implot.cpp
/data/repo/imzero_client_cpp/skia/./src/widgets/imgui_implot/implot_demo.cpp
/data/repo/imzero_client_cpp/skia/./src/widgets/imgui_implot/implot_items.cpp)
# end imguiImplot

# begin imguiToggle
add_library(imguiToggle STATIC /data/repo/imzero_client_cpp/skia/./src/widgets/imgui_toggle/imgui_toggle.cpp
/data/repo/imzero_client_cpp/skia/./src/widgets/imgui_toggle/imgui_toggle_palette.cpp
/data/repo/imzero_client_cpp/skia/./src/widgets/imgui_toggle/imgui_toggle_presets.cpp
/data/repo/imzero_client_cpp/skia/./src/widgets/imgui_toggle/imgui_toggle_renderer.cpp)
target_compile_options(imguiToggle PUBLIC -Wno-unused-parameter
-Wno-unused-variable)
# end imguiToggle

# begin imguiKnobs
add_library(imguiKnobs STATIC /data/repo/imzero_client_cpp/skia/./src/widgets/imgui_knobs/imgui-knobs.cpp)
# end imguiKnobs

# begin imguiCoolbar
add_library(imguiCoolbar STATIC /data/repo/imzero_client_cpp/skia/./src/widgets/imgui_coolbar/ImCoolbar.cpp)
# end imguiCoolbar

# begin imguiFlamegraph
add_library(imguiFlamegraph STATIC /data/repo/imzero_client_cpp/skia/./src/widgets/imgui_flamegraph/imgui_widget_flamegraph.cpp)
# end imguiFlamegraph

# begin imguiTextedit
add_library(imguiTextedit STATIC /data/repo/imzero_client_cpp/skia/./imcolortextedit/TextEditor.cpp
/data/repo/imzero_client_cpp/skia/./imcolortextedit/LanguageDefinitions.cpp)
target_compile_options(imguiTextedit PUBLIC -Wno-unused-variable)
# end imguiTextedit

# begin widgets
add_library(widgets STATIC /data/repo/imzero_client_cpp/skia/./src/widgets/common.cpp
/data/repo/imzero_client_cpp/skia/./src/widgets/piemenu.cpp
/data/repo/imzero_client_cpp/skia/./src/widgets/splitter.cpp)
target_compile_options(widgets PUBLIC -Wno-unused-parameter
-Wno-unused-variable)
# end widgets

# begin imzeroClientSkiaSdl3Impl
add_executable(imzeroClientSkiaSdl3Impl  /data/repo/imzero_client_cpp/skia/./imzero_client_skia_sdl3_impl/main.cpp
/data/repo/imzero_client_cpp/skia/./imzero_client_skia_sdl3_impl/imzero_client_skia_sdl3_app.cpp
/data/repo/imzero_client_cpp/skia/./imzero_client_skia_sdl3_impl/imzero_client_skia_sdl3_cli_options.cpp
/data/repo/imzero_client_cpp/skia/./imzero_client_skia_sdl3_impl/bmp_encoder.cpp)
target_compile_options(imzeroClientSkiaSdl3Impl PUBLIC -Wno-unused-parameter)
target_include_directories(imzeroClientSkiaSdl3Impl PUBLIC /data/repo/imzero_client_cpp/skia/./imzero_client_skia_sdl3_impl
/data/repo/imzero_client_cpp/skia/./imzero_client_skia_sdl3_impl
/data/repo/imzero_client_cpp/skia/./imzero_client_skia_sdl3_impl/../../../contrib/qoi)
target_link_libraries(imzeroClientSkiaSdl3Impl marshalling)
target_link_libraries(imzeroClientSkiaSdl3Impl arena)
target_link_libraries(imzeroClientSkiaSdl3Impl render)
target_link_libraries(imzeroClientSkiaSdl3Impl imguiImplot)
target_link_libraries(imzeroClientSkiaSdl3Impl imguiToggle)
target_link_libraries(imzeroClientSkiaSdl3Impl imguiKnobs)
target_link_libraries(imzeroClientSkiaSdl3Impl imguiCoolbar)
target_link_libraries(imzeroClientSkiaSdl3Impl imguiFlamegraph)
target_link_libraries(imzeroClientSkiaSdl3Impl imguiTextedit)
target_link_libraries(imzeroClientSkiaSdl3Impl widgets)
# end imzeroClientSkiaSdl3Impl
add_library(libimgui_skia STATIC IMPORTED)
set_target_properties(libimgui_skia PROPERTIES IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/../imgui_skia/build/libimgui_skia.a")
target_link_libraries(imzeroClientSkiaSdl3Impl libimgui_skia)
