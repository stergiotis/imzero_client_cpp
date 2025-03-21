let lib = ../../dhall/lib.dhall
let prelude = ../../dhall/prelude.dhall
let sourceTreePart = lib.sourceTreePart
let path = \(loc : prelude.Location.Type) -> "${env:IMGUI_SKIA_CPP_ROOT as Text}/skia_minimal/${lib.locationToString loc}"

let TargetOs = <windows | linux | windows_cross >
let Target = {
   os : TargetOs
}

let flatbuffers = let dir = path (../../../contrib/flatbuffers as Location) in
 sourceTreePart::{
   , dir = dir
   , name = "flatbuffers"
   , includeDirs = {
      , local = [] : List Text
      , global = ["${dir}/include"] : List Text
   }
   , sources = [] : List Text
}
let systemFlags = \(tgt : Target) -> merge {
    , windows = sourceTreePart::{
              , dir = "."
            , sources = [] : List Text
            , name = "systemFlags"
            , cxxflags = {
               , local = [] : List Text
               , global = [
               ] : List Text
            }
            , ldflags = {
               , global = [
               ] : List Text
            }
    }
    , linux = sourceTreePart::{
              , dir = "."
            , sources = [] : List Text
            , name = "systemFlags"
            , cxxflags = {
               , local = [] : List Text
               , global = [
                  , "-fPIC"
               ]
            }
            , ldflags = {
               , global = [
                , "-Wl,--enable-new-dtags"
                , "-Wl,-rpath,'$ORIGIN/../lib' -Wl,-z,origin"
               ]
            }
    }
   , windows_cross = let dir = path (./.xwin-cache/splat as Location) in sourceTreePart::{
              , dir = "."
            , name = "systemFlags"
            , sources = [] : List Text
            , cxxflags = {
               , local = [] : List Text
               , global = [
                  , "-target=x86_64-pc-windows-msvc"
                  , "-i${dir}/crt/include"
                  , "-i${dir}/sdk/Include/ucrt"
                  , "-i${dir}/sdk/include/ucrt"
                  , "-i${dir}/sdk/Include/shared"
                  , "-i${dir}/sdk/Include/um"
                  , "-Xclang"
                  , "-dependent-lib=msvcrt"
                  --, "-fuse-ld=lld-link"
               ]
            }
            , ldflags = {
               , global = [
                  , "--target=x86_64-pc-windows-msvc"
                  , "-L${dir}/crt/lib/x64"
                  , "-L${dir}/sdk/Lib/ucrt/x64"
                  , "-L${dir}/sdk/Lib/um/x64"
                  , "-z noexecstack"
                  , "-Xclang"
                  , "--dependent-lib=msvcrt"
                  , "-v"
               ]
            }
   }
} tgt.os
let sdl3 = \(target : Target) -> let dir = path (../../../contrib/sdl as Location)
in
sourceTreePart::{
   , dir = dir
   , name = "sdl3Shared"
   , sources = [] : List Text
   , includeDirs = {
      , local = [] : List Text
      , global = ["${dir}/include"] : List Text
   }
   , cxxflags = {
      , local = [] : List Text
      , global = [] : List Text
   }
   , ldflags = { global = [
      , "-L${dir}/build"
      ] # (let win =  [, "-lSDL3-static"
                     , "-lwinmm"
                     , "-limm32"
                     , "-lole32"
                     , "-loleaut32"
                     , "-lversion"
                     , "-luuid"
                     , "-ladvapi32"
                     , "-lsetupapi"
                     , "-lshell32"
                     , "-ldinput8"] in
       merge {
        , windows = win
        , windows_cross = win 
        , linux = ["-lSDL3"]
        } target.os)
      }
}

let tracyEnabled = let dir = path (../../../contrib/tracy/public as Location) in sourceTreePart::{
   , name = "tracyEnabled"
   , dir = dir
   , defines = { 
      , local = [] : List Text
      , global = [ 
         , "TRACY_ENABLE"
         -- , "TRACY_ON_DEMAND"
      ]
   }
   , sources = [
      , "${dir}/TracyClient.cpp"
   ] : List Text
   , additionalDependants = [
   ] : List Text
   , includeDirs = {
      , global = ["${dir}"] : List Text
      , local = [] : List Text
   }
   , cxxflags = {
      , global = [
      ] : List Text
      , local = [] : List Text
   }
   , ldflags = {
      , global = [
      ] : List Text
   }
}
let tracyDisabled = tracyEnabled // {
   , name = "tracyDisabled"
   , defines = { 
      , local = [] : List Text
      , global = [] : List Text
   }
}
let ImGuiAppHelper = <SDL3>
let imguiWithHooks1919Wip = \(apph : ImGuiAppHelper) -> let dir = path (../imgui_w_hooks_1.91.9_wip as Location) in sourceTreePart::{
   , name = "imguiWithHooks1919Wip"
   , dir = dir
   , sources = [
      , "${dir}/imgui.cpp"
      , "${dir}/imgui_demo.cpp"
      , "${dir}/imgui_draw.cpp"
      , "${dir}/imgui_tables.cpp"
      , "${dir}/imgui_widgets.cpp"
      , "${dir}/imgui_impl_sdl3.cpp"
   ] # (merge {
           SDL3 = [ "${dir}/imgui_impl_sdl3.cpp" ]
        } apph)
   , includeDirs = {
      , local = [] : List Text
      , global = ["${dir}"] : List Text
   }
   , cxxflags = {
      , global = [
      ] : List Text
      , local = [] : List Text
   }
   , ldflags = {
      , global = [
      ] : List Text
   }
   , defines = {
      , local = [
             -- , "IMGUI_DISABLE_OBSOLETE_FUNCTIONS"
      ] : List Text
      , global = [] : List Text
      }
}
let imguiSkiaDriverImpl = let dir = path (../imgui_skia_driver_impl as Location) in sourceTreePart::{
   , name = "imguiSkiaDriverImpl"
   , dir = dir
   , sources = [
      , "${dir}/imgui_skia_app_sdl3.cpp"
      , "${dir}/imgui_skia_cli_options.cpp"
   ]
   , includeDirs = {
      , local = [] : List Text
      , global = ["${dir}"] : List Text
   }
   , cxxflags = {
      , global = [
      ] : List Text
      , local = [] : List Text
   }
   , ldflags = {
      , global = [
      ] : List Text
   }
   , defines = {
      , local = [] : List Text
      , global = [] : List Text
      }
}
let imguiSkiaImpl = let dir = path (../imgui_skia_impl as Location) in sourceTreePart::{
   , name = "imguiSkiaImpl"
   , dir = dir
   , sources = [
      , "${dir}/imgui_skia_extensions.cpp"
      , "${dir}/imgui_skia_hooks_impl.cpp"
      , "${dir}/imgui_skia_imzero_cmd_render.cpp"
      , "${dir}/imgui_skia_paragraph.cpp"
      , "${dir}/imgui_skia_setup_ui.cpp"
      , "${dir}/imgui_skia_tracy_support.cpp"
   ]
   , includeDirs = {
      , local = [] : List Text
      , global = ["${dir}"] : List Text
   }
   , cxxflags = {
      , global = [
      ] : List Text
      , local = [] : List Text
   }
   , ldflags = {
      , global = [
      ] : List Text
   }
   , defines = {
      , local = [] : List Text
      , global = [] : List Text
      }
}
let skia = \(tgt : Target) ->
    let dir = path (../../../contrib/skia as Location)
    in merge {
      linux = sourceTreePart::{
            , name = "skiaShared"
            , dir = dir
            , sources = [] : List Text
            , includeDirs = {
               , local = [] : List Text
               , global = [
                  , "${dir}"
               ] : List Text
            }
            , defines = {, local = [] : List Text
                     , global = [ --, "IMGUI_USE_BGRA_PACKED_COLOR" 
                     -- FIXME extract from rsp
               , "SK_RELEASE"
               , "SK_GAMMA_APPLY_TO_A8"
               , "SK_ALLOW_STATIC_GLOBAL_INITIALIZERS=1"
               , "SK_TYPEFACE_FACTORY_FREETYPE"
               , "SK_FONTMGR_FREETYPE_EMBEDDED_AVAILABLE"
               , "SK_FONTMGR_FONTCONFIG_AVAILABLE"
               , "SK_FONTMGR_FREETYPE_DIRECTORY_AVAILABLE"
               , "SK_FONTMGR_FREETYPE_EMPTY_AVAILABLE"
               , "SK_GL"
               , "SK_SUPPORT_PDF"
               , "SK_CODEC_DECODES_JPEG"
               , "SK_CODEC_DECODES_JPEG_GAINMAPS"
               , "SK_XML"
               , "SK_CODEC_DECODES_PNG"
               , "SK_CODEC_DECODES_RAW"
               , "SK_CODEC_DECODES_WEBP"
               , "SK_DEFAULT_TYPEFACE_IS_EMPTY"
               , "SK_DISABLE_LEGACY_DEFAULT_TYPEFACE"
               , "SK_R32_SHIFT=16"
               , "SK_ENABLE_PRECOMPILE"
               , "SK_GANESH"
               , "SK_ENABLE_PARAGRAPH"
               , "SK_UNICODE_AVAILABLE"
               , "SK_UNICODE_ICU_IMPLEMENTATION"
               , "SK_SHAPER_PRIMITIVE_AVAILABLE"
               , "SK_SHAPER_HARFBUZZ_AVAILABLE"
               , "SK_SHAPER_UNICODE_AVAILABLE"
               , "SK_ENABLE_SVG"
               , "SK_BUILD_FOR_UNIX"
                     ] : List Text}
            , cxxflags = {
               , global = [
               , "-ffp-contract=off" -- standard compliant fp processing
               , "-fstrict-aliasing" -- is on for optimization levels larger than O1
               , "-fvisibility=hidden"
               , "-fdata-sections"
               , "-ffunction-sections"
               , "-fvisibility-inlines-hidden"
               , "-fno-exceptions"
               , "-fno-rtti"
               ] : List Text
               , local = [
               ] : List Text
            }
            , ldflags = {
               , global = [
                  , "-ldl"
                  , "-lpthread"
                  , "-lfreetype"
                  , "-lz"
                  , "-lfontconfig"
                  , "-lwebpmux"
                  , "-lwebpdemux"
                  , "-lX11"
                  , "-lGLU"
                  , "-lGL"
                  , "-L${dir}/out/Shared"
                  , "-lskparagraph"
                  , "-lskia"
                  , "-lskunicode"
                  , "-lbentleyottmann"
                  , "-lskshaper"
                  -- , "-lsvg"
                  --, "-Wl,--verbose"
               ] : List Text
            }
            , nonSourceObjs = [] : List Text
         }
      , windows_cross = sourceTreePart::{
            , name = "skiaShared"
            , dir = dir
            , sources = [] : List Text
            , includeDirs = {
               , local = [] : List Text
               , global = [
                  , "${dir}"
               ] : List Text
            }
            , defines = {, local = [] : List Text
                     , global = [
                        , "MESA_EGL_NO_X11_HEADERS"
                     -- FIXME extract from rsp
               , "SK_RELEASE"
               , "SK_GAMMA_APPLY_TO_A8"
               , "SK_FONTMGR_DIRECTWRITE_AVAILABLE"
               , "SK_ALLOW_STATIC_GLOBAL_INITIALIZERS=1"
               , "SK_TYPEFACE_FACTORY_FREETYPE"
               , "SK_FONTMGR_FREETYPE_EMBEDDED_AVAILABLE"
               , "SK_SUPPORT_PDF"
               , "SK_XML"
               , "SK_CODEC_DECODES_RAW"
               , "SK_DEFAULT_TYPEFACE_IS_EMPTY"
               , "SK_DISABLE_LEGACY_DEFAULT_TYPEFACE"
               , "SK_ENABLE_PRECOMPILE"
               , "SK_GANESH"
               , "SK_ENABLE_PARAGRAPH"
               , "SK_UNICODE_AVAILABLE"
               , "SK_UNICODE_ICU_IMPLEMENTATION"
               , "SK_SHAPER_PRIMITIVE_AVAILABLE"
               , "SK_SHAPER_HARFBUZZ_AVAILABLE"
               , "SK_SHAPER_UNICODE_AVAILABLE"
               , "SK_ENABLE_SVG"
               , "SK_BUILD_FOR_UNIX"
                     ] : List Text}
            , cxxflags = {
               , global = [
               , "-funwind-tables"
               , "-ffp-contract=off" -- standard compliant fp processing
               , "-fstrict-aliasing" -- is on for optimization levels larger than O1
               , "-fvisibility=hidden"
               , "-fdata-sections"
               , "-ffunction-sections"
               , "-fvisibility-inlines-hidden"
               , "-fno-exceptions"
               , "-fno-rtti"
               ] : List Text
               , local = [
               ] : List Text
            }
            , ldflags = {
               , global = [
                  , "-lz"
                  , "-lwebpmux"
                  , "-lwebpdemux"
                  , "-L${dir}/out/Shared"
                  , "-lskparagraph"
                  , "-lskia"
                  , "-lskunicode"
                  , "-lbentleyottmann"
                  , "-lskshaper"
                  -- , "-lsvg"
                  --, "-Wl,--verbose"
               ] : List Text
            }
            , nonSourceObjs = [] : List Text
         }
      , windows = sourceTreePart::{
            , name = "skiaShared"
            , dir = dir
            , sources = [] : List Text
            , includeDirs = {
               , local = [] : List Text
               , global = [
                  , "${dir}"
               ] : List Text
            }
            , defines = {, local = [] : List Text
                     , global = [
                        , "MESA_EGL_NO_X11_HEADERS"
                     -- FIXME extract from skia.ninja
                     , "NOMINMAX"
                     , "NDEBUG"
                     , "SK_CODEC_DECODES_BMP"
                     , "SK_CODEC_DECODES_WBMP"
                     , "SKIA_DLL"
                     , "SK_ENABLE_PRECOMPILE"
                     , "SK_GANESH"
                     , "SK_DISABLE_TRACING"
                     , "SK_GAMMA_APPLY_TO_A8"
                     , "SK_ENABLE_AVX512_OPTS"
                     , "SKIA_IMPLEMENTATION=1"
                     , "SK_FONTMGR_FREETYPE_DIRECTORY_AVAILABLE"
                     , "SK_TYPEFACE_FACTORY_FREETYPE"
                     , "SK_FONTMGR_FREETYPE_EMBEDDED_AVAILABLE"
                     , "SK_FONTMGR_FREETYPE_EMPTY_AVAILABLE"
                     , "SK_TYPEFACE_FACTORY_DIRECTWRITE"
                     , "SK_FONTMGR_DIRECTWRITE_AVAILABLE"
                     , "SK_FONTMGR_GDI_AVAILABLE"
                     , "SK_GL"
                     , "SK_CODEC_ENCODES_JPEG"
                     , "SK_SUPPORT_PDF"
                     , "SK_CODEC_DECODES_JPEG"
                     , "SK_CODEC_ENCODES_PNG"
                     , "SK_CODEC_ENCODES_PNG_WITH_LIBPNG"
                     , "SK_CODEC_ENCODES_WEBP"
                     , "SK_SUPPORT_XPS"
                     , "SK_CODEC_DECODES_ICO"
                     , "SK_CODEC_DECODES_PNG"
                     , "SK_CODEC_DECODES_PNG_WITH_LIBPNG"
                     , "SK_CODEC_DECODES_RAW"
                     , "SK_CODEC_DECODES_WEBP"
                     , "SK_HAS_WUFFS_LIBRARY"
                     , "SK_CODEC_DECODES_GIF"
                     , "SK_XML"
                     , "_HAS_AUTO_PTR_ETC" -- FIXME workaround
                     ] : List Text}
            , cxxflags = {
               , global = [
               , "-funwind-tables"
               , "-ffp-contract=off" -- standard compliant fp processing
               , "-fstrict-aliasing" -- is on for optimization levels larger than O1
               , "-fvisibility=hidden"
               , "-fdata-sections"
               , "-ffunction-sections"
               , "-fvisibility-inlines-hidden"
               , "-fno-exceptions"
               , "-fno-rtti"
               ] : List Text
               , local = [
               ] : List Text
            }
            , ldflags = {
               , global = [
                  , "-L${dir}/out/Static"
                  , "-lskparagraph"
                  , "-lskia"
                  , "-lskunicode_core"
                  , "-lskunicode_icu"
                  , "-lbentleyottmann"
                  , "-lskshaper"
               ] : List Text
            }
            , nonSourceObjs = [] : List Text
         }
   } tgt.os

let mainSkiaSdl3Minimal = 
    let dir = path (../example_sdl3 as Location)
    in sourceTreePart::{
   , executable = True
   , name = "mainSkiaSdl3Minimal"
   , dir = dir
   , sources = [
      , "${dir}/main.cpp"
   ]
   , includeDirs = {
      , local = [] : List Text
      , global = [] : List Text
   }
   , defines = {
                   , local = [] : List Text
                   , global = [] : List Text
                  }
   , cxxflags = {
      , global = [
      ] : List Text
      , local = [
      , "-Wno-unused-parameter"
      ] : List Text
   }
   , ldflags = {
      , global = [
         , "-ldl"
         , "-lpthread"
         , "-lfreetype"
         , "-lz"
         , "-lfontconfig"
         , "-lwebpmux"
         , "-lwebpdemux"
         , "-lX11"
         , "-lGLU"
         , "-lGL"
         --, "-Wl,--verbose"
      ] : List Text
   }
   , nonSourceObjs = [ ] : List Text
}
in
{
   , TargetOs
   , Target
   , systemFlags
   , flatbuffers
   , imguiSkiaImpl
   , imguiSkiaDriverImpl
    , imguiWithHooks1919Wip
    , ImGuiAppHelper
   , mainSkiaSdl3Minimal
   , tracyEnabled
   , tracyDisabled
   , sdl3
   , skia
}
