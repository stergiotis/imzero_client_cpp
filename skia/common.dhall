let sourceTreePartsRepo = ../dhall/sourceTreeParts.dhall
let sourceTreeParts = [
	, sourceTreePartsRepo.imguiWithSkia
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
	, sourceTreePartsRepo.skia
	, sourceTreePartsRepo.tracyEnabled
	, sourceTreePartsRepo.flatbuffers
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
