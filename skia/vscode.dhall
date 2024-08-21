#!/usr/bin/env -S dhall-to-json --output .vscode/c_cpp_properties.json --file
let lib = ../dhall/lib.dhall
let c = ./common.dhall
let common = c False
let prelude = ../../contrib/dhall-lang/Prelude/package.dhall
let sourceTreeParts = common.sourceTreeParts
in
lib.vsCodeProperties::{
	, version = 4
	, enableConfigurationSquiggles = True
	, configurations = [
		, lib.vsCodeConfiguration::{
			, name = "linux"
			, compilerPath = common.cxx
			, compilerArgs = [] : List Text
			, intelliSenseMode  = "linux-gcc-x86"
			, includePath = (prelude.List.concatMap lib.sourceTreePart.Type Text (\(p : lib.sourceTreePart.Type) -> [p.dir] # p.includeDirs.local # p.includeDirs.global) sourceTreeParts)

			, defines = (prelude.List.concatMap lib.sourceTreePart.Type Text (\(p : lib.sourceTreePart.Type) -> p.defines.local # p.defines.global) sourceTreeParts)
			, cStandard = "c11"
			, cppStandard = "c++${Natural/show common.cppstd}"
			, configurationProvider = "ms-vscode.cmake-tools"
			, forcedInclude = [] : List Text
			, compileCommands = "\${workspaceFolder}/build/compile_commands.json"
      		, mergeConfigurations = True
			, browse = {
				, path = [] : List Text
				, limitSymbolsToIncludedHeaders = True
				, databaseFilename = "\${workspaceFolder}/.vscode/browse.vc.db"
			}
		}
	] : List lib.vsCodeConfiguration.Type
}
