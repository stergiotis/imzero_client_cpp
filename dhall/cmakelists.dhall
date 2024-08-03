let prelude = ./prelude.dhall
let lib = ./lib.dhall
let cmakelists = let T = {
    , cxx : Text
    , linker : Text
    , exe : Text
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
let definesToCxxflags = \(defines : List Text) -> (prelude.List.map Text Text (\(d : Text) -> "-D${d}") defines)
let includesToCxxflags = \(dirs : List Text) -> (prelude.List.map Text Text (\(d : Text) -> "-I${d}") dirs)
let cmakelistsToText = \(m : cmakelists.Type) -> 
	let gDefines = prelude.List.concatMap lib.sourceTreePart.Type Text (\(p : lib.sourceTreePart.Type) -> p.defines.global) m.sourceTreeParts
	let gIncludeDirs = prelude.List.concatMap lib.sourceTreePart.Type Text (\(p : lib.sourceTreePart.Type) -> p.includeDirs.global) m.sourceTreeParts
	let gCompileOptions = m.cxxflags # prelude.List.concatMap lib.sourceTreePart.Type Text (\(p : lib.sourceTreePart.Type) -> p.cxxflags.global) m.sourceTreeParts
	let gLinkOptions = m.ldflags # prelude.List.concatMap lib.sourceTreePart.Type Text (\(p : lib.sourceTreePart.Type) -> p.ldflags.global) m.sourceTreeParts

	let sources = prelude.List.concatMap lib.sourceTreePart.Type Text (\(p : lib.sourceTreePart.Type) -> p.sources) m.sourceTreeParts
	let nonSourceObjs = prelude.List.concatMap lib.sourceTreePart.Type Text (\(p : lib.sourceTreePart.Type) -> p.nonSourceObjs) m.sourceTreeParts
	let composePathList = \(l : List Text) -> prelude.Text.concatMapSep "\n" Text (\(p : Text) -> "\"\${CMAKE_CURRENT_LIST_DIR}/${p}\"") l

	let composeTarget = \(p : lib.sourceTreePart.Type) ->
	    ""
		++ (if (prelude.List.null Text p.sources) then "" else "add_library(${p.name} OBJECT ${composePathList p.sources})\n")
		++ (if (prelude.List.null Text p.cxxflags.local) then "" else "target_compile_options(${p.name} PUBLIC ${prelude.Text.concatSep "\n" p.cxxflags.local})\n")
		++ (if (prelude.List.null Text p.defines.local) then "" else "target_compile_definitions(${p.name} PUBLIC ${prelude.Text.concatSep "\n" p.defines.local})\n")
		++ (if (prelude.List.null Text p.includeDirs.local) then "" else "target_include_directories(${p.name} PUBLIC ${composePathList ([p.dir] # p.includeDirs.local)})\n")
		++ (if (prelude.List.null Text p.nonSourceObjs) then "" else
		 ("add_library(${p.name}_imported OBJECT IMPORTED)\n"
		 ++ "set_property(TARGET ${p.name}_imported PROPERTY IMPORTED_OBJECTS ${composePathList p.nonSourceObjs})\n"
		 )
		 )
	let targets = prelude.Text.concatMapSep "\n" lib.sourceTreePart.Type composeTarget m.sourceTreeParts
	let refTargetName = \(n : Text) -> "$<TARGET_OBJECTS:${n}>"
	let targetNames = prelude.Text.concatMapSep "\n" Text refTargetName (prelude.List.concatMap lib.sourceTreePart.Type Text (\(p : lib.sourceTreePart.Type) -> 
	(if (prelude.List.null Text p.sources) then [] : List Text else [p.name])
	# (if (prelude.List.null Text p.nonSourceObjs) then [] : List Text else ["${p.name}_imported"])
	)  m.sourceTreeParts)
	in
	"# generated using cmakelists.dhall\n"
	++ "cmake_minimum_required(VERSION 3.24)\n"
	++ "project(${m.projectName})\n"
	-- ++ "set(CMAKE_CXX_STANDARD ${Natural/show m.cxxStandard})\n"
	++ "set(CMAKE_LINKER ${m.linker})\n"
	++ "set(CMAKE_CXX_COMPILER ${m.cxx})\n"
	++ "\n"
	++ "add_compile_definitions(${prelude.Text.concatSep "\n" gDefines})\n"
	++ "include_directories(${composePathList gIncludeDirs})\n"
	++ "add_compile_options(${prelude.Text.concatSep "\n" gCompileOptions})\n"
	++ "link_libraries(${prelude.Text.concatSep "\n" gLinkOptions})\n"
	++ "\n"
	++ targets
	++ "add_executable(${m.exe} ${targetNames})\n"
	++ "\n"
in {
    , cmakelists
    , cmakelistsToText
}
