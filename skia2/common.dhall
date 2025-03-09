let common = 
	let lib = ../dhall/lib.dhall
	let debug = False
	let asan = False
	let ubsan = False
	let sourceTreePartsRepo = ./dhall/sourceTreeParts.dhall
	let sourceTreePartsImGuiSkia = ../skia_minimal/dhall/sourceTreeParts.dhall
	let sourceTreeParts = [
        --, sourceTreePartsImGuiSkia.flatbuffers
        --, sourceTreePartsImGuiSkia.imguiWithHooks1919Wip sourceTreePartsImGuiSkia.ImGuiAppHelper.SDL3
        --, sourceTreePartsImGuiSkia.imguiSkiaImpl
        --, sourceTreePartsImGuiSkia.sdl3Shared
        --, sourceTreePartsImGuiSkia.skiaShared
        --, sourceTreePartsImGuiSkia.imguiSkiaDriverImpl
		--, sourceTreePartsImGuiSkia.flatbuffers

		, sourceTreePartsRepo.imguiImplot
		, sourceTreePartsRepo.imguiToggle
		, sourceTreePartsRepo.imguiKnobs
		, sourceTreePartsRepo.imguiCoolbar
		, sourceTreePartsRepo.imguiFlamegraph
		, sourceTreePartsRepo.imguiTextedit

		, sourceTreePartsRepo.marshalling
		, sourceTreePartsRepo.arena
		, sourceTreePartsRepo.widgets
		, sourceTreePartsRepo.render
		, sourceTreePartsRepo.binding

		, sourceTreePartsRepo.imzeroClientSkiaSdl3Impl
	] 
	--# (if debug then [ , sourceTreePartsImGuiSkia.tracyEnabled ] else [ ,sourceTreePartsImGuiSkia.tracyDisabled ] : List lib.sourceTreePart.Type )
        let cxx = "clang++"
	let cppstd = 20
	let cxxflagsRelease = [
	    , "-Wall"
		, "-Wformat"
		, "-Wextra"
		, "-Wno-unused-parameter"
		, "-fno-omit-frame-pointer" -- increases debuggability with little to no performance impact
		, "-O3"
		]
	let cxxflagsDebug = [
		, "-g"
		, "-gdwarf-4"
		, "-Wall"
		, "-Wformat"
		, "-Wextra"
		, "-Wno-unused-parameter"
		, "-O1"
		, "-DIMZERO_DEBUG_BUILD"
		--, "-fno-optimize-sibling-calls" -- no tail calls for better stacktraces
	]
	let cxxflags = (if debug then cxxflagsDebug else cxxflagsRelease)
	               # (if asan then [ "-fsanitize=address" ] else [] : List Text)
	               # (if ubsan then [ "-fsanitize=undefined" ] else [] : List Text)
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
	let ldflags = ["-Wl,-rpath,'$ORIGIN/../lib' -Wl,-z,origin"] # (if debug then ldflagsDebug else ldflagsRelease)
	let stdlibFlags = [] : List Text
	let linker = cxx
	in {
	    , sourceTreeParts
	    , cxx
		, linker
	    , cppstd
		, cxxflags = cxxflags
		, ldflags = ldflags
		, stdlibFlags
		, debug
	}
in common
