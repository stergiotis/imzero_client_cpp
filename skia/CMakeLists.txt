# generated using cmakelists.dhall
cmake_minimum_required(VERSION 3.24)
project(imgui_skia_exe)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_LINKER clang++)
set(CMAKE_CXX_COMPILER clang++)

add_compile_definitions(TRACY_ENABLE)
include_directories("${CMAKE_CURRENT_LIST_DIR}/./imgui"
"${CMAKE_CURRENT_LIST_DIR}/./src/arena/simple"
"${CMAKE_CURRENT_LIST_DIR}/./contrib/sdl3/include"
"${CMAKE_CURRENT_LIST_DIR}/./contrib/skia"
"${CMAKE_CURRENT_LIST_DIR}/./contrib/skia/modules/sksg/include"
"${CMAKE_CURRENT_LIST_DIR}/./contrib/skia/modules/bentleyottmann/include"
"${CMAKE_CURRENT_LIST_DIR}/./contrib/skia/modules/skottie/include"
"${CMAKE_CURRENT_LIST_DIR}/./contrib/skia/modules/skparagraph/include"
"${CMAKE_CURRENT_LIST_DIR}/./contrib/skia/modules/skplaintexteditor/include"
"${CMAKE_CURRENT_LIST_DIR}/./contrib/skia/modules/skresources/include"
"${CMAKE_CURRENT_LIST_DIR}/./contrib/skia/modules/skshaper/include"
"${CMAKE_CURRENT_LIST_DIR}/./contrib/skia/modules/skunicode/include"
"${CMAKE_CURRENT_LIST_DIR}/./contrib/skia/modules/svg/include"
"${CMAKE_CURRENT_LIST_DIR}/./contrib/skia/include"
"${CMAKE_CURRENT_LIST_DIR}/./contrib/skia/include/core"
"${CMAKE_CURRENT_LIST_DIR}/./contrib/flatbuffers"
"${CMAKE_CURRENT_LIST_DIR}/./contrib/tracy/public")
set(COMPILE_OPTIONS -std=c++20
-fno-exceptions
-g
-gdwarf-4
-Wall
-Wformat
-Wextra
-O1
-fsanitize=address
-fsanitize=undefined
-fno-omit-frame-pointer
-DIMZERO_DEBUG_BUILD)
link_libraries(-fsanitize=address
-fsanitize=undefined
-fuse-ld=lld
-v
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
-lGL)

add_library(imguiWithSkia OBJECT "${CMAKE_CURRENT_LIST_DIR}/./imgui/imgui.cpp"
"${CMAKE_CURRENT_LIST_DIR}/./imgui/imgui_demo.cpp"
"${CMAKE_CURRENT_LIST_DIR}/./imgui/imgui_draw_fb.cpp"
"${CMAKE_CURRENT_LIST_DIR}/./imgui/imgui_tables.cpp"
"${CMAKE_CURRENT_LIST_DIR}/./imgui/imgui_widgets.cpp"
"${CMAKE_CURRENT_LIST_DIR}/./imgui/flatbufferHelpers.cpp")
target_include_directories(imguiWithSkia PUBLIC "${CMAKE_CURRENT_LIST_DIR}/./imgui"
"${CMAKE_CURRENT_LIST_DIR}/./imgui"
"${CMAKE_CURRENT_LIST_DIR}/./src/widgets/imgui_implot"
"${CMAKE_CURRENT_LIST_DIR}/./src")

add_library(render OBJECT "${CMAKE_CURRENT_LIST_DIR}/./src/render.cpp")
target_include_directories(render PUBLIC "${CMAKE_CURRENT_LIST_DIR}/./src"
"${CMAKE_CURRENT_LIST_DIR}/./src/widgets/imgui_implot"
"${CMAKE_CURRENT_LIST_DIR}/./imcolortextedit")

add_library(marshalling OBJECT "${CMAKE_CURRENT_LIST_DIR}/./src/marshalling/receive.cpp"
"${CMAKE_CURRENT_LIST_DIR}/./src/marshalling/send.cpp"
"${CMAKE_CURRENT_LIST_DIR}/./src/marshalling/helper.cpp")
target_include_directories(marshalling PUBLIC "${CMAKE_CURRENT_LIST_DIR}/./src/marshalling"
"${CMAKE_CURRENT_LIST_DIR}/./src/widgets/imgui_implot")

add_library(arena OBJECT "${CMAKE_CURRENT_LIST_DIR}/./src/arena/simple/simple.cpp")

add_library(widgets OBJECT "${CMAKE_CURRENT_LIST_DIR}/./src/widgets/common.cpp"
"${CMAKE_CURRENT_LIST_DIR}/./src/widgets/piemenu.cpp"
"${CMAKE_CURRENT_LIST_DIR}/./src/widgets/coloredbutton.cpp"
"${CMAKE_CURRENT_LIST_DIR}/./src/widgets/splitter.cpp")
target_compile_options(widgets PUBLIC -Wno-unused-parameter
-Wno-unused-variable)

add_library(imguiToggle OBJECT "${CMAKE_CURRENT_LIST_DIR}/./src/widgets/imgui_toggle/imgui_toggle.cpp"
"${CMAKE_CURRENT_LIST_DIR}/./src/widgets/imgui_toggle/imgui_toggle_palette.cpp"
"${CMAKE_CURRENT_LIST_DIR}/./src/widgets/imgui_toggle/imgui_toggle_presets.cpp"
"${CMAKE_CURRENT_LIST_DIR}/./src/widgets/imgui_toggle/imgui_toggle_renderer.cpp")
target_compile_options(imguiToggle PUBLIC -Wno-unused-parameter
-Wno-unused-variable)

add_library(imguiImplot OBJECT "${CMAKE_CURRENT_LIST_DIR}/./src/widgets/imgui_implot/implot.cpp"
"${CMAKE_CURRENT_LIST_DIR}/./src/widgets/imgui_implot/implot_demo.cpp"
"${CMAKE_CURRENT_LIST_DIR}/./src/widgets/imgui_implot/implot_items.cpp")

add_library(imguiKnobs OBJECT "${CMAKE_CURRENT_LIST_DIR}/./src/widgets/imgui_knobs/imgui-knobs.cpp")

add_library(imguiCoolbar OBJECT "${CMAKE_CURRENT_LIST_DIR}/./src/widgets/imgui_coolbar/ImCoolbar.cpp")

add_library(imguiFlamegraph OBJECT "${CMAKE_CURRENT_LIST_DIR}/./src/widgets/imgui_flamegraph/imgui_widget_flamegraph.cpp")
target_include_directories(imguiFlamegraph PUBLIC "${CMAKE_CURRENT_LIST_DIR}/./src/widgets/imgui_flamegraph"
"${CMAKE_CURRENT_LIST_DIR}/./src/widgets/imgui_flamegraph")

add_library(imguiTextedit OBJECT "${CMAKE_CURRENT_LIST_DIR}/./imcolortextedit/TextEditor.cpp")
target_compile_options(imguiTextedit PUBLIC -Wno-unused-variable)


add_library(sdl3_imported OBJECT IMPORTED)
set_property(TARGET sdl3_imported PROPERTY IMPORTED_OBJECTS "${CMAKE_CURRENT_LIST_DIR}/./contrib/sdl3/build/libSDL3.a")

add_library(skiaSdl OBJECT "${CMAKE_CURRENT_LIST_DIR}/./skia/sdl3/imgui_impl_opengl3.cpp"
"${CMAKE_CURRENT_LIST_DIR}/./skia/sdl3/imgui_impl_sdl3.cpp"
"${CMAKE_CURRENT_LIST_DIR}/./skia/sdl3/main.cpp"
"${CMAKE_CURRENT_LIST_DIR}/./skia/sdl3/app.cpp"
"${CMAKE_CURRENT_LIST_DIR}/./skia/paragraph.cpp"
"${CMAKE_CURRENT_LIST_DIR}/./skia/cliOptions.cpp"
"${CMAKE_CURRENT_LIST_DIR}/./skia/setupUI.cpp"
"${CMAKE_CURRENT_LIST_DIR}/./skia/vectorCmdSkiaRenderer.cpp"
"${CMAKE_CURRENT_LIST_DIR}/./skia/skiaTracyTracer.cpp"
"${CMAKE_CURRENT_LIST_DIR}/./contrib/skia/src/gpu/ganesh/gl/GrGLInterfaceAutogen.cpp"
"${CMAKE_CURRENT_LIST_DIR}/./contrib/skia/src/gpu/ganesh/gl/GrGLUtil.cpp")
target_compile_options(skiaSdl PUBLIC -Wno-unused-parameter)
target_compile_definitions(skiaSdl PUBLIC SK_DEBUG
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
SK_HAS_HEIF_LIBRARY
SK_CODEC_DECODES_PNG
SK_CODEC_DECODES_RAW
SK_CODEC_DECODES_WEBP
SK_HAS_WUFFS_LIBRARY
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
SK_BUILD_FOR_UNIX)
target_include_directories(skiaSdl PUBLIC "${CMAKE_CURRENT_LIST_DIR}/./skia"
"${CMAKE_CURRENT_LIST_DIR}/./imgui"
"${CMAKE_CURRENT_LIST_DIR}/./src/widgets/imgui_implot"
"${CMAKE_CURRENT_LIST_DIR}/./src")
add_library(skiaSdl_imported OBJECT IMPORTED)
set_property(TARGET skiaSdl_imported PROPERTY IMPORTED_OBJECTS "${CMAKE_CURRENT_LIST_DIR}/./contrib/skia/out/asan/obj/../libskparagraph.a"
"${CMAKE_CURRENT_LIST_DIR}/./contrib/skia/out/asan/obj/../libsvg.a"
"${CMAKE_CURRENT_LIST_DIR}/./contrib/skia/out/asan/obj/../libskia.a"
"${CMAKE_CURRENT_LIST_DIR}/./contrib/skia/out/asan/obj/../libskshaper.a"
"${CMAKE_CURRENT_LIST_DIR}/./contrib/skia/out/asan/obj/../libskunicode.a"
"${CMAKE_CURRENT_LIST_DIR}/./contrib/skia/out/asan/obj/../libwindow.a"
"${CMAKE_CURRENT_LIST_DIR}/./contrib/skia/out/asan/obj/../libcompression_utils_portable.a"
"${CMAKE_CURRENT_LIST_DIR}/./contrib/skia/out/asan/obj/../libdng_sdk.a"
"${CMAKE_CURRENT_LIST_DIR}/./contrib/skia/out/asan/obj/../libexpat.a"
"${CMAKE_CURRENT_LIST_DIR}/./contrib/skia/out/asan/obj/../libharfbuzz.a"
"${CMAKE_CURRENT_LIST_DIR}/./contrib/skia/out/asan/obj/../libicu.a"
"${CMAKE_CURRENT_LIST_DIR}/./contrib/skia/out/asan/obj/../libicu_bidi.a"
"${CMAKE_CURRENT_LIST_DIR}/./contrib/skia/out/asan/obj/../libjpeg.a"
"${CMAKE_CURRENT_LIST_DIR}/./contrib/skia/out/asan/obj/../libmicrohttpd.a"
"${CMAKE_CURRENT_LIST_DIR}/./contrib/skia/out/asan/obj/../libpathkit.a"
"${CMAKE_CURRENT_LIST_DIR}/./contrib/skia/out/asan/obj/../libperfetto.a"
"${CMAKE_CURRENT_LIST_DIR}/./contrib/skia/out/asan/obj/../libpiex.a"
"${CMAKE_CURRENT_LIST_DIR}/./contrib/skia/out/asan/obj/../libpng.a")


add_library(tracyEnabled OBJECT "${CMAKE_CURRENT_LIST_DIR}/./contrib/tracy/public/TracyClient.cpp")
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
$<TARGET_OBJECTS:sdl3_imported>
$<TARGET_OBJECTS:skiaSdl>
$<TARGET_OBJECTS:skiaSdl_imported>
$<TARGET_OBJECTS:tracyEnabled>)

