#!/bin/bash
here=$(dirname "$(readlink -f "$BASH_SOURCE")")
cd "$here"
source "../common/lib.sh"

link ../common/contrib/imgui/imgui.cpp ./imgui/imgui.cpp
link ../common/contrib/imgui/imgui_demo.cpp ./imgui/imgui_demo.cpp 
link ../common/contrib/imgui/imgui_internal.h ./imgui/imgui_internal.h 
link ../common/contrib/imgui/imgui_tables.cpp ./imgui/imgui_tables.cpp 
link ../common/contrib/imgui/imgui_widgets.cpp ./imgui/imgui_widgets.cpp 
link ../common/contrib/imgui/imstb_rectpack.h ./imgui/imstb_rectpack.h 
link ../common/contrib/imgui/imstb_textedit.h ./imgui/imstb_textedit.h 
link ../common/contrib/imgui/imstb_truetype.h ./imgui/imstb_truetype.h 
link ../common/contrib/imgui/imgui_draw.cpp ./imgui/imgui_draw.cpp
link ../common/contrib/imgui/imgui.h ./imgui/imgui.h
link ../common/contrib/imgui/hooking.h ./imgui/hooking.h
link ../common/contrib/imgui/imgui_draw.cpp ./imgui/imgui_draw.cpp
link ../common/contrib/sdl3 contrib/sdl3
link ../common/contrib/qoi contrib/qoi
link ../common/contrib/tracy contrib/tracy
link ../common/contrib/flatbuffers/include contrib/flatbuffers
link ../common/contrib/imgui/backends/imgui_impl_sdl3.cpp ./imgui/imgui_impl_sdl3.cpp
link ../common/contrib/imgui/backends/imgui_impl_sdl3.h ./imgui/imgui_impl_sdl3.h
