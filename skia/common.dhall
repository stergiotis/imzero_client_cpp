let lib = ../dhall/lib.dhall
let debug = False
let asan = False
let sourceTreePartsRepo = ../dhall/sourceTreeParts.dhall
let sourceTreeParts = [
	, sourceTreePartsRepo.flatbuffers
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
	, sourceTreePartsRepo.sdl3Shared
	, sourceTreePartsRepo.skiaShared
	, sourceTreePartsRepo.mainSkiaSdl3Video
] # (if debug then [ , sourceTreePartsRepo.tracyEnabled ] else [ ,sourceTreePartsRepo.tracyDisabled ] : List lib.sourceTreePart.Type )
let clangdir = (env:HOME as Text) ++ "/Downloads/clang+llvm-18.1.8-x86_64-linux-gnu-ubuntu-18.04"
let cxx = "${clangdir}/bin/clang++"
let cppstd = 20
let cxxflags = [
	, "-fno-omit-frame-pointer" -- increases debuggability with little to no performance impact
	]
let cxxflagsDebug = [
	, "-g"
	, "-gdwarf-4"
	, "-Wall"
	, "-Wformat"
	, "-Wextra"
	, "-O1"
	] # (if asan then [ "-fsanitize=address", "-fsanitize=undefined" ] else [] : List Text) #
	[
	, "-DIMZERO_DEBUG_BUILD"
	--, "-fno-optimize-sibling-calls" -- no tail calls for better stacktraces
]
let cxxflagsRelease = [
	, "-O3"
]
let linker = "-fuse-ld=lld"
let ldflagsDebug = if asan then [ 
	, "-fsanitize=address"
	, "-fsanitize=undefined"
--  , "-Wl,--verbose"
--	, linker
	] else [
--	, linker
	] : List Text
let ldflagsRelease = [
        , "-DNDEBUG"
--  , "-Wl,--verbose"
--	, linker
	] : List Text
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
