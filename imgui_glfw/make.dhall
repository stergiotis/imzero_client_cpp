#!/usr/bin/env -S dhall text --output Makefile.out --file
let prelude = ../dhall/prelude.dhall
let lib = ../dhall/lib.dhall
let make = ../dhall/makefile.dhall
let common = ./common.dhall
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
let ldflags = [] : List Text

in make.makefileToText make.makefile::{
	, cxx = common.cxx
	, exe = "imgui_exe"
	, cxxflags = [, "-std=" ++ common.cppstd
	              , "\${CXXFLAGS}"
				  , "-Wno-unused-command-line-argument" -- triggered by -MF flags
				 ] # cxxflagsDebug
	, ldflags = ldflags
	, sourceTreeParts = common.sourceTreeParts
}
