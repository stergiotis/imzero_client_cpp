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
include_directories(./contrib/flatbuffers
./imgui
./src/arena/simple
./contrib/skia
./contrib/tracy/public
)
add_compile_options(-std=c++20
-Wall
-Wformat
-Wextra
-Wno-unused-parameter
-fno-omit-frame-pointer
-O3
# sdl3Shared
-I/home/deck/repo/imzero_client_cpp/skia/./../../contrib/sdl/include
# skiaShared
-ffp-contract=off
-fstrict-aliasing
-fPIC
-fvisibility=hidden
-fdata-sections
-ffunction-sections
-fvisibility-inlines-hidden
-fno-exceptions
-fno-rtti
)
link_libraries(-Wl,-rpath,'$ORIGIN/../lib' -Wl,-z,origin
-DNDEBUG
# sdl3Shared
-L/home/deck/repo/imzero_client_cpp/skia/./../../contrib/sdl/build
-Wl,--enable-new-dtags
-lSDL3
# skiaShared
-ldl
-lpthread
-lfreetype
-lz
-lglfw
-lfontconfig
-lwebpmux
-lwebpdemux
-lX11
-lGLU
-lGL
-L/home/deck/repo/imzero_client_cpp/skia/./../../contrib/skia/out/Shared
-lskparagraph
-lskia
-lskunicode
-lbentleyottmann
-lskshaper
# mainSkiaSdl3
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
)

# empty target flatbuffers

# begin imguiWithSkia
add_library(imguiWithSkia STATIC ./imgui/imzero_hooks.cpp
./imgui/imzero_extensions.cpp
./imgui/imgui.cpp
./imgui/imgui_demo.cpp
./imgui/imgui_draw.cpp
./imgui/imgui_tables.cpp
./imgui/imgui_widgets.cpp)
# end imguiWithSkia

# begin render
add_library(render STATIC ./src/render.cpp)
target_include_directories(render PUBLIC ./src
./src/widgets/imgui_implot
./imcolortextedit)
# end render

# begin marshalling
add_library(marshalling STATIC ./src/marshalling/receive.cpp
./src/marshalling/send.cpp
./src/marshalling/helper.cpp)
target_include_directories(marshalling PUBLIC ./src/marshalling
./src/widgets/imgui_implot)
# end marshalling

# begin arena
add_library(arena STATIC ./src/arena/simple/simple.cpp)
# end arena

# begin widgets
add_library(widgets STATIC ./src/widgets/common.cpp
./src/widgets/piemenu.cpp
./src/widgets/splitter.cpp)
target_compile_options(widgets PUBLIC -Wno-unused-parameter
-Wno-unused-variable)
# end widgets

# begin imguiToggle
add_library(imguiToggle STATIC ./src/widgets/imgui_toggle/imgui_toggle.cpp
./src/widgets/imgui_toggle/imgui_toggle_palette.cpp
./src/widgets/imgui_toggle/imgui_toggle_presets.cpp
./src/widgets/imgui_toggle/imgui_toggle_renderer.cpp)
target_compile_options(imguiToggle PUBLIC -Wno-unused-parameter
-Wno-unused-variable)
# end imguiToggle

# begin imguiImplot
add_library(imguiImplot STATIC ./src/widgets/imgui_implot/implot.cpp
./src/widgets/imgui_implot/implot_demo.cpp
./src/widgets/imgui_implot/implot_items.cpp)
# end imguiImplot

# begin imguiKnobs
add_library(imguiKnobs STATIC ./src/widgets/imgui_knobs/imgui-knobs.cpp)
# end imguiKnobs

# begin imguiCoolbar
add_library(imguiCoolbar STATIC ./src/widgets/imgui_coolbar/ImCoolbar.cpp)
# end imguiCoolbar

# begin imguiFlamegraph
add_library(imguiFlamegraph STATIC ./src/widgets/imgui_flamegraph/imgui_widget_flamegraph.cpp)
target_include_directories(imguiFlamegraph PUBLIC ./src/widgets/imgui_flamegraph
./src/widgets/imgui_flamegraph)
# end imguiFlamegraph

# begin imguiTextedit
add_library(imguiTextedit STATIC ./imcolortextedit/TextEditor.cpp
./imcolortextedit/LanguageDefinitions.cpp)
target_compile_options(imguiTextedit PUBLIC -Wno-unused-variable)
# end imguiTextedit

# empty target binding

# empty target sdl3Shared

# empty target skiaShared

# begin mainSkiaSdl3
add_executable(mainSkiaSdl3  ./main/sdl3/../../imgui/imgui_impl_sdl3.cpp
./main/sdl3/main.cpp
./main/sdl3/app.cpp
./main/sdl3/../paragraph.cpp
./main/sdl3/../cliOptions.cpp
./main/sdl3/../setupUI.cpp
./main/sdl3/../vectorCmdSkiaRenderer.cpp)
target_compile_options(mainSkiaSdl3 PUBLIC -Wno-unused-parameter)
target_include_directories(mainSkiaSdl3 PUBLIC ./main/sdl3
./imgui
./src/widgets/imgui_implot
./src
./main/sdl3/..)
target_link_libraries(mainSkiaSdl3 imguiWithSkia)
target_link_libraries(mainSkiaSdl3 render)
target_link_libraries(mainSkiaSdl3 marshalling)
target_link_libraries(mainSkiaSdl3 arena)
target_link_libraries(mainSkiaSdl3 widgets)
target_link_libraries(mainSkiaSdl3 imguiToggle)
target_link_libraries(mainSkiaSdl3 imguiImplot)
target_link_libraries(mainSkiaSdl3 imguiKnobs)
target_link_libraries(mainSkiaSdl3 imguiCoolbar)
target_link_libraries(mainSkiaSdl3 imguiFlamegraph)
target_link_libraries(mainSkiaSdl3 imguiTextedit)
target_link_libraries(mainSkiaSdl3 tracyDisabled)
# end mainSkiaSdl3

# begin tracyDisabled
add_library(tracyDisabled STATIC ./contrib/tracy/public/TracyClient.cpp)
# end tracyDisabled
