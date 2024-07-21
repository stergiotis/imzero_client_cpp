# generated using cmakelists.dhall
cmake_minimum_required(VERSION 3.24)
project(mzero_video_play)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_LINKER clang++)
set(CMAKE_CXX_COMPILER clang++)

add_compile_definitions(TRACY_ENABLE)
include_directories("${CMAKE_CURRENT_LIST_DIR}/./contrib/sdl3/include"
"${CMAKE_CURRENT_LIST_DIR}/./contrib/flatbuffers"
"${CMAKE_CURRENT_LIST_DIR}/./contrib/tracy/public")
set(COMPILE_OPTIONS -std=c++20
-fno-exceptions
-g
-gdwarf-4
-Wall
-Wformat
-Wextra
-O1
-fsanitize=address
-fsanitize=undefined
-fno-omit-frame-pointer
-DIMZERO_DEBUG_BUILD)
link_libraries(-fsanitize=address
-fsanitize=undefined
-v
-lmpv)


add_library(render OBJECT "${CMAKE_CURRENT_LIST_DIR}/./sdl3_mpv/main.cpp"
"${CMAKE_CURRENT_LIST_DIR}/./sdl3_mpv/events.cpp"
"${CMAKE_CURRENT_LIST_DIR}/./sdl3_mpv/app.cpp")

add_library(mpv_imported OBJECT IMPORTED)
set_property(TARGET mpv_imported PROPERTY IMPORTED_OBJECTS "${CMAKE_CURRENT_LIST_DIR}/./contrib/sdl3/build/libSDL3.a")


add_library(tracyEnabled OBJECT "${CMAKE_CURRENT_LIST_DIR}/./contrib/tracy/public/TracyClient.cpp")
add_executable(imzero_video_play $<TARGET_OBJECTS:render>
$<TARGET_OBJECTS:mpv_imported>
$<TARGET_OBJECTS:tracyEnabled>)

