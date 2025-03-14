let common = 
    let lib = ../dhall/lib.dhall
    let debug = False
    let asan = False
    let ubsan = False
    let sourceTreePartsRepo = ./dhall/sourceTreeParts.dhall
    let target = {os = sourceTreePartsRepo.TargetOs.linux}
    let sourceTreeParts = [
        , sourceTreePartsRepo.systemFlags target
        , sourceTreePartsRepo.flatbuffers
        , sourceTreePartsRepo.imguiWithHooks1919Wip sourceTreePartsRepo.ImGuiAppHelper.SDL3
        , sourceTreePartsRepo.imguiSkiaImpl
        , sourceTreePartsRepo.sdl3Shared
        , sourceTreePartsRepo.skiaShared target
        , sourceTreePartsRepo.imguiSkiaDriverImpl
        --, sourceTreePartsRepo.mainSkiaSdl3Minimal
    ] 
    # (if debug then [ , sourceTreePartsRepo.tracyEnabled ] else [ ,sourceTreePartsRepo.tracyDisabled ] : List lib.sourceTreePart.Type )
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
        , "-DIMGUI_SKIA_DEBUG_BUILD"
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
    --    , linker
        ] else [
    --    , linker
        ] : List Text
    let ldflagsRelease = [
        , "-DNDEBUG"
    --  , "-Wl,--verbose"
    --    , linker
        ] : List Text
    --let stdlibFlags = ["-stdlib=libc++"] : List Text
    let ldflags = (if debug then ldflagsDebug else ldflagsRelease)
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
