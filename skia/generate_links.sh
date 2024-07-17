#!/bin/bash
here=$(dirname "$(readlink -f "$BASH_SOURCE")")
cd "$here"
source "../common/lib.sh"

link ../common/src src
link ../common/contrib/imgui/imgui.cpp ./imgui/imgui.cpp 
link ../common/contrib/imgui/imgui_demo.cpp ./imgui/imgui_demo.cpp 
link ../common/contrib/imgui/imgui_internal.h ./imgui/imgui_internal.h 
link ../common/contrib/imgui/imgui_tables.cpp ./imgui/imgui_tables.cpp 
link ../common/contrib/imgui/imgui_widgets.cpp ./imgui/imgui_widgets.cpp 
link ../common/contrib/imgui/imstb_rectpack.h ./imgui/imstb_rectpack.h 
link ../common/contrib/imgui/imstb_textedit.h ./imgui/imstb_textedit.h 
link ../common/contrib/imgui/imstb_truetype.h ./imgui/imstb_truetype.h 
link ../common/contrib/imgui_implot/implot.cpp ./implot/implot.cpp 
link ../common/contrib/imgui_implot/implot.h ./implot/implot.h 
link ../common/contrib/imgui_implot/implot_internal.h ./implot/implot_internal.h 
link ../common/contrib/imgui_implot/implot_items.cpp ./implot/implot_items.cpp 
link ../common/contrib/imgui_implot/implot_demo.cpp ./implot/implot_demo.cpp 
link ../common/contrib/imgui_ImGuiColorTextEdit/TextEditor.cpp ./imcolortextedit/TextEditor.cpp 
link ../common/contrib/imgui_ImGuiColorTextEdit/TextEditor.h ./imcolortextedit/TextEditor.h 
link ../common/contrib/sdl3 contrib/sdl3
link ../common/contrib/qoi contrib/qoi
link ../common/contrib/tracy contrib/tracy
