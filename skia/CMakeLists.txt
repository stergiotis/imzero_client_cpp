# generated using cmakelists.dhall
cmake_minimum_required(VERSION 3.24)
project(imgui_skia.exe)
set(CMAKE_LINKER clang++)
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_BUILD_TYPE Release)

add_compile_definitions(MESA_EGL_NO_X11_HEADERS
NOMINMAX
NDEBUG
SK_CODEC_DECODES_BMP
SK_CODEC_DECODES_WBMP
SKIA_DLL
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
SK_DISABLE_LEGACY_GL_MAKE_NATIVE_INTERFACE
_HAS_AUTO_PTR_ETC
)
include_directories(C:/Users/User/repo/imzero_client_cpp/skia/./src/marshalling
C:/Users/User/repo/imzero_client_cpp/skia/./src/arena/simple
C:/Users/User/repo/imzero_client_cpp/skia/./src/binding
C:/Users/User/repo/imzero_client_cpp/skia/./src
C:/Users/User/repo/imzero_client_cpp/skia/./src/widgets/imgui_implot
C:/Users/User/repo/imzero_client_cpp/skia/./src/widgets/imgui_toggle
C:/Users/User/repo/imzero_client_cpp/skia/./src/widgets/imgui_knobs
C:/Users/User/repo/imzero_client_cpp/skia/./src/widgets/imgui_coolbar
C:/Users/User/repo/imzero_client_cpp/skia/./src/widgets/imgui_flamegraph
C:/Users/User/repo/imzero_client_cpp/skia/./imcolortextedit
C:/Users/User/repo/imzero_client_cpp/skia/./src/widgets
C:/Users/User/repo/imzero_client_cpp/imgui_skia/./../../contrib/flatbuffers/include
C:/Users/User/repo/imzero_client_cpp/imgui_skia/./../imgui_skia/imgui_w_hooks_1.91.9_wip
C:/Users/User/repo/imzero_client_cpp/imgui_skia/./../imgui_skia/imgui_skia_impl
C:/Users/User/repo/imzero_client_cpp/imgui_skia/./../../contrib/sdl/include
C:/Users/User/repo/imzero_client_cpp/imgui_skia/./../../contrib/skia
C:/Users/User/repo/imzero_client_cpp/imgui_skia/./../imgui_skia/imgui_skia_driver_impl
C:/Users/User/repo/imzero_client_cpp/imgui_skia/./../../contrib/tracy/public
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
-LC:/Users/User/repo/imzero_client_cpp/imgui_skia/./../../contrib/sdl/build
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
-LC:/Users/User/repo/imzero_client_cpp/imgui_skia/./../../contrib/skia/out/Static
-lskparagraph
-lskia
-lskunicode_core
-lskunicode_icu
-lbentleyottmann
-lskshaper
-lOpenGL32
)

# begin marshalling
add_library(marshalling STATIC C:/Users/User/repo/imzero_client_cpp/skia/./src/marshalling/receive.cpp
C:/Users/User/repo/imzero_client_cpp/skia/./src/marshalling/send.cpp
C:/Users/User/repo/imzero_client_cpp/skia/./src/marshalling/helper.cpp)
# end marshalling

# begin arena
add_library(arena STATIC C:/Users/User/repo/imzero_client_cpp/skia/./src/arena/simple/simple.cpp)
# end arena

# empty target binding

# begin render
add_library(render STATIC C:/Users/User/repo/imzero_client_cpp/skia/./src/render.cpp)
# end render

# begin imguiImplot
add_library(imguiImplot STATIC C:/Users/User/repo/imzero_client_cpp/skia/./src/widgets/imgui_implot/implot.cpp
C:/Users/User/repo/imzero_client_cpp/skia/./src/widgets/imgui_implot/implot_demo.cpp
C:/Users/User/repo/imzero_client_cpp/skia/./src/widgets/imgui_implot/implot_items.cpp)
# end imguiImplot

# begin imguiToggle
add_library(imguiToggle STATIC C:/Users/User/repo/imzero_client_cpp/skia/./src/widgets/imgui_toggle/imgui_toggle.cpp
C:/Users/User/repo/imzero_client_cpp/skia/./src/widgets/imgui_toggle/imgui_toggle_palette.cpp
C:/Users/User/repo/imzero_client_cpp/skia/./src/widgets/imgui_toggle/imgui_toggle_presets.cpp
C:/Users/User/repo/imzero_client_cpp/skia/./src/widgets/imgui_toggle/imgui_toggle_renderer.cpp)
target_compile_options(imguiToggle PUBLIC -Wno-unused-parameter
-Wno-unused-variable)
# end imguiToggle

# begin imguiKnobs
add_library(imguiKnobs STATIC C:/Users/User/repo/imzero_client_cpp/skia/./src/widgets/imgui_knobs/imgui-knobs.cpp)
# end imguiKnobs

# begin imguiCoolbar
add_library(imguiCoolbar STATIC C:/Users/User/repo/imzero_client_cpp/skia/./src/widgets/imgui_coolbar/ImCoolbar.cpp)
# end imguiCoolbar

# begin imguiFlamegraph
add_library(imguiFlamegraph STATIC C:/Users/User/repo/imzero_client_cpp/skia/./src/widgets/imgui_flamegraph/imgui_widget_flamegraph.cpp)
# end imguiFlamegraph

# begin imguiTextedit
add_library(imguiTextedit STATIC C:/Users/User/repo/imzero_client_cpp/skia/./imcolortextedit/TextEditor.cpp
C:/Users/User/repo/imzero_client_cpp/skia/./imcolortextedit/LanguageDefinitions.cpp)
target_compile_options(imguiTextedit PUBLIC -Wno-unused-variable)
# end imguiTextedit

# begin widgets
add_library(widgets STATIC C:/Users/User/repo/imzero_client_cpp/skia/./src/widgets/common.cpp
C:/Users/User/repo/imzero_client_cpp/skia/./src/widgets/piemenu.cpp
C:/Users/User/repo/imzero_client_cpp/skia/./src/widgets/splitter.cpp)
target_compile_options(widgets PUBLIC -Wno-unused-parameter
-Wno-unused-variable)
# end widgets

# begin imzeroClientSkiaSdl3Impl
add_executable(imzeroClientSkiaSdl3Impl  C:/Users/User/repo/imzero_client_cpp/skia/./imzero_client_skia_sdl3_impl/main.cpp
C:/Users/User/repo/imzero_client_cpp/skia/./imzero_client_skia_sdl3_impl/imzero_client_skia_sdl3_app.cpp
C:/Users/User/repo/imzero_client_cpp/skia/./imzero_client_skia_sdl3_impl/imzero_client_skia_sdl3_cli_options.cpp
C:/Users/User/repo/imzero_client_cpp/skia/./imzero_client_skia_sdl3_impl/bmp_encoder.cpp)
target_compile_options(imzeroClientSkiaSdl3Impl PUBLIC -Wno-unused-parameter)
target_include_directories(imzeroClientSkiaSdl3Impl PUBLIC C:/Users/User/repo/imzero_client_cpp/skia/./imzero_client_skia_sdl3_impl
C:/Users/User/repo/imzero_client_cpp/skia/./imzero_client_skia_sdl3_impl
C:/Users/User/repo/imzero_client_cpp/skia/./imzero_client_skia_sdl3_impl/../../../contrib/qoi)
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
set_target_properties(libimgui_skia PROPERTIES IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/../imgui_skia/build/imgui_skia.lib")
target_link_libraries(imzeroClientSkiaSdl3Impl libimgui_skia)
