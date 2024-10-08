#!/usr/bin/env -S dhall text --output CMakeLists.txt --file
let prelude = ../dhall/prelude.dhall
let lib = ../dhall/lib.dhall
let cmake = ../dhall/cmakelists.dhall
let sourceTreePartsRepo = ../dhall/sourceTreeParts.dhall
let common = ./common.dhall
in 
cmake.cmakelistsToText cmake.cmakelists::{
	, cxx = common.cxx
	, exe = "imgui_exe"
	, projectName = "imgui_exe"
	, cxxflags = ["-std=c++${Natural/show common.cppstd}" ] # common.cxxflags # common.stdlibFlags
	, ldflags = common.ldflags # common.stdlibFlags
	, sourceTreeParts = common.sourceTreeParts
	, linker = "clang"
}
