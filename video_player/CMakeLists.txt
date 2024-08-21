# generated using cmakelists.dhall
cmake_minimum_required(VERSION 3.24)
project(mzero_video_play)
set(CMAKE_LINKER clang++)
set(CMAKE_CXX_COMPILER clang++)

add_compile_definitions(TRACY_ENABLE
)
include_directories("${CMAKE_CURRENT_LIST_DIR}/./contrib/flatbuffers"
"${CMAKE_CURRENT_LIST_DIR}/./contrib/tracy/public"
)
add_compile_options(-std=c++20
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
-DIMZERO_DEBUG_BUILD
# sdl3Shared
-I/data/repo/contrib/sdl/include
)
link_libraries(-fsanitize=address
-fsanitize=undefined
-v
# sdl3Shared
-L/data/repo/contrib/sdl/build
# sdl3Shared
-Wl,-rpath,/data/repo/contrib/sdl/build
# sdl3Shared
-Wl,--enable-new-dtags
# sdl3Shared
-lSDL3
# mpvShared
-lmpv
)

# sdl3Shared

# mpvShared

# flatbuffers

# mainVideoPlayerSdl3Mpv
add_library(mainVideoPlayerSdl3Mpv OBJECT "${CMAKE_CURRENT_LIST_DIR}/./sdl3_mpv/main.cpp"
"${CMAKE_CURRENT_LIST_DIR}/./sdl3_mpv/events.cpp"
"${CMAKE_CURRENT_LIST_DIR}/./sdl3_mpv/app.cpp")
target_compile_options(mainVideoPlayerSdl3Mpv PUBLIC -Wno-unused-parameter)

# tracyEnabled
add_library(tracyEnabled OBJECT "${CMAKE_CURRENT_LIST_DIR}/./contrib/tracy/public/TracyClient.cpp")
add_executable(imzero_video_play $<TARGET_OBJECTS:mainVideoPlayerSdl3Mpv>
$<TARGET_OBJECTS:tracyEnabled>)

