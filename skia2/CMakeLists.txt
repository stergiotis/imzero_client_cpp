# generated using cmakelists.dhall
cmake_minimum_required(VERSION 3.24)
project(imgui_skia_exe)
set(CMAKE_LINKER clang++)
set(CMAKE_CXX_COMPILER clang++)

add_compile_definitions(
)
include_directories("${CMAKE_CURRENT_LIST_DIR}/./src/widgets/imgui_implot"
"${CMAKE_CURRENT_LIST_DIR}/./src/widgets/imgui_toggle"
"${CMAKE_CURRENT_LIST_DIR}/./src/widgets/imgui_knobs"
"${CMAKE_CURRENT_LIST_DIR}/./src/widgets/imgui_coolbar"
"${CMAKE_CURRENT_LIST_DIR}/./src/widgets/imgui_flamegraph"
"${CMAKE_CURRENT_LIST_DIR}/./imcolortextedit"
"${CMAKE_CURRENT_LIST_DIR}/./src/marshalling"
"${CMAKE_CURRENT_LIST_DIR}/./src/arena/simple"
"${CMAKE_CURRENT_LIST_DIR}/./src/widgets"
"${CMAKE_CURRENT_LIST_DIR}/./src"
"${CMAKE_CURRENT_LIST_DIR}/./src/binding"
)
add_compile_options(-std=c++20
-Wall
-Wformat
-Wextra
-Wno-unused-parameter
-fno-omit-frame-pointer
-O3
-I${CMAKE_CURRENT_LIST_DIR}/../skia_minimal/imgui_w_hooks_1.91.9_wip
-I${CMAKE_CURRENT_LIST_DIR}/../skia_minimal/imgui_skia_impl
-I${CMAKE_CURRENT_LIST_DIR}/../skia_minimal/imgui_skia_driver_impl
-I${CMAKE_CURRENT_LIST_DIR}/../../contrib/skia
-I${CMAKE_CURRENT_LIST_DIR}/../../contrib/flatbuffers/include
-I${CMAKE_CURRENT_LIST_DIR}/../../contrib/sdl/include
-I${CMAKE_CURRENT_LIST_DIR}/../../contrib/tracy/public
)
link_libraries(-Wl,-rpath,'$ORIGIN/../lib' -Wl,-z,origin
-DNDEBUG
)

# begin imguiImplot
add_library(imguiImplot OBJECT "${CMAKE_CURRENT_LIST_DIR}/./src/widgets/imgui_implot/implot.cpp"
"${CMAKE_CURRENT_LIST_DIR}/./src/widgets/imgui_implot/implot_demo.cpp"
"${CMAKE_CURRENT_LIST_DIR}/./src/widgets/imgui_implot/implot_items.cpp")
# end imguiImplot

# begin imguiToggle
add_library(imguiToggle OBJECT "${CMAKE_CURRENT_LIST_DIR}/./src/widgets/imgui_toggle/imgui_toggle.cpp"
"${CMAKE_CURRENT_LIST_DIR}/./src/widgets/imgui_toggle/imgui_toggle_palette.cpp"
"${CMAKE_CURRENT_LIST_DIR}/./src/widgets/imgui_toggle/imgui_toggle_presets.cpp"
"${CMAKE_CURRENT_LIST_DIR}/./src/widgets/imgui_toggle/imgui_toggle_renderer.cpp")
target_compile_options(imguiToggle PUBLIC -Wno-unused-parameter
-Wno-unused-variable)
# end imguiToggle

# begin imguiKnobs
add_library(imguiKnobs OBJECT "${CMAKE_CURRENT_LIST_DIR}/./src/widgets/imgui_knobs/imgui-knobs.cpp")
# end imguiKnobs

# begin imguiCoolbar
add_library(imguiCoolbar OBJECT "${CMAKE_CURRENT_LIST_DIR}/./src/widgets/imgui_coolbar/ImCoolbar.cpp")
# end imguiCoolbar

# begin imguiFlamegraph
add_library(imguiFlamegraph OBJECT "${CMAKE_CURRENT_LIST_DIR}/./src/widgets/imgui_flamegraph/imgui_widget_flamegraph.cpp")
# end imguiFlamegraph

# begin imguiTextedit
add_library(imguiTextedit OBJECT "${CMAKE_CURRENT_LIST_DIR}/./imcolortextedit/TextEditor.cpp"
"${CMAKE_CURRENT_LIST_DIR}/./imcolortextedit/LanguageDefinitions.cpp")
target_compile_options(imguiTextedit PUBLIC -Wno-unused-variable)
# end imguiTextedit

# begin marshalling
add_library(marshalling OBJECT "${CMAKE_CURRENT_LIST_DIR}/./src/marshalling/receive.cpp"
"${CMAKE_CURRENT_LIST_DIR}/./src/marshalling/send.cpp"
"${CMAKE_CURRENT_LIST_DIR}/./src/marshalling/helper.cpp")
# end marshalling

# begin arena
add_library(arena OBJECT "${CMAKE_CURRENT_LIST_DIR}/./src/arena/simple/simple.cpp")
# end arena

# begin widgets
add_library(widgets OBJECT "${CMAKE_CURRENT_LIST_DIR}/./src/widgets/common.cpp"
"${CMAKE_CURRENT_LIST_DIR}/./src/widgets/piemenu.cpp"
"${CMAKE_CURRENT_LIST_DIR}/./src/widgets/splitter.cpp")
target_compile_options(widgets PUBLIC -Wno-unused-parameter
-Wno-unused-variable)
# end widgets

# begin render
add_library(render OBJECT "${CMAKE_CURRENT_LIST_DIR}/./src/render.cpp")
# end render

# empty target binding

# begin imzeroClientSkiaSdl3Impl
add_library(imzeroClientSkiaSdl3Impl OBJECT "${CMAKE_CURRENT_LIST_DIR}/./imzero_client_skia_sdl3_impl/main.cpp"
"${CMAKE_CURRENT_LIST_DIR}/./imzero_client_skia_sdl3_impl/imzero_client_skia_sdl3_app.cpp"
"${CMAKE_CURRENT_LIST_DIR}/./imzero_client_skia_sdl3_impl/imzero_client_skia_sdl3_cli_options.cpp"
"${CMAKE_CURRENT_LIST_DIR}/./imzero_client_skia_sdl3_impl/bmp_encoder.cpp")
target_compile_options(imzeroClientSkiaSdl3Impl PUBLIC -Wno-unused-parameter)
target_include_directories(imzeroClientSkiaSdl3Impl PUBLIC "${CMAKE_CURRENT_LIST_DIR}/./imzero_client_skia_sdl3_impl"
"${CMAKE_CURRENT_LIST_DIR}/./imzero_client_skia_sdl3_impl")
# end imzeroClientSkiaSdl3Impl
add_executable(imgui_skia_exe $<TARGET_OBJECTS:imguiImplot>
$<TARGET_OBJECTS:imguiToggle>
$<TARGET_OBJECTS:imguiKnobs>
$<TARGET_OBJECTS:imguiCoolbar>
$<TARGET_OBJECTS:imguiFlamegraph>
$<TARGET_OBJECTS:imguiTextedit>
$<TARGET_OBJECTS:marshalling>
$<TARGET_OBJECTS:arena>
$<TARGET_OBJECTS:widgets>
$<TARGET_OBJECTS:render>
$<TARGET_OBJECTS:imzeroClientSkiaSdl3Impl>)
add_executable(imzero_client_skia_sdl3 $<TARGET_OBJECTS:imzeroClientSkiaSdl3Impl>)
add_library(libimgui_skia STATIC IMPORTED)
set_target_properties(libimgui_skia PROPERTIES IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/../skia_minimal/build/libimgui_skia.a")
target_link_libraries(imzero_client_skia_sdl3 imgui_skia)
