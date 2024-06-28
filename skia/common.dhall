let lib = ../dhall/lib.dhall
let debug = False
let asan = False
let clangdir = env:CLANGDIR as Text -- FIXME sync with ./build_skia_asan.sh
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
	--, sourceTreePartsRepo.skia
	, sourceTreePartsRepo.skiaSdl asan
	, sourceTreePartsRepo.flatbuffers
] # (if debug then [ , sourceTreePartsRepo.tracyEnabled ] else [ ,sourceTreePartsRepo.tracyDisabled ] : List lib.sourceTreePart.Type )
let cxx = "${clangdir}/bin/clang++"
let cppstd = 20
let cxxflags = ["-fno-exceptions"]
let cxxflagsDebug = [
	, "-g"
	, "-gdwarf-4"
	, "-Wall"
	, "-Wformat"
	, "-Wextra"
	, "-O1"
	] # (if asan then [ "-fsanitize=address", "-fsanitize=undefined" ] else [] : List Text) #
	[, "-fno-omit-frame-pointer"
	, "-DIMZERO_DEBUG_BUILD"
	--, "-fno-optimize-sibling-calls" -- no tail calls for better stacktraces
]
let cxxflagsRelease = [
	, "-O3"
]
let ldflagsDebug = if asan then [ 
	, "-fsanitize=address"
	, "-fsanitize=undefined"
	, "-fuse-ld=lld"
	, "-v"
	, "-Wl,-rpath,${clangdir}/lib/x86_64-unknown-linux-gnu"
	] else [] : List Text
let ldflagsRelease = ["-DNDEBUG"] : List Text
--let stdlibFlags = ["-stdlib=libc++"] : List Text
let stdlibFlags = [] : List Text
let linker = cxx
in {
    , sourceTreeParts
    , cxx
	, linker
    , cppstd
	, cxxflags = cxxflags # (if debug then cxxflagsDebug else cxxflagsRelease)
	, ldflags = if debug then ldflagsDebug else ldflagsRelease
	, stdlibFlags
	, debug
}
