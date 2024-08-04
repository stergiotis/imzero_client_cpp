# generated using cmakelists.dhall
cmake_minimum_required(VERSION 3.24)
project(imgui_skia_exe)
set(CMAKE_LINKER /home/spx/Downloads/clang+llvm-18.1.8-x86_64-linux-gnu-ubuntu-18.04/bin/clang++)
set(CMAKE_CXX_COMPILER /home/spx/Downloads/clang+llvm-18.1.8-x86_64-linux-gnu-ubuntu-18.04/bin/clang++)

add_compile_definitions(SK_RELEASE
SK_GAMMA_APPLY_TO_A8
SK_ALLOW_STATIC_GLOBAL_INITIALIZERS=1
SK_TYPEFACE_FACTORY_FREETYPE
SK_FONTMGR_FREETYPE_EMBEDDED_AVAILABLE
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
"${CMAKE_CURRENT_LIST_DIR}/./src/arena/simple"
"${CMAKE_CURRENT_LIST_DIR}/./contrib/skia"
"${CMAKE_CURRENT_LIST_DIR}/./contrib/tracy/public"
)
add_compile_options(-std=c++20
-fno-omit-frame-pointer
-O3
# sdl3Shared
-I/data/repo/contrib/sdl/include
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
link_libraries(-DNDEBUG
# sdl3Shared
-L/data/repo/contrib/sdl/build
# sdl3Shared
-Wl,-rpath,/data/repo/contrib/sdl/build
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
-L/data/repo/contrib/skia/out/Shared
# skiaShared
-Wl,-rpath,/data/repo/contrib/skia/out/Shared
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
-lglfw
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
add_library(imguiWithSkia OBJECT "${CMAKE_CURRENT_LIST_DIR}/./imgui/imgui.cpp"
"${CMAKE_CURRENT_LIST_DIR}/./imgui/imgui_demo.cpp"
"${CMAKE_CURRENT_LIST_DIR}/./imgui/imgui_draw_fb.cpp"
"${CMAKE_CURRENT_LIST_DIR}/./imgui/imgui_tables.cpp"
"${CMAKE_CURRENT_LIST_DIR}/./imgui/imgui_widgets.cpp"
"${CMAKE_CURRENT_LIST_DIR}/./imgui/flatbufferHelpers.cpp")

# render
add_library(render OBJECT "${CMAKE_CURRENT_LIST_DIR}/./src/render.cpp")
target_include_directories(render PUBLIC "${CMAKE_CURRENT_LIST_DIR}/./src"
"${CMAKE_CURRENT_LIST_DIR}/./src/widgets/imgui_implot"
"${CMAKE_CURRENT_LIST_DIR}/./imcolortextedit")

# marshalling
add_library(marshalling OBJECT "${CMAKE_CURRENT_LIST_DIR}/./src/marshalling/receive.cpp"
"${CMAKE_CURRENT_LIST_DIR}/./src/marshalling/send.cpp"
"${CMAKE_CURRENT_LIST_DIR}/./src/marshalling/helper.cpp")
target_include_directories(marshalling PUBLIC "${CMAKE_CURRENT_LIST_DIR}/./src/marshalling"
"${CMAKE_CURRENT_LIST_DIR}/./src/widgets/imgui_implot")

# arena
add_library(arena OBJECT "${CMAKE_CURRENT_LIST_DIR}/./src/arena/simple/simple.cpp")

# widgets
add_library(widgets OBJECT "${CMAKE_CURRENT_LIST_DIR}/./src/widgets/common.cpp"
"${CMAKE_CURRENT_LIST_DIR}/./src/widgets/piemenu.cpp"
"${CMAKE_CURRENT_LIST_DIR}/./src/widgets/coloredbutton.cpp"
"${CMAKE_CURRENT_LIST_DIR}/./src/widgets/splitter.cpp")
target_compile_options(widgets PUBLIC -Wno-unused-parameter
-Wno-unused-variable)

# imguiToggle
add_library(imguiToggle OBJECT "${CMAKE_CURRENT_LIST_DIR}/./src/widgets/imgui_toggle/imgui_toggle.cpp"
"${CMAKE_CURRENT_LIST_DIR}/./src/widgets/imgui_toggle/imgui_toggle_palette.cpp"
"${CMAKE_CURRENT_LIST_DIR}/./src/widgets/imgui_toggle/imgui_toggle_presets.cpp"
"${CMAKE_CURRENT_LIST_DIR}/./src/widgets/imgui_toggle/imgui_toggle_renderer.cpp")
target_compile_options(imguiToggle PUBLIC -Wno-unused-parameter
-Wno-unused-variable)

# imguiImplot
add_library(imguiImplot OBJECT "${CMAKE_CURRENT_LIST_DIR}/./src/widgets/imgui_implot/implot.cpp"
"${CMAKE_CURRENT_LIST_DIR}/./src/widgets/imgui_implot/implot_demo.cpp"
"${CMAKE_CURRENT_LIST_DIR}/./src/widgets/imgui_implot/implot_items.cpp")

# imguiKnobs
add_library(imguiKnobs OBJECT "${CMAKE_CURRENT_LIST_DIR}/./src/widgets/imgui_knobs/imgui-knobs.cpp")

# imguiCoolbar
add_library(imguiCoolbar OBJECT "${CMAKE_CURRENT_LIST_DIR}/./src/widgets/imgui_coolbar/ImCoolbar.cpp")

# imguiFlamegraph
add_library(imguiFlamegraph OBJECT "${CMAKE_CURRENT_LIST_DIR}/./src/widgets/imgui_flamegraph/imgui_widget_flamegraph.cpp")
target_include_directories(imguiFlamegraph PUBLIC "${CMAKE_CURRENT_LIST_DIR}/./src/widgets/imgui_flamegraph"
"${CMAKE_CURRENT_LIST_DIR}/./src/widgets/imgui_flamegraph")

# imguiTextedit
add_library(imguiTextedit OBJECT "${CMAKE_CURRENT_LIST_DIR}/./imcolortextedit/TextEditor.cpp")
target_compile_options(imguiTextedit PUBLIC -Wno-unused-variable)

# binding

# sdl3Shared

# skiaShared

# mainSkiaSdl3
add_library(mainSkiaSdl3 OBJECT "${CMAKE_CURRENT_LIST_DIR}/./skia/sdl3/imgui_impl_opengl3.cpp"
"${CMAKE_CURRENT_LIST_DIR}/./skia/sdl3/imgui_impl_sdl3.cpp"
"${CMAKE_CURRENT_LIST_DIR}/./skia/sdl3/main.cpp"
"${CMAKE_CURRENT_LIST_DIR}/./skia/sdl3/app.cpp"
"${CMAKE_CURRENT_LIST_DIR}/./skia/sdl3/../paragraph.cpp"
"${CMAKE_CURRENT_LIST_DIR}/./skia/sdl3/../cliOptions.cpp"
"${CMAKE_CURRENT_LIST_DIR}/./skia/sdl3/../setupUI.cpp"
"${CMAKE_CURRENT_LIST_DIR}/./skia/sdl3/../vectorCmdSkiaRenderer.cpp"
"${CMAKE_CURRENT_LIST_DIR}/./skia/sdl3/../skiaTracyTracer.cpp")
target_compile_options(mainSkiaSdl3 PUBLIC -Wno-unused-parameter)
target_include_directories(mainSkiaSdl3 PUBLIC "${CMAKE_CURRENT_LIST_DIR}/./skia/sdl3"
"${CMAKE_CURRENT_LIST_DIR}/./imgui"
"${CMAKE_CURRENT_LIST_DIR}/./src/widgets/imgui_implot"
"${CMAKE_CURRENT_LIST_DIR}/./src")

# tracyDisabled
add_library(tracyDisabled OBJECT "${CMAKE_CURRENT_LIST_DIR}/./contrib/tracy/public/TracyClient.cpp")
add_executable(imgui_skia_exe $<TARGET_OBJECTS:imguiWithSkia>
$<TARGET_OBJECTS:render>
$<TARGET_OBJECTS:marshalling>
$<TARGET_OBJECTS:arena>
$<TARGET_OBJECTS:widgets>
$<TARGET_OBJECTS:imguiToggle>
$<TARGET_OBJECTS:imguiImplot>
$<TARGET_OBJECTS:imguiKnobs>
$<TARGET_OBJECTS:imguiCoolbar>
$<TARGET_OBJECTS:imguiFlamegraph>
$<TARGET_OBJECTS:imguiTextedit>
$<TARGET_OBJECTS:mainSkiaSdl3>
$<TARGET_OBJECTS:tracyDisabled>)

