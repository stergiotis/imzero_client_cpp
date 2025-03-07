#!/usr/bin/env -S dhall text --output CMakeLists.txt --file
let prelude = ../dhall/prelude.dhall
let lib = ../dhall/lib.dhall
let cmake = ../dhall/cmakelists.dhall
let sourceTreePartsRepo = ./dhall/sourceTreeParts.dhall
let common = ./common.dhall
let winSdkDir = "/home/deck/repo/imzero_client_cpp/skia_minimal/.xwin-cache/splat"
let crossCxxFlags = [, "--target=x86_64-pc-windows-msvc"
                     , "-isystem${winSdkDir}/crt/include"
                     , "-isystem${winSdkDir}/sdk/Include/ucrt"
                     , "-isystem${winSdkDir}/sdk/include/ucrt"
                     , "-isystem${winSdkDir}/sdk/Include/shared"
                     , "-isystem${winSdkDir}/sdk/Include/um"

                     , "-Xclang", "--dependent-lib=msvcrt"
                     , "-fuse-ld=lld-link"
                   --  , "-Wno-unused-command-line-argument"
                     ]
let crossLdFlags = [ , "--target=x86_64-pc-windows-msvc"
                     , "-L${winSdkDir}/crt/lib/x64"
                     , "-L${winSdkDir}/sdk/Lib/ucrt/x64"
                     , "-L${winSdkDir}/sdk/Lib/um/x64"
                     , "-z noexecstack"
                     , "-Xclang", "--dependent-lib=msvcrt"
                     , "-v"
                   ]
in 
cmake.cmakelistsToText cmake.cmakelists::{
	, cxx = common.cxx
	, linker = "lld-link" --common.linker
	, output = {
	            , exe = None Text --"imgui_skia_exe"
	            , staticLib = Some "imgui_skia"
	            }
	, projectName = "imgui_skia_exe"
	, cxxflags = ["-std=c++${Natural/show common.cppstd}" ] # common.cxxflags # common.stdlibFlags # crossCxxFlags
	, ldflags = common.ldflags # common.stdlibFlags # crossLdFlags
	, sourceTreeParts = common.sourceTreeParts
	, cxxStandard = common.cppstd
}
++ "\n"
++ "add_executable(imgui_skia_exe $<TARGET_OBJECTS:mainSkiaSdl3Minimal>)\n"
++ "target_link_libraries(imgui_skia_exe imgui_skia)\n"

++ "add_executable(imgui_skia_exe2 $<TARGET_OBJECTS:mainSkiaSdl3Minimal>)\n"
++ "add_library(libimgui_skia2 STATIC IMPORTED)\n"
++ "set_target_properties(libimgui_skia2 PROPERTIES IMPORTED_LOCATION \"libimgui_skia.a\")\n"
++ "target_link_libraries(imgui_skia_exe2 libimgui_skia2)\n"
