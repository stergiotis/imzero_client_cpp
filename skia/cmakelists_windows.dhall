#!/usr/bin/env -S dhall text --output CMakeLists.txt --file
let prelude = ../dhall/prelude.dhall
let lib = ../dhall/lib.dhall
let cmake = ../dhall/cmakelists.dhall
let sourceTreePartsRepo = ../dhall/sourceTreeParts.dhall
let c = ./common.dhall
let common = c.common {os = c.TargetOs.windows}
in 
cmake.cmakelistsToText cmake.cmakelists::{
	, cxx = common.cxx
	, linker = common.linker
	, output = {
             , exe = Some "imgui_skia.exe"
             , staticLib = None Text
           }
	, projectName = "imgui_skia.exe"
	, cxxflags = ["-std=c++${Natural/show common.cppstd}" ] # common.cxxflags # common.stdlibFlags
	, ldflags = common.ldflags # common.stdlibFlags
	, sourceTreeParts = common.sourceTreeParts
	, librarySourceTreeParts = common.librarySourceTreeParts
	, cxxStandard = common.cppstd
	, recursiveLinking = False
}
++ "add_library(libimgui_skia STATIC IMPORTED)\n"
++ "set_target_properties(libimgui_skia PROPERTIES IMPORTED_LOCATION \"\${CMAKE_CURRENT_LIST_DIR}/../imgui_skia/build/imgui_skia.lib\")\n"
++ "target_link_libraries(imzeroClientSkiaSdl3Impl libimgui_skia)\n"
