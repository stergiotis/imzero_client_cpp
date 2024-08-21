let lib = ../dhall/lib.dhall
let sourceTreePartsRepo = ../dhall/sourceTreeParts.dhall
let mainPart = let dir = "." in lib.sourceTreePart::{
        , name = "main"
	, dir = dir
	, sources = [ "${dir}/main.cpp" ]
}
let sourceTreeParts = [
	, sourceTreePartsRepo.imgui
	, sourceTreePartsRepo.imguiBackendGlfw
	, sourceTreePartsRepo.imguiFreetype
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
	, sourceTreePartsRepo.tracyDisabled
	, mainPart
]
let cxx = "clang++"
let cppstd = 20
let cxxflagsDebug = [
        , "-g"
        , "-gdwarf-4"
        , "-Wall"
        , "-Wformat"
        , "-Wextra"
        , "-O1"
        , "-fsanitize=address"
        , "-fno-omit-frame-pointer"
        , "-DIMZERO_DEBUG_BUILD"
        --, "-fno-optimize-sibling-calls" -- no tail calls for better stacktraces
]
let cxxflagsRelease = [
        , "-O3"
]
let ldflagsDebug = ["-fsanitize=address"] : List Text
let ldflagsRelease = ["-DNDEBUG"] : List Text
--let stdlibFlags = ["-stdlib=libc++"] : List Text
let stdlibFlags = [] : List Text
let debug = False
in {
    , sourceTreeParts
    , cxx
    , cppstd
    , cxxflags = if debug then cxxflagsDebug else cxxflagsRelease
    , ldflags = if debug then ldflagsDebug else ldflagsRelease
    , stdlibFlags
    , debug
}

