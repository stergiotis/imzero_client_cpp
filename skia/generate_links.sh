#!/bin/bash
here=$(dirname "$(readlink -f "$BASH_SOURCE")")
cd "$here"
source "../common/lib.sh"

link ../../contrib/imgui_implot/implot.cpp ./implot/implot.cpp
link ../../contrib/imgui_implot/implot.h ./implot/implot.h 
link ../../contrib/imgui_implot/implot_internal.h ./implot/implot_internal.h 
link ../../contrib/imgui_implot/implot_items.cpp ./implot/implot_items.cpp 
link ../../contrib/imgui_implot/implot_demo.cpp ./implot/implot_demo.cpp 
link ../../contrib/imgui_ImGuiColorTextEdit/TextEditor.cpp ./imcolortextedit/TextEditor.cpp 
link ../../contrib/imgui_ImGuiColorTextEdit/TextEditor.h ./imcolortextedit/TextEditor.h 
link ../../contrib/imgui_ImGuiColorTextEdit/LanguageDefinitions.cpp ./imcolortextedit/LanguageDefinitions.cpp
