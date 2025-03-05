let prelude = ./prelude.dhall
let lib = ./lib.dhall
let cmakelists = let T = {
	, output : {
		, exe : Optional Text
		, staticLib : Optional Text
	}
    , cxx : Text
    , linker : Text
	, cxxflags : List Text
	, ldflags : List Text
	, sourceTreeParts : List lib.sourceTreePart.Type
	, generateDepFiles : Bool
	, projectName : Text
	, cxxStandard : Natural
} in {Type = T, default = {
	, cxxflags = [] : List Text
	, ldflags = [] : List Text
	, sourceTreeParts = [] : List lib.sourceTreePart.Type
	, generateDepFiles = False
	, cxxStandard = 20
}}
let decorateWithName = \(t : List Text) -> \(name : Text) -> if prelude.List.null Text t then ([] : List Text) else (["# ${name}"] # t)
let definesToCxxflags = \(defines : List Text) -> (prelude.List.map Text Text (\(d : Text) -> "-D${d}") defines)
let includesToCxxflags = \(dirs : List Text) -> (prelude.List.map Text Text (\(d : Text) -> "-I${d}") dirs)
let cmakelistsToText = \(m : cmakelists.Type) -> 
	let gDefines = prelude.List.concatMap lib.sourceTreePart.Type Text (\(p : lib.sourceTreePart.Type) -> p.defines.global) m.sourceTreeParts
	let gIncludeDirs = prelude.List.concatMap lib.sourceTreePart.Type Text (\(p : lib.sourceTreePart.Type) -> p.includeDirs.global) m.sourceTreeParts
	let gCompileOptions = m.cxxflags # prelude.List.concatMap lib.sourceTreePart.Type Text (\(p : lib.sourceTreePart.Type) -> decorateWithName p.cxxflags.global p.name) m.sourceTreeParts
	let gLinkOptions = m.ldflags # prelude.List.concatMap lib.sourceTreePart.Type Text (\(p : lib.sourceTreePart.Type) -> decorateWithName p.ldflags.global p.name) m.sourceTreeParts

	let sources = prelude.List.concatMap lib.sourceTreePart.Type Text (\(p : lib.sourceTreePart.Type) -> p.sources) m.sourceTreeParts
	let nonSourceObjs = prelude.List.concatMap lib.sourceTreePart.Type Text (\(p : lib.sourceTreePart.Type) -> p.nonSourceObjs) m.sourceTreeParts
	let composePathList = \(l : List Text) -> prelude.Text.concatMapSep "\n" Text (\(p : Text) -> "\"\${CMAKE_CURRENT_LIST_DIR}/${p}\"") l

	let composeTarget = \(p : lib.sourceTreePart.Type) ->
	 	let out = (if (prelude.List.null Text p.sources) then [] : List Text else ["add_library(${p.name} OBJECT ${composePathList p.sources})"])
		# (if (prelude.List.null Text p.cxxflags.local) then [] : List Text else ["target_compile_options(${p.name} PUBLIC ${prelude.Text.concatSep "\n" p.cxxflags.local})"])
		# (if (prelude.List.null Text p.defines.local) then [] : List Text else ["target_compile_definitions(${p.name} PUBLIC ${prelude.Text.concatSep "\n" p.defines.local})"])
		# (if (prelude.List.null Text p.includeDirs.local) then [] : List Text else ["target_include_directories(${p.name} PUBLIC ${composePathList ([p.dir] # p.includeDirs.local)})"])
		# (if (prelude.List.null Text p.nonSourceObjs) then [] : List Text else
		 [, "add_library(${p.name}_imported OBJECT IMPORTED)"
		  , "set_property(TARGET ${p.name}_imported PROPERTY IMPORTED_OBJECTS ${composePathList p.nonSourceObjs})"])
		in
			if prelude.List.null Text out then "# empty target ${p.name}\n" else 
			("# begin ${p.name}\n"
			++ prelude.Text.concatSep "\n" out
			++ "\n# end ${p.name}\n")
	let targets = prelude.Text.concatMapSep "\n" lib.sourceTreePart.Type composeTarget m.sourceTreeParts
	let refTargetName = \(n : Text) -> "$<TARGET_OBJECTS:${n}>"
	let targetNames = \(p : lib.sourceTreePart.Type) -> 
	                    (if (prelude.List.null Text p.sources) then [] : List Text else [p.name])
	                    # (if (prelude.List.null Text p.nonSourceObjs) then [] : List Text else ["${p.name}_imported"])
	let referencedTargetNames = \(ps : List lib.sourceTreePart.Type) -> prelude.Text.concatMapSep "\n" Text refTargetName (prelude.List.concatMap lib.sourceTreePart.Type Text targetNames ps)
	let allTargetNames = referencedTargetNames m.sourceTreeParts
	let staticLibTargetNames = referencedTargetNames (prelude.List.filter lib.sourceTreePart.Type (\(p : lib.sourceTreePart.Type) -> p.executable != True) m.sourceTreeParts)
	in
	"# generated using cmakelists.dhall\n"
	++ "cmake_minimum_required(VERSION 3.24)\n"
	++ "project(${m.projectName})\n"
	-- ++ "set(CMAKE_CXX_STANDARD ${Natural/show m.cxxStandard})\n" -- FIXME why does cmake translate CMAKE_CXX_STANDARD=20 to -std=gnu++20
	++ "set(CMAKE_LINKER ${m.linker})\n"
	++ "set(CMAKE_CXX_COMPILER ${m.cxx})\n"
	++ "\n"
	++ "add_compile_definitions(${prelude.Text.concatSep "\n" gDefines}\n)\n"
	++ "include_directories(${composePathList gIncludeDirs}\n)\n"
	++ "add_compile_options(${prelude.Text.concatSep "\n" gCompileOptions}\n)\n"
	++ "link_libraries(${prelude.Text.concatSep "\n" gLinkOptions}\n)\n"
	++ "\n"
	++ targets
	++ merge {
		, Some = \(e : Text) -> "add_executable(${e} ${allTargetNames})\n"
		, None = ""
		} m.output.exe
	++ merge {
		, Some = \(a : Text) -> "add_library(${a} STATIC ${staticLibTargetNames})\n"
		, None = ""
	} m.output.staticLib
in {
    , cmakelists
    , cmakelistsToText
}
