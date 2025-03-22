# generated using cmakelists.dhall
cmake_minimum_required(VERSION 3.24)
project(imgui_skia_exe)
set(CMAKE_LINKER clang++)
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_BUILD_TYPE Release)

add_compile_definitions(NDEBUG
SK_CODEC_DECODES_BMP
SK_CODEC_DECODES_WBMP
SK_R32_SHIFT=16
SK_ENABLE_PRECOMPILE
SK_GANESH
SK_DISABLE_TRACING
SK_GAMMA_APPLY_TO_A8
SK_ENABLE_AVX512_OPTS
SKIA_IMPLEMENTATION=1
SK_FONTMGR_FCI_AVAILABLE
SK_TYPEFACE_FACTORY_FREETYPE
SK_FONTMGR_FREETYPE_DIRECTORY_AVAILABLE
SK_FONTMGR_FREETYPE_EMBEDDED_AVAILABLE
SK_FONTMGR_FREETYPE_EMPTY_AVAILABLE
SK_FONTMGR_FONTCONFIG_AVAILABLE
SK_GL
SK_CODEC_ENCODES_JPEG
SK_SUPPORT_PDF
SK_CODEC_DECODES_JPEG
SK_CODEC_ENCODES_PNG
SK_CODEC_ENCODES_PNG_WITH_LIBPNG
SK_CODEC_ENCODES_WEBP
SK_CODEC_DECODES_RAW
SK_CODEC_DECODES_WEBP
SK_HAS_WUFFS_LIBRARY
SK_CODEC_DECODES_GIF
SK_XML
)
include_directories(/home/deck/repo/imzero_client_cpp/skia/./src/marshalling
/home/deck/repo/imzero_client_cpp/skia/./src/arena/simple
/home/deck/repo/imzero_client_cpp/skia/./src/binding
/home/deck/repo/imzero_client_cpp/skia/./src
/home/deck/repo/imzero_client_cpp/skia/./src/widgets/imgui_implot
/home/deck/repo/imzero_client_cpp/skia/./src/widgets/imgui_toggle
/home/deck/repo/imzero_client_cpp/skia/./src/widgets/imgui_knobs
/home/deck/repo/imzero_client_cpp/skia/./src/widgets/imgui_coolbar
/home/deck/repo/imzero_client_cpp/skia/./src/widgets/imgui_flamegraph
/home/deck/repo/imzero_client_cpp/skia/./imcolortextedit
/home/deck/repo/imzero_client_cpp/skia/./src/widgets
/home/deck/repo/imzero_client_cpp/imgui_skia/./../../contrib/flatbuffers/include
/home/deck/repo/imzero_client_cpp/imgui_skia/./../imgui_skia/imgui_w_hooks_1.91.9_wip
/home/deck/repo/imzero_client_cpp/imgui_skia/./../imgui_skia/imgui_skia_impl
/home/deck/repo/imzero_client_cpp/imgui_skia/./../../contrib/sdl/include
/home/deck/repo/imzero_client_cpp/imgui_skia/./../../contrib/skia
/home/deck/repo/imzero_client_cpp/imgui_skia/./../imgui_skia/imgui_skia_driver_impl
/home/deck/repo/imzero_client_cpp/imgui_skia/./../../contrib/tracy/public
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
-Wno-attributes
-ffp-contract=off
-fPIC
-fvisibility=hidden
-fstrict-aliasing
-O3
-fdata-sections
-ffunction-sections
-Wno-psabi
-fvisibility-inlines-hidden
-fno-exceptions
-fno-rtti
)
link_libraries(-DNDEBUG
# systemFlags
-Wl,--enable-new-dtags
-Wl,-rpath,'$ORIGIN/../lib' -Wl,-z,origin
# sdl3Shared
-L/home/deck/repo/imzero_client_cpp/imgui_skia/./../../contrib/sdl/build
-lSDL3
# skia
-L/home/deck/repo/imzero_client_cpp/imgui_skia/./../../contrib/skia/out/Static
-lbentleyottmann
-lcompression_utils_portable
-ldng_sdk
-lexpat
-lharfbuzz
-licu
-licu4x_rust
-ljpeg
-lpathkit
-lpiex
-lpng
-lskcms
-lskia
-lskparagraph
-lskresources
-lskshaper
-lskunicode_core
-lskunicode_icu
-lskunicode_icu4x
-lsvg
-lwebp
-lwebp_sse41
-lwuffs
-lzlib
-ldl
-lpthread
-lfreetype
-lfontconfig
-lX11
-lGLU
-lGL
)
if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND CMAKE_SYSTEM_NAME STREQUAL "Linux")
add_link_options(-Wl,--start-group)
endif()
# begin marshalling
add_library(marshalling STATIC /home/deck/repo/imzero_client_cpp/skia/./src/marshalling/receive.cpp
/home/deck/repo/imzero_client_cpp/skia/./src/marshalling/send.cpp
/home/deck/repo/imzero_client_cpp/skia/./src/marshalling/helper.cpp)
# end marshalling

# begin arena
add_library(arena STATIC /home/deck/repo/imzero_client_cpp/skia/./src/arena/simple/simple.cpp)
# end arena

# empty target binding

# begin render
add_library(render STATIC /home/deck/repo/imzero_client_cpp/skia/./src/render.cpp)
# end render

# begin imguiImplot
add_library(imguiImplot STATIC /home/deck/repo/imzero_client_cpp/skia/./src/widgets/imgui_implot/implot.cpp
/home/deck/repo/imzero_client_cpp/skia/./src/widgets/imgui_implot/implot_demo.cpp
/home/deck/repo/imzero_client_cpp/skia/./src/widgets/imgui_implot/implot_items.cpp)
# end imguiImplot

# begin imguiToggle
add_library(imguiToggle STATIC /home/deck/repo/imzero_client_cpp/skia/./src/widgets/imgui_toggle/imgui_toggle.cpp
/home/deck/repo/imzero_client_cpp/skia/./src/widgets/imgui_toggle/imgui_toggle_palette.cpp
/home/deck/repo/imzero_client_cpp/skia/./src/widgets/imgui_toggle/imgui_toggle_presets.cpp
/home/deck/repo/imzero_client_cpp/skia/./src/widgets/imgui_toggle/imgui_toggle_renderer.cpp)
target_compile_options(imguiToggle PUBLIC -Wno-unused-parameter
-Wno-unused-variable)
# end imguiToggle

# begin imguiKnobs
add_library(imguiKnobs STATIC /home/deck/repo/imzero_client_cpp/skia/./src/widgets/imgui_knobs/imgui-knobs.cpp)
# end imguiKnobs

# begin imguiCoolbar
add_library(imguiCoolbar STATIC /home/deck/repo/imzero_client_cpp/skia/./src/widgets/imgui_coolbar/ImCoolbar.cpp)
# end imguiCoolbar

# begin imguiFlamegraph
add_library(imguiFlamegraph STATIC /home/deck/repo/imzero_client_cpp/skia/./src/widgets/imgui_flamegraph/imgui_widget_flamegraph.cpp)
# end imguiFlamegraph

# begin imguiTextedit
add_library(imguiTextedit STATIC /home/deck/repo/imzero_client_cpp/skia/./imcolortextedit/TextEditor.cpp
/home/deck/repo/imzero_client_cpp/skia/./imcolortextedit/LanguageDefinitions.cpp)
target_compile_options(imguiTextedit PUBLIC -Wno-unused-variable)
# end imguiTextedit

# begin widgets
add_library(widgets STATIC /home/deck/repo/imzero_client_cpp/skia/./src/widgets/common.cpp
/home/deck/repo/imzero_client_cpp/skia/./src/widgets/piemenu.cpp
/home/deck/repo/imzero_client_cpp/skia/./src/widgets/splitter.cpp)
target_compile_options(widgets PUBLIC -Wno-unused-parameter
-Wno-unused-variable)
# end widgets

# begin imzeroClientSkiaSdl3Impl
add_executable(imzeroClientSkiaSdl3Impl  /home/deck/repo/imzero_client_cpp/skia/./imzero_client_skia_sdl3_impl/main.cpp
/home/deck/repo/imzero_client_cpp/skia/./imzero_client_skia_sdl3_impl/imzero_client_skia_sdl3_app.cpp
/home/deck/repo/imzero_client_cpp/skia/./imzero_client_skia_sdl3_impl/imzero_client_skia_sdl3_cli_options.cpp
/home/deck/repo/imzero_client_cpp/skia/./imzero_client_skia_sdl3_impl/bmp_encoder.cpp)
target_compile_options(imzeroClientSkiaSdl3Impl PUBLIC -Wno-unused-parameter)
target_include_directories(imzeroClientSkiaSdl3Impl PUBLIC /home/deck/repo/imzero_client_cpp/skia/./imzero_client_skia_sdl3_impl
/home/deck/repo/imzero_client_cpp/skia/./imzero_client_skia_sdl3_impl
/home/deck/repo/imzero_client_cpp/skia/./imzero_client_skia_sdl3_impl/../../../contrib/qoi)
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
