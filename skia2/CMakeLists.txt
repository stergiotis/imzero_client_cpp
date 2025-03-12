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
include_directories(/home/deck/repo/imzero_client_cpp/skia2/./src/widgets/imgui_implot
/home/deck/repo/imzero_client_cpp/skia2/./src/widgets/imgui_toggle
/home/deck/repo/imzero_client_cpp/skia2/./src/widgets/imgui_knobs
/home/deck/repo/imzero_client_cpp/skia2/./src/widgets/imgui_coolbar
/home/deck/repo/imzero_client_cpp/skia2/./src/widgets/imgui_flamegraph
/home/deck/repo/imzero_client_cpp/skia2/./imcolortextedit
/home/deck/repo/imzero_client_cpp/skia2/./src/marshalling
/home/deck/repo/imzero_client_cpp/skia2/./src/arena/simple
/home/deck/repo/imzero_client_cpp/skia2/./src/widgets
/home/deck/repo/imzero_client_cpp/skia2/./src
/home/deck/repo/imzero_client_cpp/skia2/./src/binding
/home/deck/repo/imzero_client_cpp/skia_minimal/./../skia_minimal/contrib/flatbuffers/include
/home/deck/repo/imzero_client_cpp/skia_minimal/./../skia_minimal/imgui_w_hooks_1.91.9_wip
/home/deck/repo/imzero_client_cpp/skia_minimal/./../skia_minimal/imgui_skia_impl
/home/deck/repo/imzero_client_cpp/skia_minimal/./../../contrib/skia
/home/deck/repo/imzero_client_cpp/skia_minimal/./../skia_minimal/imgui_skia_driver_impl
/home/deck/repo/imzero_client_cpp/skia_minimal/./../skia_minimal/contrib/tracy/public
)
add_compile_options(-std=c++20
-Wall
-Wformat
-Wextra
-Wno-unused-parameter
-fno-omit-frame-pointer
-O3
# sdl3Shared
-I/home/deck/repo/imzero_client_cpp/skia_minimal/./../skia_minimal/contrib/sdl/include
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
-L/home/deck/repo/imzero_client_cpp/skia_minimal/./../skia_minimal/contrib/sdl/build
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
-L/home/deck/repo/imzero_client_cpp/skia_minimal/./../../contrib/skia/out/Shared
-lskparagraph
-lskia
-lskunicode
-lbentleyottmann
-lskshaper
)

# begin imguiImplot
add_library(imguiImplot OBJECT /home/deck/repo/imzero_client_cpp/skia2/./src/widgets/imgui_implot/implot.cpp
/home/deck/repo/imzero_client_cpp/skia2/./src/widgets/imgui_implot/implot_demo.cpp
/home/deck/repo/imzero_client_cpp/skia2/./src/widgets/imgui_implot/implot_items.cpp)
# end imguiImplot

# begin imguiToggle
add_library(imguiToggle OBJECT /home/deck/repo/imzero_client_cpp/skia2/./src/widgets/imgui_toggle/imgui_toggle.cpp
/home/deck/repo/imzero_client_cpp/skia2/./src/widgets/imgui_toggle/imgui_toggle_palette.cpp
/home/deck/repo/imzero_client_cpp/skia2/./src/widgets/imgui_toggle/imgui_toggle_presets.cpp
/home/deck/repo/imzero_client_cpp/skia2/./src/widgets/imgui_toggle/imgui_toggle_renderer.cpp)
target_compile_options(imguiToggle PUBLIC -Wno-unused-parameter
-Wno-unused-variable)
# end imguiToggle

# begin imguiKnobs
add_library(imguiKnobs OBJECT /home/deck/repo/imzero_client_cpp/skia2/./src/widgets/imgui_knobs/imgui-knobs.cpp)
# end imguiKnobs

# begin imguiCoolbar
add_library(imguiCoolbar OBJECT /home/deck/repo/imzero_client_cpp/skia2/./src/widgets/imgui_coolbar/ImCoolbar.cpp)
# end imguiCoolbar

# begin imguiFlamegraph
add_library(imguiFlamegraph OBJECT /home/deck/repo/imzero_client_cpp/skia2/./src/widgets/imgui_flamegraph/imgui_widget_flamegraph.cpp)
# end imguiFlamegraph

# begin imguiTextedit
add_library(imguiTextedit OBJECT /home/deck/repo/imzero_client_cpp/skia2/./imcolortextedit/TextEditor.cpp
/home/deck/repo/imzero_client_cpp/skia2/./imcolortextedit/LanguageDefinitions.cpp)
target_compile_options(imguiTextedit PUBLIC -Wno-unused-variable)
# end imguiTextedit

# begin marshalling
add_library(marshalling OBJECT /home/deck/repo/imzero_client_cpp/skia2/./src/marshalling/receive.cpp
/home/deck/repo/imzero_client_cpp/skia2/./src/marshalling/send.cpp
/home/deck/repo/imzero_client_cpp/skia2/./src/marshalling/helper.cpp)
# end marshalling

# begin arena
add_library(arena OBJECT /home/deck/repo/imzero_client_cpp/skia2/./src/arena/simple/simple.cpp)
# end arena

# begin widgets
add_library(widgets OBJECT /home/deck/repo/imzero_client_cpp/skia2/./src/widgets/common.cpp
/home/deck/repo/imzero_client_cpp/skia2/./src/widgets/piemenu.cpp
/home/deck/repo/imzero_client_cpp/skia2/./src/widgets/splitter.cpp)
target_compile_options(widgets PUBLIC -Wno-unused-parameter
-Wno-unused-variable)
# end widgets

# begin render
add_library(render OBJECT /home/deck/repo/imzero_client_cpp/skia2/./src/render.cpp)
# end render

# empty target binding

# begin imzeroClientSkiaSdl3Impl
add_library(imzeroClientSkiaSdl3Impl OBJECT /home/deck/repo/imzero_client_cpp/skia2/./imzero_client_skia_sdl3_impl/main.cpp
/home/deck/repo/imzero_client_cpp/skia2/./imzero_client_skia_sdl3_impl/imzero_client_skia_sdl3_app.cpp
/home/deck/repo/imzero_client_cpp/skia2/./imzero_client_skia_sdl3_impl/imzero_client_skia_sdl3_cli_options.cpp
/home/deck/repo/imzero_client_cpp/skia2/./imzero_client_skia_sdl3_impl/bmp_encoder.cpp)
target_compile_options(imzeroClientSkiaSdl3Impl PUBLIC -Wno-unused-parameter)
target_include_directories(imzeroClientSkiaSdl3Impl PUBLIC /home/deck/repo/imzero_client_cpp/skia2/./imzero_client_skia_sdl3_impl
/home/deck/repo/imzero_client_cpp/skia2/./imzero_client_skia_sdl3_impl)
# end imzeroClientSkiaSdl3Impl
add_executable(imgui_skia_exe $<TARGET_OBJECTS:imguiImplot>
$<TARGET_OBJECTS:imguiToggle>
$<TARGET_OBJECTS:imguiKnobs>
$<TARGET_OBJECTS:imguiCoolbar>
$<TARGET_OBJECTS:imguiFlamegraph>
$<TARGET_OBJECTS:imguiTextedit>
$<TARGET_OBJECTS:marshalling>
$<TARGET_OBJECTS:arena>
$<TARGET_OBJECTS:widgets>
$<TARGET_OBJECTS:render>
$<TARGET_OBJECTS:imzeroClientSkiaSdl3Impl>)
add_executable(imzero_client_skia_sdl3 $<TARGET_OBJECTS:imzeroClientSkiaSdl3Impl>)
add_library(libimgui_skia STATIC IMPORTED)
set_target_properties(libimgui_skia PROPERTIES IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/../skia_minimal/build/libimgui_skia.a")
target_link_libraries(imzero_client_skia_sdl3 libimgui_skia)
