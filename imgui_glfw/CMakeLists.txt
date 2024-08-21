# generated using cmakelists.dhall
cmake_minimum_required(VERSION 3.24)
project(imgui_exe)
set(CMAKE_LINKER clang)
set(CMAKE_CXX_COMPILER clang++)

add_compile_definitions(IMGUI_ENABLE_FREETYPE
)
include_directories("${CMAKE_CURRENT_LIST_DIR}/./imgui"
"${CMAKE_CURRENT_LIST_DIR}/./src/arena/simple"
"${CMAKE_CURRENT_LIST_DIR}/./contrib/tracy/public"
)
add_compile_options(-std=c++20
-O3
)
link_libraries(-DNDEBUG
# imguiBackendGlfw
-lGL
# imguiBackendGlfw
-lglfw
# imguiFreetype
-lfreetype
)

# imgui
add_library(imgui OBJECT "${CMAKE_CURRENT_LIST_DIR}/./imgui/imgui.cpp"
"${CMAKE_CURRENT_LIST_DIR}/./imgui/imgui_demo.cpp"
"${CMAKE_CURRENT_LIST_DIR}/./imgui/imgui_draw.cpp"
"${CMAKE_CURRENT_LIST_DIR}/./imgui/imgui_tables.cpp"
"${CMAKE_CURRENT_LIST_DIR}/./imgui/imgui_widgets.cpp")

# imguiBackendGlfw
add_library(imguiBackendGlfw OBJECT "${CMAKE_CURRENT_LIST_DIR}/./imgui/imgui_impl_glfw.cpp"
"${CMAKE_CURRENT_LIST_DIR}/./imgui/imgui_impl_opengl3.cpp")
target_compile_options(imguiBackendGlfw PUBLIC )

# imguiFreetype
add_library(imguiFreetype OBJECT "${CMAKE_CURRENT_LIST_DIR}/./imgui/misc/freetype/imgui_freetype.cpp")
target_compile_options(imguiFreetype PUBLIC -I/usr/include/freetype2 -I/usr/include/libpng16)

# render
add_library(render OBJECT "${CMAKE_CURRENT_LIST_DIR}/./src/render.cpp")
target_include_directories(render PUBLIC "${CMAKE_CURRENT_LIST_DIR}/./src"
"${CMAKE_CURRENT_LIST_DIR}/./src/widgets/imgui_implot"
"${CMAKE_CURRENT_LIST_DIR}/./imcolortextedit")

# marshalling
add_library(marshalling OBJECT "${CMAKE_CURRENT_LIST_DIR}/./src/marshalling/receive.cpp"
"${CMAKE_CURRENT_LIST_DIR}/./src/marshalling/send.cpp"
"${CMAKE_CURRENT_LIST_DIR}/./src/marshalling/helper.cpp")
target_include_directories(marshalling PUBLIC "${CMAKE_CURRENT_LIST_DIR}/./src/marshalling"
"${CMAKE_CURRENT_LIST_DIR}/./src/widgets/imgui_implot")

# arena
add_library(arena OBJECT "${CMAKE_CURRENT_LIST_DIR}/./src/arena/simple/simple.cpp")

# widgets
add_library(widgets OBJECT "${CMAKE_CURRENT_LIST_DIR}/./src/widgets/common.cpp"
"${CMAKE_CURRENT_LIST_DIR}/./src/widgets/piemenu.cpp"
"${CMAKE_CURRENT_LIST_DIR}/./src/widgets/coloredbutton.cpp"
"${CMAKE_CURRENT_LIST_DIR}/./src/widgets/splitter.cpp")
target_compile_options(widgets PUBLIC -Wno-unused-parameter
-Wno-unused-variable)

# imguiToggle
add_library(imguiToggle OBJECT "${CMAKE_CURRENT_LIST_DIR}/./src/widgets/imgui_toggle/imgui_toggle.cpp"
"${CMAKE_CURRENT_LIST_DIR}/./src/widgets/imgui_toggle/imgui_toggle_palette.cpp"
"${CMAKE_CURRENT_LIST_DIR}/./src/widgets/imgui_toggle/imgui_toggle_presets.cpp"
"${CMAKE_CURRENT_LIST_DIR}/./src/widgets/imgui_toggle/imgui_toggle_renderer.cpp")
target_compile_options(imguiToggle PUBLIC -Wno-unused-parameter
-Wno-unused-variable)

# imguiImplot
add_library(imguiImplot OBJECT "${CMAKE_CURRENT_LIST_DIR}/./src/widgets/imgui_implot/implot.cpp"
"${CMAKE_CURRENT_LIST_DIR}/./src/widgets/imgui_implot/implot_demo.cpp"
"${CMAKE_CURRENT_LIST_DIR}/./src/widgets/imgui_implot/implot_items.cpp")

# imguiKnobs
add_library(imguiKnobs OBJECT "${CMAKE_CURRENT_LIST_DIR}/./src/widgets/imgui_knobs/imgui-knobs.cpp")

# imguiCoolbar
add_library(imguiCoolbar OBJECT "${CMAKE_CURRENT_LIST_DIR}/./src/widgets/imgui_coolbar/ImCoolbar.cpp")

# imguiFlamegraph
add_library(imguiFlamegraph OBJECT "${CMAKE_CURRENT_LIST_DIR}/./src/widgets/imgui_flamegraph/imgui_widget_flamegraph.cpp")
target_include_directories(imguiFlamegraph PUBLIC "${CMAKE_CURRENT_LIST_DIR}/./src/widgets/imgui_flamegraph"
"${CMAKE_CURRENT_LIST_DIR}/./src/widgets/imgui_flamegraph")

# imguiTextedit
add_library(imguiTextedit OBJECT "${CMAKE_CURRENT_LIST_DIR}/./imcolortextedit/TextEditor.cpp")
target_compile_options(imguiTextedit PUBLIC -Wno-unused-variable)

# binding

# tracyDisabled
add_library(tracyDisabled OBJECT "${CMAKE_CURRENT_LIST_DIR}/./contrib/tracy/public/TracyClient.cpp")

# main
add_library(main OBJECT "${CMAKE_CURRENT_LIST_DIR}/./main.cpp")
add_executable(imgui_exe $<TARGET_OBJECTS:imgui>
$<TARGET_OBJECTS:imguiBackendGlfw>
$<TARGET_OBJECTS:imguiFreetype>
$<TARGET_OBJECTS:render>
$<TARGET_OBJECTS:marshalling>
$<TARGET_OBJECTS:arena>
$<TARGET_OBJECTS:widgets>
$<TARGET_OBJECTS:imguiToggle>
$<TARGET_OBJECTS:imguiImplot>
$<TARGET_OBJECTS:imguiKnobs>
$<TARGET_OBJECTS:imguiCoolbar>
$<TARGET_OBJECTS:imguiFlamegraph>
$<TARGET_OBJECTS:imguiTextedit>
$<TARGET_OBJECTS:tracyDisabled>
$<TARGET_OBJECTS:main>)

