let sourceTreePartsImGuiSkia = ../imgui_skia/dhall/sourceTreeParts.dhall
let TargetOs = sourceTreePartsImGuiSkia.TargetOs
let Target = sourceTreePartsImGuiSkia.Target
let common = \(target : Target) ->
	let lib = ../dhall/lib.dhall
	let debug = False
	let asan = False
	let ubsan = False
	let sourceTreePartsRepo = ./dhall/sourceTreeParts.dhall
	let librarySourceTreeParts = [
        , sourceTreePartsImGuiSkia.systemFlags target
        , sourceTreePartsImGuiSkia.flatbuffers
        , sourceTreePartsImGuiSkia.imguiWithHooks1919Wip sourceTreePartsImGuiSkia.ImGuiAppHelper.SDL3
        , sourceTreePartsImGuiSkia.imguiSkiaImpl
	, sourceTreePartsImGuiSkia.sdl3 target
        , sourceTreePartsImGuiSkia.skia target
        , sourceTreePartsImGuiSkia.imguiSkiaDriverImpl
	]
	let sourceTreeParts = [
		, sourceTreePartsRepo.marshalling
		, sourceTreePartsRepo.arena
		, sourceTreePartsRepo.binding
		, sourceTreePartsRepo.render

		, sourceTreePartsRepo.imguiImplot
		, sourceTreePartsRepo.imguiToggle
		, sourceTreePartsRepo.imguiKnobs
		, sourceTreePartsRepo.imguiCoolbar
		, sourceTreePartsRepo.imguiFlamegraph
		, sourceTreePartsRepo.imguiTextedit
		, sourceTreePartsRepo.widgets

		, sourceTreePartsRepo.imzeroClientSkiaSdl3Impl
	] 
	# (if debug then [ , sourceTreePartsImGuiSkia.tracyEnabled ] else [ , sourceTreePartsImGuiSkia.tracyDisabled ] : List lib.sourceTreePart.Type )
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
	let ldflags = (if debug then ldflagsDebug else ldflagsRelease)
	let stdlibFlags = [] : List Text
	let linker = cxx
	in {
	    , sourceTreeParts
		, librarySourceTreeParts
	    , cxx
		, linker
	    , cppstd
		, cxxflags = cxxflags
		, ldflags = ldflags
		, stdlibFlags
		, debug
	}
in {
  , common
  , Target
  , TargetOs
  }
