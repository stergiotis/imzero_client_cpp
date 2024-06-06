# generated using cmakelists.dhall
cmake_minimum_required(VERSION 3.24)
project(imgui_skia_exe)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_COMPILER clang++)

add_compile_definitions()
include_directories("${CMAKE_CURRENT_LIST_DIR}/./imgui"
"${CMAKE_CURRENT_LIST_DIR}/./src/arena/simple"
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
-O3)
link_libraries(-DNDEBUG
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


add_library(skia OBJECT "${CMAKE_CURRENT_LIST_DIR}/./skia/modified/app.cpp"
"${CMAKE_CURRENT_LIST_DIR}/./skia/modified/ImGuiLayer.cpp"
"${CMAKE_CURRENT_LIST_DIR}/./skia/paragraph.cpp"
"${CMAKE_CURRENT_LIST_DIR}/./skia/setupUI.cpp"
"${CMAKE_CURRENT_LIST_DIR}/./skia/vectorCmdSkiaRenderer.cpp"
"${CMAKE_CURRENT_LIST_DIR}/./skia/skiaTracyTracer.cpp")
target_compile_options(skia PUBLIC -Wno-unused-parameter)
target_compile_definitions(skia PUBLIC SK_TRIVIAL_ABI=\[\[clang::trivial_abi\]\]
SK_GAMMA_APPLY_TO_A8
SK_ALLOW_STATIC_GLOBAL_INITIALIZERS=1
GR_TEST_UTILS=1
SK_TYPEFACE_FACTORY_FREETYPE
SK_FONTMGR_ANDROID_AVAILABLE
SK_FONTMGR_FREETYPE_DIRECTORY_AVAILABLE
SK_FONTMGR_FREETYPE_EMBEDDED_AVAILABLE
SK_FONTMGR_FREETYPE_EMPTY_AVAILABLE
SK_FONTMGR_FONTCONFIG_AVAILABLE
SK_GL
SK_SUPPORT_PDF
SK_CODEC_DECODES_JPEG
SK_CODEC_DECODES_JPEG_GAINMAPS
SK_XML
SK_ENABLE_ANDROID_UTILS
SK_HAS_HEIF_LIBRARY
SK_CODEC_DECODES_PNG
SK_CODEC_DECODES_RAW
SK_CODEC_DECODES_WEBP
SK_HAS_WUFFS_LIBRARY
SK_DEFAULT_TYPEFACE_IS_EMPTY
SK_DISABLE_LEGACY_DEFAULT_TYPEFACE
SK_R32_SHIFT=16
SK_ENABLE_PRECOMPILE
SKSL_ENABLE_TRACING
SK_GANESH
SK_USE_PERFETTO
SK_ENABLE_SKOTTIE
SK_ENABLE_SKOTTIE_SKSLEFFECT
SK_ENABLE_PARAGRAPH
SK_UNICODE_AVAILABLE
SK_UNICODE_ICU_IMPLEMENTATION
SK_SHAPER_PRIMITIVE_AVAILABLE
SK_SHAPER_HARFBUZZ_AVAILABLE
SK_SHAPER_UNICODE_AVAILABLE
SK_ENABLE_SVG
SK_BUILD_FOR_UNIX)
target_include_directories(skia PUBLIC "${CMAKE_CURRENT_LIST_DIR}/./skia"
"${CMAKE_CURRENT_LIST_DIR}/./imgui"
"${CMAKE_CURRENT_LIST_DIR}/./src/widgets/imgui_implot"
"${CMAKE_CURRENT_LIST_DIR}/./src")
add_library(skia_imported OBJECT IMPORTED)
set_property(TARGET skia_imported PROPERTY IMPORTED_OBJECTS "${CMAKE_CURRENT_LIST_DIR}/./contrib/skia/out/Static/obj/tools/flags/flags.CommandLineFlags.o"
"${CMAKE_CURRENT_LIST_DIR}/./contrib/skia/out/Static/obj/tools/gpu/gpu_tool_utils.BackendSurfaceFactory.o"
"${CMAKE_CURRENT_LIST_DIR}/./contrib/skia/out/Static/obj/tools/gpu/gpu_tool_utils.BackendTextureImageFactory.o"
"${CMAKE_CURRENT_LIST_DIR}/./contrib/skia/out/Static/obj/tools/gpu/gpu_tool_utils.ContextType.o"
"${CMAKE_CURRENT_LIST_DIR}/./contrib/skia/out/Static/obj/tools/gpu/gpu_tool_utils.FlushFinishTracker.o"
"${CMAKE_CURRENT_LIST_DIR}/./contrib/skia/out/Static/obj/tools/gpu/gpu_tool_utils.GrContextFactory.o"
"${CMAKE_CURRENT_LIST_DIR}/./contrib/skia/out/Static/obj/tools/gpu/gpu_tool_utils.GrTest.o"
"${CMAKE_CURRENT_LIST_DIR}/./contrib/skia/out/Static/obj/tools/gpu/gpu_tool_utils.ManagedBackendTexture.o"
"${CMAKE_CURRENT_LIST_DIR}/./contrib/skia/out/Static/obj/tools/gpu/gpu_tool_utils.MemoryCache.o"
"${CMAKE_CURRENT_LIST_DIR}/./contrib/skia/out/Static/obj/tools/gpu/gpu_tool_utils.ProtectedUtils.o"
"${CMAKE_CURRENT_LIST_DIR}/./contrib/skia/out/Static/obj/tools/gpu/gpu_tool_utils.ProxyUtils.o"
"${CMAKE_CURRENT_LIST_DIR}/./contrib/skia/out/Static/obj/tools/gpu/gpu_tool_utils.TestContext.o"
"${CMAKE_CURRENT_LIST_DIR}/./contrib/skia/out/Static/obj/tools/gpu/gpu_tool_utils.TestOps.o"
"${CMAKE_CURRENT_LIST_DIR}/./contrib/skia/out/Static/obj/tools/gpu/gpu_tool_utils.YUVUtils.o"
"${CMAKE_CURRENT_LIST_DIR}/./contrib/skia/out/Static/obj/tools/gpu/mock/gpu_tool_utils.MockTestContext.o"
"${CMAKE_CURRENT_LIST_DIR}/./contrib/skia/out/Static/obj/src/utils/gpu_tool_utils.SkTestCanvas.o"
"${CMAKE_CURRENT_LIST_DIR}/./contrib/skia/out/Static/obj/tools/gpu/gl/gpu_tool_utils.GLTestContext.o"
"${CMAKE_CURRENT_LIST_DIR}/./contrib/skia/out/Static/obj/tools/gpu/gl/glx/gpu_tool_utils.CreatePlatformGLTestContext_glx.o"
"${CMAKE_CURRENT_LIST_DIR}/./contrib/skia/out/Static/obj/tools/sk_app/sk_app.CommandSet.o"
"${CMAKE_CURRENT_LIST_DIR}/./contrib/skia/out/Static/obj/tools/sk_app/sk_app.Window.o"
"${CMAKE_CURRENT_LIST_DIR}/./contrib/skia/out/Static/obj/tools/sk_app.SkGetExecutablePath_linux.o"
"${CMAKE_CURRENT_LIST_DIR}/./contrib/skia/out/Static/obj/tools/sk_app/unix/sk_app.Window_unix.o"
"${CMAKE_CURRENT_LIST_DIR}/./contrib/skia/out/Static/obj/tools/sk_app/unix/sk_app.keysym2ucs.o"
"${CMAKE_CURRENT_LIST_DIR}/./contrib/skia/out/Static/obj/tools/sk_app/unix/sk_app.main_unix.o"
"${CMAKE_CURRENT_LIST_DIR}/./contrib/skia/out/Static/obj/tools/tool_utils.AndroidSkDebugToStdOut.o"
"${CMAKE_CURRENT_LIST_DIR}/./contrib/skia/out/Static/obj/tools/tool_utils.DDLPromiseImageHelper.o"
"${CMAKE_CURRENT_LIST_DIR}/./contrib/skia/out/Static/obj/tools/tool_utils.DDLTileHelper.o"
"${CMAKE_CURRENT_LIST_DIR}/./contrib/skia/out/Static/obj/tools/tool_utils.DecodeUtils.o"
"${CMAKE_CURRENT_LIST_DIR}/./contrib/skia/out/Static/obj/tools/tool_utils.EncodeUtils.o"
"${CMAKE_CURRENT_LIST_DIR}/./contrib/skia/out/Static/obj/tools/tool_utils.GpuToolUtils.o"
"${CMAKE_CURRENT_LIST_DIR}/./contrib/skia/out/Static/obj/tools/tool_utils.LsanSuppressions.o"
"${CMAKE_CURRENT_LIST_DIR}/./contrib/skia/out/Static/obj/tools/tool_utils.MSKPPlayer.o"
"${CMAKE_CURRENT_LIST_DIR}/./contrib/skia/out/Static/obj/tools/tool_utils.ProcStats.o"
"${CMAKE_CURRENT_LIST_DIR}/./contrib/skia/out/Static/obj/tools/tool_utils.Resources.o"
"${CMAKE_CURRENT_LIST_DIR}/./contrib/skia/out/Static/obj/tools/tool_utils.RuntimeBlendUtils.o"
"${CMAKE_CURRENT_LIST_DIR}/./contrib/skia/out/Static/obj/tools/tool_utils.SkMetaData.o"
"${CMAKE_CURRENT_LIST_DIR}/./contrib/skia/out/Static/obj/tools/tool_utils.SkSharingProc.o"
"${CMAKE_CURRENT_LIST_DIR}/./contrib/skia/out/Static/obj/tools/tool_utils.TestFontDataProvider.o"
"${CMAKE_CURRENT_LIST_DIR}/./contrib/skia/out/Static/obj/tools/tool_utils.ToolUtils.o"
"${CMAKE_CURRENT_LIST_DIR}/./contrib/skia/out/Static/obj/tools/tool_utils.UrlDataManager.o"
"${CMAKE_CURRENT_LIST_DIR}/./contrib/skia/out/Static/obj/tools/debugger/tool_utils.DebugCanvas.o"
"${CMAKE_CURRENT_LIST_DIR}/./contrib/skia/out/Static/obj/tools/debugger/tool_utils.DebugLayerManager.o"
"${CMAKE_CURRENT_LIST_DIR}/./contrib/skia/out/Static/obj/tools/debugger/tool_utils.DrawCommand.o"
"${CMAKE_CURRENT_LIST_DIR}/./contrib/skia/out/Static/obj/tools/debugger/tool_utils.JsonWriteBuffer.o"
"${CMAKE_CURRENT_LIST_DIR}/./contrib/skia/out/Static/obj/tools/fonts/tool_utils.FontToolUtils.o"
"${CMAKE_CURRENT_LIST_DIR}/./contrib/skia/out/Static/obj/tools/fonts/tool_utils.RandomScalerContext.o"
"${CMAKE_CURRENT_LIST_DIR}/./contrib/skia/out/Static/obj/tools/fonts/tool_utils.TestFontMgr.o"
"${CMAKE_CURRENT_LIST_DIR}/./contrib/skia/out/Static/obj/tools/fonts/tool_utils.TestSVGTypeface.o"
"${CMAKE_CURRENT_LIST_DIR}/./contrib/skia/out/Static/obj/tools/fonts/tool_utils.TestTypeface.o"
"${CMAKE_CURRENT_LIST_DIR}/./contrib/skia/out/Static/obj/tools/timer/tool_utils.Timer.o"
"${CMAKE_CURRENT_LIST_DIR}/./contrib/skia/out/Static/obj/tools/tool_utils.SvgPathExtractor.o"
"${CMAKE_CURRENT_LIST_DIR}/./contrib/skia/out/Static/obj/tools/tool_utils.CrashHandler.o"
"${CMAKE_CURRENT_LIST_DIR}/./contrib/skia/out/Static/obj/tools/tool_utils.CrashHandler.o"
"${CMAKE_CURRENT_LIST_DIR}/./contrib/skia/out/Static/obj/tools/trace/trace.ChromeTracingTracer.o"
"${CMAKE_CURRENT_LIST_DIR}/./contrib/skia/out/Static/obj/tools/trace/trace.EventTracingPriv.o"
"${CMAKE_CURRENT_LIST_DIR}/./contrib/skia/out/Static/obj/tools/trace/trace.SkDebugfTracer.o"
"${CMAKE_CURRENT_LIST_DIR}/./contrib/skia/out/Static/obj/tools/trace/trace.SkPerfettoTrace.o"
"${CMAKE_CURRENT_LIST_DIR}/./contrib/skia/out/Static/libsvg.a"
"${CMAKE_CURRENT_LIST_DIR}/./contrib/skia/out/Static/libskia.a"
"${CMAKE_CURRENT_LIST_DIR}/./contrib/skia/out/Static/libskshaper.a"
"${CMAKE_CURRENT_LIST_DIR}/./contrib/skia/out/Static/libskparagraph.a"
"${CMAKE_CURRENT_LIST_DIR}/./contrib/skia/out/Static/libskunicode.a"
"${CMAKE_CURRENT_LIST_DIR}/./contrib/skia/out/Static/libwindow.a"
"${CMAKE_CURRENT_LIST_DIR}/./contrib/skia/out/Static/libcompression_utils_portable.a"
"${CMAKE_CURRENT_LIST_DIR}/./contrib/skia/out/Static/libdng_sdk.a"
"${CMAKE_CURRENT_LIST_DIR}/./contrib/skia/out/Static/libexpat.a"
"${CMAKE_CURRENT_LIST_DIR}/./contrib/skia/out/Static/libharfbuzz.a"
"${CMAKE_CURRENT_LIST_DIR}/./contrib/skia/out/Static/libicu.a"
"${CMAKE_CURRENT_LIST_DIR}/./contrib/skia/out/Static/libicu_bidi.a"
"${CMAKE_CURRENT_LIST_DIR}/./contrib/skia/out/Static/libjpeg.a"
"${CMAKE_CURRENT_LIST_DIR}/./contrib/skia/out/Static/libmicrohttpd.a"
"${CMAKE_CURRENT_LIST_DIR}/./contrib/skia/out/Static/libpathkit.a"
"${CMAKE_CURRENT_LIST_DIR}/./contrib/skia/out/Static/libperfetto.a"
"${CMAKE_CURRENT_LIST_DIR}/./contrib/skia/out/Static/libpiex.a"
"${CMAKE_CURRENT_LIST_DIR}/./contrib/skia/out/Static/libpng.a")


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
$<TARGET_OBJECTS:skia>
$<TARGET_OBJECTS:skia_imported>
$<TARGET_OBJECTS:tracyDisabled>)

