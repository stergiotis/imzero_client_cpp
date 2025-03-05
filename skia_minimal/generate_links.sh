#!/bin/bash
set -ev
here=$(dirname "$(readlink -f "$BASH_SOURCE")")
cd "$here" || exit
source "../common/lib.sh"

link ../common/contrib/imgui/imgui.cpp ./imgui_w_hooks_1.91.9_wip/imgui.cpp
link ../common/contrib/imgui/imgui_demo.cpp ./imgui_w_hooks_1.91.9_wip/imgui_demo.cpp
link ../common/contrib/imgui/imgui_internal.h ./imgui_w_hooks_1.91.9_wip/imgui_internal.h
link ../common/contrib/imgui/imgui_tables.cpp ./imgui_w_hooks_1.91.9_wip/imgui_tables.cpp
link ../common/contrib/imgui/imgui_widgets.cpp ./imgui_w_hooks_1.91.9_wip/imgui_widgets.cpp
link ../common/contrib/imgui/imstb_rectpack.h ./imgui_w_hooks_1.91.9_wip/imstb_rectpack.h
link ../common/contrib/imgui/imstb_textedit.h ./imgui_w_hooks_1.91.9_wip/imstb_textedit.h
link ../common/contrib/imgui/imstb_truetype.h ./imgui_w_hooks_1.91.9_wip/imstb_truetype.h
link ../common/contrib/imgui/imgui_draw.cpp ./imgui_w_hooks_1.91.9_wip/imgui_draw.cpp
link ../common/contrib/imgui/imgui.h ./imgui_w_hooks_1.91.9_wip/imgui.h
link ../common/contrib/imgui/hooking.h ./imgui_w_hooks_1.91.9_wip/hooking.h
link ../common/contrib/imgui/imgui_draw.cpp ./imgui_w_hooks_1.91.9_wip/imgui_draw.cpp
link ../common/contrib/sdl3 contrib/sdl3
link ../common/contrib/tracy contrib/tracy
link ../common/contrib/flatbuffers/include contrib/flatbuffers
link ../common/contrib/imgui/backends/imgui_impl_sdl3.cpp ./imgui_w_hooks_1.91.9_wip/imgui_impl_sdl3.cpp
link ../common/contrib/imgui/backends/imgui_impl_sdl3.h ./imgui_w_hooks_1.91.9_wip/imgui_impl_sdl3.h
