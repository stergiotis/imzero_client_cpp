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
include_directories(/home/deck/repo/imzero_client_cpp/skia2/./src/marshalling
/home/deck/repo/imzero_client_cpp/skia2/./src/arena/simple
/home/deck/repo/imzero_client_cpp/skia2/./src/binding
/home/deck/repo/imzero_client_cpp/skia2/./src
/home/deck/repo/imzero_client_cpp/skia2/./src/widgets/imgui_implot
/home/deck/repo/imzero_client_cpp/skia2/./src/widgets/imgui_toggle
/home/deck/repo/imzero_client_cpp/skia2/./src/widgets/imgui_knobs
/home/deck/repo/imzero_client_cpp/skia2/./src/widgets/imgui_coolbar
/home/deck/repo/imzero_client_cpp/skia2/./src/widgets/imgui_flamegraph
/home/deck/repo/imzero_client_cpp/skia2/./imcolortextedit
/home/deck/repo/imzero_client_cpp/skia2/./src/widgets
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
# systemFlags
-fPIC
# sdl3Shared
-I/home/deck/repo/imzero_client_cpp/skia_minimal/./../skia_minimal/contrib/sdl/include
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
link_libraries(-Wl,-rpath,'$ORIGIN/../lib' -Wl,-z,origin
-DNDEBUG
# systemFlags
-Wl,-rpath,'$ORIGIN/../lib' -Wl,-z,origin
# sdl3Shared
-L/home/deck/repo/imzero_client_cpp/skia_minimal/./../skia_minimal/contrib/sdl/build
-Wl,--enable-new-dtags
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
-L/home/deck/repo/imzero_client_cpp/skia_minimal/./../../contrib/skia/out/Shared
-lskparagraph
-lskia
-lskunicode
-lbentleyottmann
-lskshaper
)
if(CMAKE_C_COMPILER_ID STREQUAL "GNU" AND CMAKE_SYSTEM_NAME STREQUAL "Linux")
add_link_options(-Wl,--start-group)
endif()
# begin marshalling
add_library(marshalling STATIC /home/deck/repo/imzero_client_cpp/skia2/./src/marshalling/receive.cpp
/home/deck/repo/imzero_client_cpp/skia2/./src/marshalling/send.cpp
/home/deck/repo/imzero_client_cpp/skia2/./src/marshalling/helper.cpp)
# end marshalling

# begin arena
add_library(arena STATIC /home/deck/repo/imzero_client_cpp/skia2/./src/arena/simple/simple.cpp)
# end arena

# empty target binding

# begin render
add_library(render STATIC /home/deck/repo/imzero_client_cpp/skia2/./src/render.cpp)
# end render

# begin imguiImplot
add_library(imguiImplot STATIC /home/deck/repo/imzero_client_cpp/skia2/./src/widgets/imgui_implot/implot.cpp
/home/deck/repo/imzero_client_cpp/skia2/./src/widgets/imgui_implot/implot_demo.cpp
/home/deck/repo/imzero_client_cpp/skia2/./src/widgets/imgui_implot/implot_items.cpp)
# end imguiImplot

# begin imguiToggle
add_library(imguiToggle STATIC /home/deck/repo/imzero_client_cpp/skia2/./src/widgets/imgui_toggle/imgui_toggle.cpp
/home/deck/repo/imzero_client_cpp/skia2/./src/widgets/imgui_toggle/imgui_toggle_palette.cpp
/home/deck/repo/imzero_client_cpp/skia2/./src/widgets/imgui_toggle/imgui_toggle_presets.cpp
/home/deck/repo/imzero_client_cpp/skia2/./src/widgets/imgui_toggle/imgui_toggle_renderer.cpp)
target_compile_options(imguiToggle PUBLIC -Wno-unused-parameter
-Wno-unused-variable)
# end imguiToggle

# begin imguiKnobs
add_library(imguiKnobs STATIC /home/deck/repo/imzero_client_cpp/skia2/./src/widgets/imgui_knobs/imgui-knobs.cpp)
# end imguiKnobs

# begin imguiCoolbar
add_library(imguiCoolbar STATIC /home/deck/repo/imzero_client_cpp/skia2/./src/widgets/imgui_coolbar/ImCoolbar.cpp)
# end imguiCoolbar

# begin imguiFlamegraph
add_library(imguiFlamegraph STATIC /home/deck/repo/imzero_client_cpp/skia2/./src/widgets/imgui_flamegraph/imgui_widget_flamegraph.cpp)
# end imguiFlamegraph

# begin imguiTextedit
add_library(imguiTextedit STATIC /home/deck/repo/imzero_client_cpp/skia2/./imcolortextedit/TextEditor.cpp
/home/deck/repo/imzero_client_cpp/skia2/./imcolortextedit/LanguageDefinitions.cpp)
target_compile_options(imguiTextedit PUBLIC -Wno-unused-variable)
# end imguiTextedit

# begin widgets
add_library(widgets STATIC /home/deck/repo/imzero_client_cpp/skia2/./src/widgets/common.cpp
/home/deck/repo/imzero_client_cpp/skia2/./src/widgets/piemenu.cpp
/home/deck/repo/imzero_client_cpp/skia2/./src/widgets/splitter.cpp)
target_compile_options(widgets PUBLIC -Wno-unused-parameter
-Wno-unused-variable)
# end widgets

# begin imzeroClientSkiaSdl3Impl
add_executable(imzeroClientSkiaSdl3Impl  /home/deck/repo/imzero_client_cpp/skia2/./imzero_client_skia_sdl3_impl/main.cpp
/home/deck/repo/imzero_client_cpp/skia2/./imzero_client_skia_sdl3_impl/imzero_client_skia_sdl3_app.cpp
/home/deck/repo/imzero_client_cpp/skia2/./imzero_client_skia_sdl3_impl/imzero_client_skia_sdl3_cli_options.cpp
/home/deck/repo/imzero_client_cpp/skia2/./imzero_client_skia_sdl3_impl/bmp_encoder.cpp)
target_compile_options(imzeroClientSkiaSdl3Impl PUBLIC -Wno-unused-parameter)
target_include_directories(imzeroClientSkiaSdl3Impl PUBLIC /home/deck/repo/imzero_client_cpp/skia2/./imzero_client_skia_sdl3_impl
/home/deck/repo/imzero_client_cpp/skia2/./imzero_client_skia_sdl3_impl)
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
set_target_properties(libimgui_skia PROPERTIES IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/../skia_minimal/build/libimgui_skia.a")
target_link_libraries(imzeroClientSkiaSdl3Impl libimgui_skia)
