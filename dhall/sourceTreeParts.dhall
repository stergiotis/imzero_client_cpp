let lib = ./lib.dhall
let sourceTreePart = lib.sourceTreePart
let tracyEnabled = let dir = "./contrib/tracy/public" in sourceTreePart::{
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
	, includeDirs = {
		, global = ["${dir}"] : List Text
		, local = [] : List Text
	}
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
	, includeDirs = {
		, local = [] : List Text
		, global = ["${dir}"] : List Text
	}
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
			, env:PKG_CONFIG_OUTPUT_CFLAGS_GLFW3 as Text
		] : List Text
	}
	, ldflags = {
		, global = [
			, "-lGL"
			, env:PKG_CONFIG_OUTPUT_LIBS_GLFW3 as Text
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
			, env:PKG_CONFIG_OUTPUT_CFLAGS_FREETYPE2 as Text
		] : List Text
	}
	, ldflags = {
		, global = [
			, env:PKG_CONFIG_OUTPUT_LIBS_FREETYPE2 as Text
		] : List Text
	}
}
let imguiImplot = let dir = "./src/widgets/imgui_implot" in sourceTreePart::{
	, name = "imguiImplot"
	, dir = dir
	, includeDirs = {
		, local = [] : List Text
		, global = [] : List Text
	}
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
let marshalling = let dir = "./src/marshalling" in sourceTreePart::{
	, name = "marshalling"
	, dir = dir
	, includeDirs = {
		, local = [imguiImplot.dir] : List Text
		, global = [] : List Text
	}
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
	, includeDirs = {
		, local = [] : List Text
		, global = ["${dir}"] : List Text
	}
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
	, includeDirs = {
		, local = [] : List Text
		, global = [] : List Text
	}  
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
	, includeDirs = {
		, local = [] : List Text
		, global = [] : List Text
	}
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
let imguiKnobs = let dir = "./src/widgets/imgui_knobs" in sourceTreePart::{
	, name = "imguiKnobs"
	, dir = dir
	, includeDirs = {
		, local = [] : List Text
		, global = [] : List Text
	}
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
	, includeDirs = {
		, local = [] : List Text
		, global = [] : List Text
	}
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
	, includeDirs = {
		, local = ["${dir}"] : List Text
		, global = [] : List Text
	}
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
	, includeDirs = {
		, local = [] : List Text
		, global = [] : List Text
	}
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
	, includeDirs = {
		, local = [
			, imguiImplot.dir
			, imguiTextedit.dir
		] : List Text
		, global = [] : List Text
	}
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
		, "${objDir}/../libcompression_utils_portable.a"
		, "${objDir}/../libdng_sdk.a"
		, "${objDir}/../libexpat.a"
		, "${objDir}/../libharfbuzz.a"
		, "${objDir}/../libicu.a"
		, "${objDir}/../libicu_bidi.a"
		, "${objDir}/../libjpeg.a"
		, "${objDir}/../libmicrohttpd.a"
		, "${objDir}/../libpathkit.a"
		, "${objDir}/../libperfetto.a"
		, "${objDir}/../libpiex.a"
		, "${objDir}/../libpng.a"
	]
    in sourceTreePart::{
	, name = "skia"
	, dir = dir
	, sources = [
		, "${dir}/modified/app.cpp"
		, "${dir}/modified/ImGuiLayer.cpp"
		, "${dir}/paragraph.cpp"
		, "${dir}/cliOptions.cpp"
		, "${dir}/setupUI.cpp"
		, "${dir}/vectorCmdSkiaRenderer.cpp"
		, "${dir}/skiaTracyTracer.cpp"
	]
	, includeDirs = {
		, local = [
			, imgui.dir
			, imguiImplot.dir
			, render.dir]
		, global = [
			, "${contribDir}"
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
		] : List Text
	}
	, defines = {, local = [
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
         , "-ffp-contract=off" -- standard compliant fp processing
         , "-fstrict-aliasing" -- is on on >=O2 optimization
         , "-fPIC"
         , "-fvisibility=hidden"
         , "-fdata-sections"
         , "-ffunction-sections"
         , "-fvisibility-inlines-hidden"
         , "-fno-exceptions"
         , "-fno-rtti"
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
        , "${objDir}/../libsvg.a"
        , "${objDir}/../libskia.a"
        , "${objDir}/../libskshaper.a"
        , "${objDir}/../libskparagraph.a"
        , "${objDir}/../libskunicode.a"
        , "${objDir}/../libwindow.a"
	] # static3rdPartyLibraries
}
let skiaVideo = \(asan : Bool) -> 
    let dir = "./skia"
    let contribDir = "./contrib/skia"
	let objDir = if asan then "${contribDir}/out/asan/obj" else "${contribDir}/out/Static/obj"
	let static3rdPartyLibraries = [
		-- static libaries (offial_build=false mode)
		, "${objDir}/../libcompression_utils_portable.a"
		, "${objDir}/../libdng_sdk.a"
		, "${objDir}/../libexpat.a"
		, "${objDir}/../libharfbuzz.a"
		, "${objDir}/../libicu.a"
		, "${objDir}/../libicu_bidi.a"
		, "${objDir}/../libjpeg.a"
		, "${objDir}/../libmicrohttpd.a"
		, "${objDir}/../libpathkit.a"
		, "${objDir}/../libperfetto.a"
		, "${objDir}/../libpiex.a"
		, "${objDir}/../libpng.a"
	]
    in sourceTreePart::{
	, name = "skiaVideo"
	, dir = dir
	, sources = [
		, "${dir}/video/main.cpp"
		, "${dir}/video/app.cpp"

		, "${dir}/paragraph.cpp"
		, "${dir}/cliOptions.cpp"
		, "${dir}/setupUI.cpp"
		, "${dir}/vectorCmdSkiaRenderer.cpp"
		, "${dir}/skiaTracyTracer.cpp"
		-- FIXME
		, "${contribDir}/src/gpu/ganesh/gl/GrGLInterfaceAutogen.cpp"
		, "${contribDir}/src/gpu/ganesh/gl/GrGLUtil.cpp"
		-- FIXME
		--, "skia/video/SkFontMgr_custom_embedded.cpp"
	]
	, includeDirs = {
		, local = [
			, imgui.dir
			, imguiImplot.dir
			, render.dir
			]
		, global = [
			, "${contribDir}"
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
		] : List Text
	}
	, defines = {, local = [
		      , "SK_DEBUG"
        , "SK_GAMMA_APPLY_TO_A8"
        , "SK_ALLOW_STATIC_GLOBAL_INITIALIZERS=1"
        --, "GR_TEST_UTILS=1"
        , "SK_TYPEFACE_FACTORY_FREETYPE"
        --, "SK_FONTMGR_ANDROID_AVAILABLE"
        --, "SK_FONTMGR_FREETYPE_DIRECTORY_AVAILABLE"
        , "SK_FONTMGR_FREETYPE_EMBEDDED_AVAILABLE"
        , "SK_FONTMGR_FREETYPE_EMPTY_AVAILABLE"
        --, "SK_FONTMGR_FONTCONFIG_AVAILABLE"
        , "SK_GL"
        , "SK_SUPPORT_PDF"
        , "SK_CODEC_DECODES_JPEG"
        , "SK_CODEC_DECODES_JPEG_GAINMAPS"
        , "SK_XML"
        --, "SK_ENABLE_ANDROID_UTILS"
        , "SK_HAS_HEIF_LIBRARY"
        , "SK_CODEC_DECODES_PNG"
        , "SK_CODEC_DECODES_RAW"
        , "SK_CODEC_DECODES_WEBP"
        , "SK_HAS_WUFFS_LIBRARY"
        , "SK_DEFAULT_TYPEFACE_IS_EMPTY"
        , "SK_DISABLE_LEGACY_DEFAULT_TYPEFACE"
        , "SK_R32_SHIFT=16"
        , "SK_ENABLE_PRECOMPILE"
        --, "SKSL_ENABLE_TRACING"
        , "SK_GANESH"
        --, "SK_USE_PERFETTO"
        --, "SK_ENABLE_SKOTTIE"
        --, "SK_ENABLE_SKOTTIE_SKSLEFFECT"
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
        , "${objDir}/../libskparagraph.a"
        , "${objDir}/../libsvg.a"
        , "${objDir}/../libskia.a"
        , "${objDir}/../libskshaper.a"
        , "${objDir}/../libskunicode.a"
        , "${objDir}/../libwindow.a"
	] # static3rdPartyLibraries
}
let skiaSdl = \(asan : Bool) -> 
    let dir = "./skia"
    let contribDir = "./contrib/skia"
	let objDir = if asan then "${contribDir}/out/asan/obj" else "${contribDir}/out/Static/obj"
	let static3rdPartyLibraries = [
		-- static libaries (offial_build=false mode)
		, "${objDir}/../libcompression_utils_portable.a"
		, "${objDir}/../libdng_sdk.a"
		, "${objDir}/../libexpat.a"
		, "${objDir}/../libharfbuzz.a"
		, "${objDir}/../libicu.a"
		, "${objDir}/../libicu_bidi.a"
		, "${objDir}/../libjpeg.a"
		, "${objDir}/../libmicrohttpd.a"
		, "${objDir}/../libpathkit.a"
		, "${objDir}/../libperfetto.a"
		, "${objDir}/../libpiex.a"
		, "${objDir}/../libpng.a"
	]
    in sourceTreePart::{
	, name = "skiaSdl"
	, dir = dir
	, sources = [
		, "${dir}/sdl3/imgui_impl_opengl3.cpp"
		, "${dir}/sdl3/imgui_impl_sdl3.cpp"
		, "${dir}/sdl3/main.cpp"
		, "${dir}/sdl3/app.cpp"

		, "${dir}/paragraph.cpp"
		, "${dir}/cliOptions.cpp"
		, "${dir}/setupUI.cpp"
		, "${dir}/vectorCmdSkiaRenderer.cpp"
		, "${dir}/skiaTracyTracer.cpp"
		-- FIXME
		, "${contribDir}/src/gpu/ganesh/gl/GrGLInterfaceAutogen.cpp"
		, "${contribDir}/src/gpu/ganesh/gl/GrGLUtil.cpp"
		-- FIXME
		--, "skia/sdl3/SkFontMgr_custom_embedded.cpp"
	]
	, includeDirs = {
		, local = [
			, imgui.dir
			, imguiImplot.dir
			, render.dir
			, "./contrib/sdl3/include"
			]
		, global = [
			, "${contribDir}"
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
		] : List Text
	}
	, defines = {, local = [
		      , "SK_DEBUG"
        , "SK_GAMMA_APPLY_TO_A8"
        , "SK_ALLOW_STATIC_GLOBAL_INITIALIZERS=1"
        --, "GR_TEST_UTILS=1"
        , "SK_TYPEFACE_FACTORY_FREETYPE"
        --, "SK_FONTMGR_ANDROID_AVAILABLE"
        --, "SK_FONTMGR_FREETYPE_DIRECTORY_AVAILABLE"
        , "SK_FONTMGR_FREETYPE_EMBEDDED_AVAILABLE"
        , "SK_FONTMGR_FREETYPE_EMPTY_AVAILABLE"
        --, "SK_FONTMGR_FONTCONFIG_AVAILABLE"
        , "SK_GL"
        , "SK_SUPPORT_PDF"
        , "SK_CODEC_DECODES_JPEG"
        , "SK_CODEC_DECODES_JPEG_GAINMAPS"
        , "SK_XML"
        --, "SK_ENABLE_ANDROID_UTILS"
        , "SK_HAS_HEIF_LIBRARY"
        , "SK_CODEC_DECODES_PNG"
        , "SK_CODEC_DECODES_RAW"
        , "SK_CODEC_DECODES_WEBP"
        , "SK_HAS_WUFFS_LIBRARY"
        , "SK_DEFAULT_TYPEFACE_IS_EMPTY"
        , "SK_DISABLE_LEGACY_DEFAULT_TYPEFACE"
        , "SK_R32_SHIFT=16"
        , "SK_ENABLE_PRECOMPILE"
        --, "SKSL_ENABLE_TRACING"
        , "SK_GANESH"
        --, "SK_USE_PERFETTO"
        --, "SK_ENABLE_SKOTTIE"
        --, "SK_ENABLE_SKOTTIE_SKSLEFFECT"
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
        , "${objDir}/../libskparagraph.a"
        , "${objDir}/../libsvg.a"
        , "${objDir}/../libskia.a"
        , "${objDir}/../libskshaper.a"
        , "${objDir}/../libskunicode.a"
        , "${objDir}/../libwindow.a"
		, "./contrib/sdl3/build/libSDL3.a"
	] # static3rdPartyLibraries
}
let flatbuffers = let dir = "./contrib/flatbuffers" in
 sourceTreePart::{
	, dir = dir
	, name = "flatbuffers"
	, includeDirs = {
		, local = [] : List Text
		, global = ["${dir}"] : List Text
	}
	, sources = [] : List Text
}
let sdl3 = let dir = "./contrib/sdl" in
 sourceTreePart::{
	, dir = dir
	, name = "sdl3"
	, includeDirs = {
		, local = [] : List Text
		, global = ["${dir}/include"] : List Text
	}
	, sources = [] : List Text
	, nonSourceObjs = [
	  , "build/libSDL3.a"
	]
}
let imguiWithSkia = imgui // {
	, name = "imguiWithSkia"
	, includeDirs = {
		, local = imgui.includeDirs.local # skia.includeDirs.local
		, global = imgui.includeDirs.global
	}
	, sources = [
		, "${imgui.dir}/imgui.cpp"
		, "${imgui.dir}/imgui_demo.cpp"
		, "${imgui.dir}/imgui_draw_fb.cpp"
		, "${imgui.dir}/imgui_tables.cpp"
		, "${imgui.dir}/imgui_widgets.cpp"
		, "${imgui.dir}/flatbufferHelpers.cpp" ]
	}
in
{
	, flatbuffers
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
	, skiaSdl
	, skiaVideo
	, tracyEnabled
	, tracyDisabled
	, sdl3
}
