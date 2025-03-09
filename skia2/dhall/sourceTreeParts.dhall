let lib = ../../dhall/lib.dhall
let prelude = ../../dhall/prelude.dhall
let sourceTreePart = lib.sourceTreePart
let imguiImplot = let dir = "./src/widgets/imgui_implot" in sourceTreePart::{
	, name = "imguiImplot"
	, dir = dir
	, includeDirs = {
		, local = [] : List Text
		, global = ["${dir}"] : List Text
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
		, local = [] : List Text
		, global = ["${dir}"] : List Text
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
		, global = ["${dir}"] : List Text
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
		, global = ["${dir}"] : List Text
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
		, global = ["${dir}"] : List Text
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
		, global = ["${dir}"] : List Text
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
		, local = [] : List Text
		, global = ["${dir}"] : List Text
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
		, global = ["${dir}"] : List Text
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
		, local = [] : List Text
		, global = ["${dir}"] : List Text
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
	, includeDirs = {
		, local = [] : List Text
		, global = ["${dir}"] : List Text
	}
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
let imzeroClientSkiaSdl3Impl =
    let dir = "./imzero_client_skia_sdl3_impl"
    in sourceTreePart::{
	, name = "imzeroClientSkiaSdl3Impl"
	, dir = dir
	, sources = [
		, "${dir}/main.cpp"
		, "${dir}/imzero_client_skia_sdl3_app.cpp"
		, "${dir}/imzero_client_skia_sdl3_cli_options.cpp"
		, "${dir}/bmp_encoder.cpp"
	]
	, includeDirs = {
		, local = [
            , "${dir}"
			]
		, global = [] : List Text
	}
	, defines = {, local = [] : List Text, global = [] : List Text}
	, cxxflags = {
		, global = [
		] : List Text
		, local = [
		, "-Wno-unused-parameter"
		] : List Text
	}
	, ldflags = {
		, global = [] : List Text
	}
	, nonSourceObjs = [ ] : List Text
}
in
{
    , imguiImplot
    , imguiToggle
    , imguiKnobs
    , imguiCoolbar
    , imguiFlamegraph
    , imguiTextedit
    , marshalling
    , arena
    , widgets
    , render
    , binding
	, imzeroClientSkiaSdl3Impl
}
