let lib = ./lib.dhall
let sourceTreePart = lib.sourceTreePart
let tracyEnabled = let dir = "../../contrib/tracy/public" in sourceTreePart::{
	, name = "tracyEnabled"
	, dir = dir
	, defines = { 
		, local = [] : List Text
		, global = [ 
			, "TRACY_ENABLE"
			-- , "TRACY_ON_DEMAND"
		]
	}
	, sources = [
		, "${dir}/TracyClient.cpp"
	] : List Text
	, additionalDependants = [
	] : List Text
	, cxxflags = {
		, global = [
		] : List Text
		, local = [] : List Text
	}
	, ldflags = {
		, global = [
		] : List Text
	}
}
let tracyDisabled = tracyEnabled // {
	, name = "tracyDisabled"
	, defines = { 
		, local = [] : List Text
		, global = [] : List Text
	}
}
let imgui = let dir = "./imgui" in sourceTreePart::{
	, name = "imgui"
	, dir = dir
	, sources = [
		, "${dir}/imgui.cpp"
		, "${dir}/imgui_demo.cpp"
		, "${dir}/imgui_draw.cpp"
		, "${dir}/imgui_tables.cpp"
		, "${dir}/imgui_widgets.cpp"
	]
	, additionalIncludeDirs = [
		, "${dir}/../skia"
		, tracyEnabled.dir
	] : List Text
	, cxxflags = {
		, global = [
		] : List Text
		, local = [] : List Text
	}
	, ldflags = {
		, global = [
		] : List Text
	}
}
let imguiBackendGlfw = let dir = "./imgui" in sourceTreePart::{
	, name = "imguiBackendGlfw"
	, dir = dir
	, sources = [
		, "${dir}/imgui_impl_glfw.cpp"
		, "${dir}/imgui_impl_opengl3.cpp"
	]
	, cxxflags = {
		, global = [
		] : List Text
		, local = [
			, "`pkg-config --static --cflags glfw3`"
		] : List Text
	}
	, ldflags = {
		, global = [
			, "-lGL"
			, "`pkg-config --static --libs glfw3`"
		]
	}
}
let imguiFreetype = let dir = "./imgui/misc/freetype" in sourceTreePart::{
	, name = "imguiFreetype"
	, dir = dir
	, defines = { 
		, local = [] : List Text
		, global = [ "IMGUI_ENABLE_FREETYPE" ]
	}
	, sources = [
		, "${dir}/imgui_freetype.cpp"
	]
	, cxxflags = {
		, global = [
		] : List Text
		, local = [
			, "`pkg-config --cflags freetype2`"
		] : List Text
	}
	, ldflags = {
		, global = [
			, "`pkg-config --libs freetype2`"
		] : List Text
	}
}
let marshalling = let dir = "./src/marshalling" in sourceTreePart::{
	, name = "marshalling"
	, dir = dir
	, sources = [
		, "${dir}/receive.cpp"
		, "${dir}/send.cpp"
		, "${dir}/helper.cpp"
	]
	, cxxflags = {
		, global = [
		] : List Text
		, local = [] : List Text
	}
	, ldflags = {
		, global = [
		] : List Text
	}
}
let arena = let dir = "./src/arena/simple" in sourceTreePart::{
	, name = "arena"
	, dir = dir
	, sources = [
		, "${dir}/simple.cpp"
	]
	, cxxflags = {
		, global = [
		] : List Text
		, local = [] : List Text
	}
	, ldflags = {
		, global = [
		] : List Text
	}
}
let widgets = let dir = "./src/widgets" in sourceTreePart::{
	, name = "widgets"
	, dir = dir
	, additionalIncludeDirs = [imgui.dir]
	, sources = [
		, "${dir}/common.cpp"
		, "${dir}/piemenu.cpp"
		, "${dir}/coloredbutton.cpp"
		, "${dir}/splitter.cpp"
	]
	, cxxflags = {
		, global = [
		] : List Text
		, local = [
			, "-Wno-unused-parameter"
			, "-Wno-unused-variable"
		] : List Text
	}
	, ldflags = {
		, global = [
		] : List Text
	}
}
let imguiToggle = let dir = "./src/widgets/imgui_toggle" in sourceTreePart::{
	, name = "imguiToggle"
	, additionalIncludeDirs = [imgui.dir]
	, dir = dir
	, sources = [
		, "${dir}/imgui_toggle.cpp"
		, "${dir}/imgui_toggle_palette.cpp"
		, "${dir}/imgui_toggle_presets.cpp"
		, "${dir}/imgui_toggle_renderer.cpp"
	]
	, cxxflags = {
		, global = [
		] : List Text
		, local = [
			, "-Wno-unused-parameter"
			, "-Wno-unused-variable"
		] : List Text
	}
	, ldflags = {
		, global = [
		] : List Text
	}
}
let imguiImplot = let dir = "./src/widgets/imgui_implot" in sourceTreePart::{
	, name = "imguiImplot"
	, dir = dir
	, additionalIncludeDirs = [imgui.dir]
	, sources = [
		, "${dir}/implot.cpp"
		, "${dir}/implot_demo.cpp"
		, "${dir}/implot_items.cpp"
	]
	, cxxflags = {
		, global = [
		] : List Text
		, local = [] : List Text
	}
	, ldflags = {
		, global = [
		] : List Text
	}
}
let imguiKnobs = let dir = "./src/widgets/imgui_knobs" in sourceTreePart::{
	, name = "imguiKnobs"
	, dir = dir
	, additionalIncludeDirs = [imgui.dir]
	, sources = [
		, "${dir}/imgui-knobs.cpp"
	]
	, cxxflags = {
		, global = [
		] : List Text
		, local = [] : List Text
	}
	, ldflags = {
		, global = [
		] : List Text
	}
}
let imguiCoolbar = let dir = "./src/widgets/imgui_coolbar" in sourceTreePart::{
	, name = "imguiCoolbar"
	, dir = dir
	, additionalIncludeDirs = [imgui.dir]
	, sources = [
		, "${dir}/ImCoolbar.cpp"
	]
	, cxxflags = {
		, global = [
		] : List Text
		, local = [] : List Text
	}
	, ldflags = {
		, global = [
		] : List Text
	}
}
let imguiFlamegraph = let dir = "./src/widgets/imgui_flamegraph" in sourceTreePart::{
	, name = "imguiFlamegraph"
	, dir = dir
	, additionalIncludeDirs = [imgui.dir]
	, sources = [
		, "${dir}/imgui_widget_flamegraph.cpp"
	]
	, cxxflags = {
		, global = [
		] : List Text
		, local = [] : List Text
	}
	, ldflags = {
		, global = [
		] : List Text
	}
}
let imguiTextedit = let dir = "./imcolortextedit" in sourceTreePart::{
	, name = "imguiTextedit"
	, dir = dir
	, additionalIncludeDirs = [imgui.dir]
	, sources = [
		, "${dir}/TextEditor.cpp"
	]
	, cxxflags = {
		, global = [
		] : List Text
		, local = [
			"-Wno-unused-variable"
		] : List Text
	}
	, ldflags = {
		, global = [
		] : List Text
	}
}
let render = let dir = "./src" in sourceTreePart::{
	, name = "render"
	, dir = dir
	, additionalIncludeDirs = [
		, imguiImplot.dir
		, imguiTextedit.dir
	]
	, sources = [
		, "${dir}/render.cpp"
	]
	, cxxflags = {
		, global = [
		] : List Text
		, local = [] : List Text
	}
	, ldflags = {
		, global = [
		] : List Text
	}
}
let binding = let dir = "./src/binding" in sourceTreePart::{
	, name = "binding"
	, dir = dir
	, sources = [] : List Text
	, additionalDependants = [
		, "${dir}/imgui/dispatch.h"
		, "${dir}/implot/dispatch.h"
		, "${dir}/imcolortextedit/dispatch.h"
	]
	, cxxflags = {
		, global = [
		] : List Text
		, local = [] : List Text
	}
	, ldflags = {
		, global = [
		] : List Text
	}
}
let skia = 
    let dir = "./skia"
    let contribDir = "./contrib/skia"
	let objDir = "${contribDir}/out/Static/obj"
	let static3rdPartyLibraries = [
		-- static libaries (offial_build=false mode)
		, "${contribDir}/out/Static/libcompression_utils_portable.a"
		, "${contribDir}/out/Static/libdng_sdk.a"
		, "${contribDir}/out/Static/libexpat.a"
		, "${contribDir}/out/Static/libharfbuzz.a"
		, "${contribDir}/out/Static/libicu.a"
		, "${contribDir}/out/Static/libicu_bidi.a"
		, "${contribDir}/out/Static/libjpeg.a"
		, "${contribDir}/out/Static/libmicrohttpd.a"
		, "${contribDir}/out/Static/libpathkit.a"
		, "${contribDir}/out/Static/libperfetto.a"
		, "${contribDir}/out/Static/libpiex.a"
		, "${contribDir}/out/Static/libpng.a"
	]
    in sourceTreePart::{
	, name = "skia"
	, dir = dir
	, sources = [
		, "${dir}/paragraph.cpp"
		, "${dir}/app.cpp"
		, "${dir}/ImGuiLayer.cpp"
		, "${dir}/vectorCmdSkiaRenderer.cpp"
		, "${dir}/skiaTracyTracer.cpp"
	]
	, additionalIncludeDirs = [
		, "${contribDir}/modules/sksg/include"
		, "${contribDir}/modules/bentleyottmann/include"
		, "${contribDir}/modules/skottie/include"
		, "${contribDir}/modules/skparagraph/include"
		, "${contribDir}/modules/skplaintexteditor/include"
		, "${contribDir}/modules/skresources/include"
		, "${contribDir}/modules/skshaper/include"
		, "${contribDir}/modules/skunicode/include"
		, "${contribDir}/modules/svg/include"
		--, "${contribDir}/experimental/sktext/include"
		, "${contribDir}/include"
		, "${contribDir}/include/core"
		, "${contribDir}"
		, render.dir
	]
	, defines = {, local = [
        , "SK_TRIVIAL_ABI=\\[\\[clang::trivial_abi\\]\\]"
        , "SK_GAMMA_APPLY_TO_A8"
        , "SK_ALLOW_STATIC_GLOBAL_INITIALIZERS=1"
        , "GR_TEST_UTILS=1"
        , "SK_TYPEFACE_FACTORY_FREETYPE"
        , "SK_FONTMGR_ANDROID_AVAILABLE"
        , "SK_FONTMGR_FREETYPE_DIRECTORY_AVAILABLE"
        , "SK_FONTMGR_FREETYPE_EMBEDDED_AVAILABLE"
        , "SK_FONTMGR_FREETYPE_EMPTY_AVAILABLE"
        , "SK_FONTMGR_FONTCONFIG_AVAILABLE"
        , "SK_GL"
        , "SK_SUPPORT_PDF"
        , "SK_CODEC_DECODES_JPEG"
        , "SK_CODEC_DECODES_JPEG_GAINMAPS"
        , "SK_XML"
        , "SK_ENABLE_ANDROID_UTILS"
        , "SK_HAS_HEIF_LIBRARY"
        , "SK_CODEC_DECODES_PNG"
        , "SK_CODEC_DECODES_RAW"
        , "SK_CODEC_DECODES_WEBP"
        , "SK_HAS_WUFFS_LIBRARY"
        , "SK_DEFAULT_TYPEFACE_IS_EMPTY"
        , "SK_DISABLE_LEGACY_DEFAULT_TYPEFACE"
        , "SK_R32_SHIFT=16"
        , "SK_ENABLE_PRECOMPILE"
        , "SKSL_ENABLE_TRACING"
        , "SK_GANESH"
        , "SK_USE_PERFETTO"
        , "SK_ENABLE_SKOTTIE"
        , "SK_ENABLE_SKOTTIE_SKSLEFFECT"
        , "SK_ENABLE_PARAGRAPH"
        , "SK_UNICODE_AVAILABLE"
        , "SK_UNICODE_ICU_IMPLEMENTATION"
        , "SK_SHAPER_PRIMITIVE_AVAILABLE"
        , "SK_SHAPER_HARFBUZZ_AVAILABLE"
        , "SK_SHAPER_UNICODE_AVAILABLE"
        , "SK_ENABLE_SVG"
        , "SK_BUILD_FOR_UNIX"
	], global = [
		--, "IMGUI_USE_BGRA_PACKED_COLOR"
	] : List Text}
	, cxxflags = {
		, global = [
		] : List Text
		, local = [
		, "-Wno-unused-parameter"
		] : List Text
	}
	, ldflags = {
		, global = [
			, "-ldl"
			, "-lpthread"
			, "-lfreetype"
			, "-lz"
			, "-lglfw"
			, "-lfontconfig"
			, "-lwebpmux"
			, "-lwebpdemux"
			, "-lX11"
			, "-lGLU"
			, "-lGL"
			--, "-Wl,--verbose"
		] : List Text
	}
	, nonSourceObjs = [
		-- extracted from HelloWorld.rsp
        , "${objDir}/tools/flags/flags.CommandLineFlags.o"
        , "${objDir}/tools/gpu/gpu_tool_utils.BackendSurfaceFactory.o"
        , "${objDir}/tools/gpu/gpu_tool_utils.BackendTextureImageFactory.o"
        , "${objDir}/tools/gpu/gpu_tool_utils.ContextType.o"
        , "${objDir}/tools/gpu/gpu_tool_utils.FlushFinishTracker.o"
        , "${objDir}/tools/gpu/gpu_tool_utils.GrContextFactory.o"
        , "${objDir}/tools/gpu/gpu_tool_utils.GrTest.o"
        , "${objDir}/tools/gpu/gpu_tool_utils.ManagedBackendTexture.o"
        , "${objDir}/tools/gpu/gpu_tool_utils.MemoryCache.o"
        , "${objDir}/tools/gpu/gpu_tool_utils.ProtectedUtils.o"
        , "${objDir}/tools/gpu/gpu_tool_utils.ProxyUtils.o"
        , "${objDir}/tools/gpu/gpu_tool_utils.TestContext.o"
        , "${objDir}/tools/gpu/gpu_tool_utils.TestOps.o"
        , "${objDir}/tools/gpu/gpu_tool_utils.YUVUtils.o"
        , "${objDir}/tools/gpu/mock/gpu_tool_utils.MockTestContext.o"
        , "${objDir}/src/utils/gpu_tool_utils.SkTestCanvas.o"
        , "${objDir}/tools/gpu/gl/gpu_tool_utils.GLTestContext.o"
        , "${objDir}/tools/gpu/gl/glx/gpu_tool_utils.CreatePlatformGLTestContext_glx.o"
        , "${objDir}/tools/sk_app/sk_app.CommandSet.o"
        , "${objDir}/tools/sk_app/sk_app.Window.o"
        , "${objDir}/tools/sk_app.SkGetExecutablePath_linux.o"
        , "${objDir}/tools/sk_app/unix/sk_app.Window_unix.o"
        , "${objDir}/tools/sk_app/unix/sk_app.keysym2ucs.o"
        , "${objDir}/tools/sk_app/unix/sk_app.main_unix.o"
        , "${objDir}/tools/tool_utils.AndroidSkDebugToStdOut.o"
        , "${objDir}/tools/tool_utils.DDLPromiseImageHelper.o"
        , "${objDir}/tools/tool_utils.DDLTileHelper.o"
        , "${objDir}/tools/tool_utils.DecodeUtils.o"
        , "${objDir}/tools/tool_utils.EncodeUtils.o"
        , "${objDir}/tools/tool_utils.GpuToolUtils.o"
        , "${objDir}/tools/tool_utils.LsanSuppressions.o"
        , "${objDir}/tools/tool_utils.MSKPPlayer.o"
        , "${objDir}/tools/tool_utils.ProcStats.o"
        , "${objDir}/tools/tool_utils.Resources.o"
        , "${objDir}/tools/tool_utils.RuntimeBlendUtils.o"
        , "${objDir}/tools/tool_utils.SkMetaData.o"
        , "${objDir}/tools/tool_utils.SkSharingProc.o"
        , "${objDir}/tools/tool_utils.TestFontDataProvider.o"
        , "${objDir}/tools/tool_utils.ToolUtils.o"
        , "${objDir}/tools/tool_utils.UrlDataManager.o"
        , "${objDir}/tools/debugger/tool_utils.DebugCanvas.o"
        , "${objDir}/tools/debugger/tool_utils.DebugLayerManager.o"
        , "${objDir}/tools/debugger/tool_utils.DrawCommand.o"
        , "${objDir}/tools/debugger/tool_utils.JsonWriteBuffer.o"
        , "${objDir}/tools/fonts/tool_utils.FontToolUtils.o"
        , "${objDir}/tools/fonts/tool_utils.RandomScalerContext.o"
        , "${objDir}/tools/fonts/tool_utils.TestFontMgr.o"
        , "${objDir}/tools/fonts/tool_utils.TestSVGTypeface.o"
        , "${objDir}/tools/fonts/tool_utils.TestTypeface.o"
        , "${objDir}/tools/timer/tool_utils.Timer.o"
        , "${objDir}/tools/tool_utils.SvgPathExtractor.o"
        , "${objDir}/tools/tool_utils.CrashHandler.o"
        , "${objDir}/tools/tool_utils.CrashHandler.o"
	, "${objDir}/tools/trace/trace.ChromeTracingTracer.o"
	, "${objDir}/tools/trace/trace.EventTracingPriv.o"
	, "${objDir}/tools/trace/trace.SkDebugfTracer.o"
	, "${objDir}/tools/trace/trace.SkPerfettoTrace.o"
        , "${contribDir}/out/Static/libsvg.a"
        , "${contribDir}/out/Static/libskia.a"
        , "${contribDir}/out/Static/libskshaper.a"
        , "${contribDir}/out/Static/libskparagraph.a"
        , "${contribDir}/out/Static/libskunicode.a"
        , "${contribDir}/out/Static/libwindow.a"
	] # static3rdPartyLibraries
}
let imguiWithSkia = imgui // {
	, name = "imguiWithSkia"
	, additionalIncludeDirs = imgui.additionalIncludeDirs # skia.additionalIncludeDirs # [
		, "${imgui.dir}/../../../contrib/flatbuffers/include"
		]
	, sources = imgui.sources # [ "${imgui.dir}/flatbufferHelpers.cpp" ]
	}
in
{
	, imgui
	, imguiWithSkia
	, imguiBackendGlfw
	, imguiFreetype
	, render
	, marshalling
	, arena
	, widgets
	, imguiToggle
	, imguiImplot
	, imguiKnobs
	, imguiCoolbar
	, imguiFlamegraph
	, imguiTextedit
	, binding
	, skia
	, tracyEnabled
	, tracyDisabled
}
