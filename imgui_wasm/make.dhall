#!/usr/bin/env -S dhall text --output Makefile.out --file
let prelude = ../dhall/prelude.dhall
let lib = ../dhall/lib.dhall
let make = ../dhall/makefile.dhall
let sourceTreePartsRepo = ../dhall/sourceTreeParts.dhall
let mainPart = let dir = "." in lib.sourceTreePart::{
	, dir = dir
	, sources = [ "${dir}/main.cpp" ]
}
let sourceTreeParts = [
	, sourceTreePartsRepo.imgui
	, sourceTreePartsRepo.render
	, sourceTreePartsRepo.marshalling
	, sourceTreePartsRepo.arena
	, sourceTreePartsRepo.widgets
	, sourceTreePartsRepo.imguiToggle
	, sourceTreePartsRepo.imguiImplot
	, sourceTreePartsRepo.imguiKnobs
	, sourceTreePartsRepo.imguiCoolbar
	, sourceTreePartsRepo.imguiFlamegraph
	, sourceTreePartsRepo.imguiTextedit
	, sourceTreePartsRepo.binding
	, mainPart
]
let cxx = "em++"
let cppstd = "c++20"
let cxxflags = [
	"-DIMGUI_DISABLE_FILE_FUNCTIONS"
]
let cxxflagsDebug = [
	, "-g"
	, "-gdwarf-4"
	, "-Wall"
	, "-Wformat"
	, "-Wextra"
]
let cxxflagsRelease = [
	, "-O3"
]
let ldflags = [
	, "-s NO_FILESYSTEM"
    , "-s STANDALONE_WASM"
	, "-s USE_FREETYPE=1"
	, "-s USE_GLFW=1"
] : List Text

in make.makefileToText make.makefile::{
	, cxx = cxx
	, exe = "imgui.wasm"
	, cxxflags = ["-std=" ++ cppstd] # cxxflagsDebug
	, ldflags = ldflags
	, sourceTreeParts = sourceTreeParts
}
