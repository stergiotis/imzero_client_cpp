#!/bin/bash
here=$(dirname "$(readlink -f "$BASH_SOURCE")")
cd "$here"
source "../common/lib.sh"

link ../common/src src
link ../common/contrib/imgui_implot/implot.cpp ./implot/implot.cpp
link ../common/contrib/imgui_implot/implot.h ./implot/implot.h 
link ../common/contrib/imgui_implot/implot_internal.h ./implot/implot_internal.h 
link ../common/contrib/imgui_implot/implot_items.cpp ./implot/implot_items.cpp 
link ../common/contrib/imgui_implot/implot_demo.cpp ./implot/implot_demo.cpp 
link ../common/contrib/imgui_ImGuiColorTextEdit/TextEditor.cpp ./imcolortextedit/TextEditor.cpp 
link ../common/contrib/imgui_ImGuiColorTextEdit/TextEditor.h ./imcolortextedit/TextEditor.h 
link ../common/contrib/imgui_ImGuiColorTextEdit/LanguageDefinitions.cpp ./imcolortextedit/LanguageDefinitions.cpp