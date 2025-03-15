let lib = ./lib.dhall
let prelude = ./prelude.dhall
let sourceTreePart = lib.sourceTreePart
let locationToString = \(loc : prelude.Location.Type) -> (merge {
	, Environment = \(t : Text) -> "UNDEFINED LOCATION"
	, Local = \(t : Text) -> t
	, Missing = "MISSING LOCATION"
	, Remote = \(t : Text) -> "REMOTE LOCATION"
} loc)
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
	, defines = {
		, local = [
	          -- , "IMGUI_DISABLE_OBSOLETE_FUNCTIONS"
		] : List Text
		, global = [] : List Text
		}
}
let ImGuiAppHelper = <SDL3>
let imguiWithHooks1919Wip = \(apph : ImGuiAppHelper) -> let dir = "./imgui_w_hooks_1.91.9_wip" in sourceTreePart::{
	, name = "imguiWithHooks1919Wip"
	, dir = dir
	, sources = [
		, "${dir}/imgui.cpp"
		, "${dir}/imgui_demo.cpp"
		, "${dir}/imgui_draw.cpp"
		, "${dir}/imgui_tables.cpp"
		, "${dir}/imgui_widgets.cpp"
		, "${dir}/imgui_impl_sdl3.cpp"
	] # (merge {
           SDL3 = [ "${dir}/imgui_impl_sdl3.cpp" ]
        } apph)
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
	, defines = {
		, local = [
	          -- , "IMGUI_DISABLE_OBSOLETE_FUNCTIONS"
		] : List Text
		, global = [] : List Text
		}
}
let imguiSkiaImpl = let dir = "./imgui_skia_impl" in sourceTreePart::{
	, name = "imguiSkiaImpl"
	, dir = dir
	, sources = [
		, "${dir}/imgui_skia_extensions.cpp"
		, "${dir}/imgui_skia_hooks_impl.cpp"
		, "${dir}/imgui_skia_imzero_cmd_render.cpp"
		, "${dir}/imgui_skia_paragraph.cpp"
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
	, defines = {
		, local = [] : List Text
		, global = [] : List Text
		}
}
let imguiSkia = let dir = "./imgui_skia" in sourceTreePart::{
	, name = "imguiSkia"
	, dir = dir
	, sources = [
		, "${dir}/imgui_skia_extensions.cpp"
		, "${dir}/imgui_skia_hooks_impl.cpp"
		, "${dir}/imgui_skia_paragraph.cpp"
		, "${dir}/imgui_skia_imzero_cmd_render.cpp"
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
	, defines = {
		, local = [] : List Text
		, global = [] : List Text
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
		, "${dir}/LanguageDefinitions.cpp"
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
let skiaShared = 
    let skiaSharedBaseDir = "${env:IMZERO_CLIENT_CPP_ROOT as Text}/${locationToString (../../contrib/skia as Location)}"
    let dir = "./skia"
    let contribDir = "./contrib/skia"
	let objDir = "${contribDir}/out/Shared/obj"
    in sourceTreePart::{
	, name = "skiaShared"
	, dir = dir
	, sources = [] : List Text
	, includeDirs = {
		, local = [] : List Text
		, global = [
			, "${contribDir}"
		] : List Text
	}
	, defines = {, local = [] : List Text
	             , global = [ --, "IMGUI_USE_BGRA_PACKED_COLOR" 
				 -- FIXME extract from rsp
		, "SK_RELEASE"
        , "SK_GAMMA_APPLY_TO_A8"
        , "SK_ALLOW_STATIC_GLOBAL_INITIALIZERS=1"
        , "SK_TYPEFACE_FACTORY_FREETYPE"
        , "SK_FONTMGR_FREETYPE_EMBEDDED_AVAILABLE"
	, "SK_FONTMGR_FONTCONFIG_AVAILABLE"
	, "SK_FONTMGR_FREETYPE_DIRECTORY_AVAILABLE"
        , "SK_FONTMGR_FREETYPE_EMPTY_AVAILABLE"
        , "SK_GL"
        , "SK_SUPPORT_PDF"
        , "SK_CODEC_DECODES_JPEG"
        , "SK_CODEC_DECODES_JPEG_GAINMAPS"
        , "SK_XML"
        , "SK_CODEC_DECODES_PNG"
        , "SK_CODEC_DECODES_RAW"
        , "SK_CODEC_DECODES_WEBP"
        , "SK_DEFAULT_TYPEFACE_IS_EMPTY"
        , "SK_DISABLE_LEGACY_DEFAULT_TYPEFACE"
        , "SK_R32_SHIFT=16"
        , "SK_ENABLE_PRECOMPILE"
        , "SK_GANESH"
        , "SK_ENABLE_PARAGRAPH"
        , "SK_UNICODE_AVAILABLE"
        , "SK_UNICODE_ICU_IMPLEMENTATION"
        , "SK_SHAPER_PRIMITIVE_AVAILABLE"
        , "SK_SHAPER_HARFBUZZ_AVAILABLE"
        , "SK_SHAPER_UNICODE_AVAILABLE"
        , "SK_ENABLE_SVG"
        , "SK_BUILD_FOR_UNIX"
				 ] : List Text}
	, cxxflags = {
		, global = [
         , "-ffp-contract=off" -- standard compliant fp processing
         , "-fstrict-aliasing" -- is on for optimization levels larger than O1
         , "-fPIC"
         , "-fvisibility=hidden"
         , "-fdata-sections"
         , "-ffunction-sections"
         , "-fvisibility-inlines-hidden"
         , "-fno-exceptions"
         , "-fno-rtti"
		] : List Text
		, local = [
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
			, "-L${skiaSharedBaseDir}/out/Shared"
			, "-lskparagraph"
			, "-lskia"
			, "-lskunicode"
			, "-lbentleyottmann"
			, "-lskshaper"
			-- , "-lsvg"
			--, "-Wl,--verbose"
		] : List Text
	}
	, nonSourceObjs = [] : List Text
}
let mainSkiaSdl3Minimal = 
    let dir = "./main/sdl3"
    in sourceTreePart::{
	, name = "mainSkiaSdl3Minimal"
	, dir = dir
	, sources = [
		, "${dir}/main.cpp"
		, "${dir}/app.cpp"
		, "${dir}/../cliOptions.cpp"
		, "${dir}/../setupUI.cpp"
	]
	, includeDirs = {
		, local = [
			, imgui.dir
			, imguiImplot.dir
			, render.dir
                        , "${dir}/.."
			]
		, global = [] : List Text
	}
	, defines = {, local = [] : List Text, global = [
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
			, "-lfontconfig"
			, "-lwebpmux"
			, "-lwebpdemux"
			, "-lX11"
			, "-lGLU"
			, "-lGL"
			--, "-Wl,--verbose"
		] : List Text
	}
	, nonSourceObjs = [ ] : List Text
}
let mainSkiaSdl3 = 
    let dir = "./main/sdl3"
    in sourceTreePart::{
        , executable = True
	, name = "mainSkiaSdl3"
	, dir = dir
	, sources = [
		, "${dir}/../../imgui/imgui_impl_sdl3.cpp"
		, "${dir}/main.cpp"
		, "${dir}/app.cpp"

		, "${dir}/../paragraph.cpp"
		, "${dir}/../cliOptions.cpp"
		, "${dir}/../setupUI.cpp"
		, "${dir}/../vectorCmdSkiaRenderer.cpp"
	]
	, includeDirs = {
		, local = [
			, imgui.dir
			, imguiImplot.dir
			, render.dir
                        , "${dir}/.."
			]
		, global = [] : List Text
	}
	, defines = {, local = [] : List Text, global = [
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
			, "-lfontconfig"
			, "-lwebpmux"
			, "-lwebpdemux"
			, "-lX11"
			, "-lGLU"
			, "-lGL"
			--, "-Wl,--verbose"
		] : List Text
	}
	, nonSourceObjs = [ ] : List Text
}
let mainVideoPlayerSdl3Mpv = 
    let dir = "./sdl3_mpv"
    in sourceTreePart::{
	, name = "mainVideoPlayerSdl3Mpv"
	, dir = dir
	, sources = [
		, "${dir}/main.cpp"
		, "${dir}/events.cpp"
		, "${dir}/app.cpp"
	]
	, includeDirs = {
		, local = [] : List Text
		, global = [] : List Text
	}
	, defines = {, local = [] : List Text, global = [
	] : List Text}
	, cxxflags = {
		, global = [
		] : List Text
		, local = [
		, "-Wno-unused-parameter"
		] : List Text
	}
	, ldflags = {
		, global = [ ] : List Text
	}
	, nonSourceObjs = [ ] : List Text
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

let sdl3Shared = let sdlDir = "${env:IMZERO_CLIENT_CPP_ROOT as Text}/${locationToString (../../contrib/sdl as Location)}"
in
sourceTreePart::{
	, dir = ""
	, name = "sdl3Shared"
	, sources = [] : List Text
	, cxxflags = {
	   , local = [] : List Text
	   , global = [
		, "-I${sdlDir}/include"
		 ] : List Text
	}
	, ldflags = { global = [
		, "-L${sdlDir}/build"
		, "-Wl,--enable-new-dtags"
		, "-lSDL3"
	 ] : List Text }
}
let mpvShared = 
 sourceTreePart::{
	, dir = ""
	, name = "mpvShared"
	, includeDirs = {
		, local = [] : List Text
		, global = [] : List Text
	}
	, sources = [] : List Text
	, ldflags = {
	   , global = [ "-lmpv" ] : List Text
	}
	, nonSourceObjs = [
	] : List Text
}
let imguiWithSkia = imgui // {
	, name = "imguiWithSkia"
	, includeDirs = {
		, local = imgui.includeDirs.local
		, global = imgui.includeDirs.global
	}
	, sources = [
		, "${imgui.dir}/imzero_hooks.cpp"
		, "${imgui.dir}/imzero_extensions.cpp"
		, "${imgui.dir}/imgui.cpp"
		, "${imgui.dir}/imgui_demo.cpp"
		, "${imgui.dir}/imgui_draw.cpp"
		, "${imgui.dir}/imgui_tables.cpp"
		, "${imgui.dir}/imgui_widgets.cpp"
		]
	}
in
{
	, flatbuffers
	, imgui
	, imguiSkia
	, imguiWithSkia
	, imguiSkiaImpl
        , imguiWithHooks1919Wip
        , ImGuiAppHelper
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
	, mainSkiaSdl3
	, mainSkiaSdl3Minimal
	, tracyEnabled
	, tracyDisabled
	, sdl3Shared
	, mpvShared
	, skiaShared
	, mainVideoPlayerSdl3Mpv
}
