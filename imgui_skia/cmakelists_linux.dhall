#!/usr/bin/env -S dhall text --output CMakeLists.txt --file
let prelude = ../dhall/prelude.dhall
let lib = ../dhall/lib.dhall
let cmake = ../dhall/cmakelists.dhall
let sourceTreePartsRepo = ./dhall/sourceTreeParts.dhall
let c = ./common.dhall
let common = c.common {os = c.TargetOs.windows}
in 
cmake.cmakelistsToText cmake.cmakelists::{
	, cxx = common.cxx
	, linker = common.linker
	, output = {
	            , exe = None Text
	            , staticLib = Some "imgui_skia"
	            }
	, projectName = "imgui_skia"
	, cxxflags = ["-std=c++${Natural/show common.cppstd}" ] # common.cxxflags # common.stdlibFlags
	, ldflags = common.ldflags # common.stdlibFlags
	, sourceTreeParts = common.sourceTreeParts
	, cxxStandard = common.cppstd
}

-- simulate using the static library libimgui_skia.a in a project
--++ "add_executable(imgui_skia_exe2 $<TARGET_OBJECTS:mainSkiaSdl3Minimal>)\n"
--++ "add_dependencies(imgui_skia_exe2 imgui_skia)\n"
--++ "add_library(libimgui_skia2 STATIC IMPORTED)\n"
--++ "set_target_properties(libimgui_skia2 PROPERTIES IMPORTED_LOCATION \"\${CMAKE_CURRENT_LIST_DIR}/build/libimgui_skia.a\")\n"
--++ "target_link_libraries(imgui_skia_exe2 libimgui_skia2)\n"