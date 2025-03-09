let lib = ../../dhall/lib.dhall
let prelude = ../../dhall/prelude.dhall
let sourceTreePart = lib.sourceTreePart

let flatbuffers = let dir = "./contrib/flatbuffers" in
 sourceTreePart::{
	, dir = dir
	, name = "flatbuffers"
	, includeDirs = {
		, local = [] : List Text
		, global = ["${dir}/include"] : List Text
	}
	, sources = [] : List Text
}

let sdl3Shared = let sdlDir = "${env:IMGUI_SKIA_CPP_ROOT as Text}/${lib.locationToString (../contrib/sdl as Location)}"
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
let imguiSkiaDriverImpl = let dir = "./imgui_skia_driver_impl" in sourceTreePart::{
	, name = "imguiSkiaDriverImpl"
	, dir = dir
	, sources = [
		, "${dir}/imgui_skia_app_sdl3.cpp"
		, "${dir}/imgui_skia_cli_options.cpp"
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
let imguiSkiaImpl = let dir = "./imgui_skia_impl" in sourceTreePart::{
	, name = "imguiSkiaImpl"
	, dir = dir
	, sources = [
		, "${dir}/imgui_skia_extensions.cpp"
		, "${dir}/imgui_skia_hooks_impl.cpp"
		, "${dir}/imgui_skia_imzero_cmd_render.cpp"
		, "${dir}/imgui_skia_paragraph.cpp"
		, "${dir}/imgui_skia_setup_ui.cpp"
		, "${dir}/imgui_skia_tracy_support.cpp"
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
let skiaShared = 
    let skiaSharedBaseDir = "${env:IMGUI_SKIA_CPP_ROOT as Text}/${lib.locationToString (../contrib/skia as Location)}"
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
    let dir = "./example_sdl3"
    in sourceTreePart::{
	, executable = True
	, name = "mainSkiaSdl3Minimal"
	, dir = dir
	, sources = [
		, "${dir}/main.cpp"
	]
	, includeDirs = {
		, local = [] : List Text
		, global = [] : List Text
	}
	, defines = {
                   , local = [] : List Text
                   , global = [] : List Text
                  }
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
in
{
	, flatbuffers
	, imguiSkiaImpl
	, imguiSkiaDriverImpl
    , imguiWithHooks1919Wip
    , ImGuiAppHelper
	, mainSkiaSdl3Minimal
	, tracyEnabled
	, tracyDisabled
	, sdl3Shared
	, skiaShared
}
