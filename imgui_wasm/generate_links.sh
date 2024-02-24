#!/bin/bash
here=$(dirname "$(readlink -f "$BASH_SOURCE")")

function link() {
   d=$(dirname "$2")
   b=$(basename "$2")
   cd "$here"
   mkdir -p "$d"
   rm -f "$2"
   touch "$2"
   l=$(realpath --strip --relative-to="$d" "$1")

   cd "$d"
   rm -f "$b"
   echo "$l --> $b"
   ln -s "$l" "$b"
}
link ../../contrib/imgui contrib/imgui
link ../../contrib/imgui_club contrib/imgui_club
link ../../contrib/imgui_flame-graph contrib/imgui_flame-graph
link ../../contrib/imgui_ImCoolBar contrib/imgui_ImCoolBar
link ../../contrib/imgui_ImGuiColorTextEdit contrib/imgui_ImGuiColorTextEdit
link ../../contrib/imgui_implot contrib/imgui_implot
link ../../contrib/imgui_imspinner contrib/imgui_imspinner
link ../../contrib/imgui_knobs contrib/imgui_knobs
link ../../contrib/imgui_toggle contrib/imgui_toggle
link ./contrib/imgui_toggle/imgui_toggle.cpp ./src/widgets/imgui_toggle/imgui_toggle.cpp 
link ./contrib/imgui_toggle/imgui_toggle_palette.cpp ./src/widgets/imgui_toggle/imgui_toggle_palette.cpp 
link ./contrib/imgui_toggle/imgui_toggle_presets.cpp ./src/widgets/imgui_toggle/imgui_toggle_presets.cpp 
link ./contrib/imgui_toggle/imgui_toggle_renderer.cpp ./src/widgets/imgui_toggle/imgui_toggle_renderer.cpp 
link ./contrib/imgui_toggle/imgui_offset_rect.h ./src/widgets/imgui_toggle/imgui_offset_rect.h 
link ./contrib/imgui_toggle/imgui_toggle.h ./src/widgets/imgui_toggle/imgui_toggle.h 
link ./contrib/imgui_toggle/imgui_toggle_math.h ./src/widgets/imgui_toggle/imgui_toggle_math.h 
link ./contrib/imgui_toggle/imgui_toggle_palette.h ./src/widgets/imgui_toggle/imgui_toggle_palette.h 
link ./contrib/imgui_toggle/imgui_toggle_presets.h ./src/widgets/imgui_toggle/imgui_toggle_presets.h 
link ./contrib/imgui_toggle/imgui_toggle_renderer.h ./src/widgets/imgui_toggle/imgui_toggle_renderer.h 
link ./contrib/imgui_implot/implot.cpp ./src/widgets/imgui_implot/implot.cpp 
link ./contrib/imgui_implot/implot_demo.cpp ./src/widgets/imgui_implot/implot_demo.cpp 
link ./contrib/imgui_implot/implot.h ./src/widgets/imgui_implot/implot.h 
link ./contrib/imgui_implot/implot_internal.h ./src/widgets/imgui_implot/implot_internal.h 
link ./contrib/imgui_implot/implot_items.cpp ./src/widgets/imgui_implot/implot_items.cpp 
link ./contrib/imgui_knobs/imgui-knobs.cpp ./src/widgets/imgui_knobs/imgui-knobs.cpp 
link ./contrib/imgui_knobs/imgui-knobs.h ./src/widgets/imgui_knobs/imgui-knobs.h 
link ./contrib/imgui_imspinner/imspinner.h ./src/widgets/imgui_imspinner/imspinner.h 
link ./contrib/imgui_ImCoolBar/ImCoolbar.cpp ./src/widgets/imgui_coolbar/ImCoolbar.cpp 
link ./contrib/imgui_ImCoolBar/ImCoolbar.h ./src/widgets/imgui_coolbar/ImCoolbar.h 
link ./contrib/imgui_flame-graph/imgui_widget_flamegraph.cpp ./src/widgets/imgui_flamegraph/imgui_widget_flamegraph.cpp 
link ./contrib/imgui_flame-graph/imgui_widget_flamegraph.h ./src/widgets/imgui_flamegraph/imgui_widget_flamegraph.h 
link ./contrib/imgui_club/imgui_memory_editor/imgui_memory_editor.h ./src/widgets/imgui_club/imgui_memory_editor.h 
link ./contrib/imgui/examples/libs/emscripten/emscripten_mainloop_stub.h ./imgui/emscripten_mainloop_stub.h 
link ./contrib/imgui/imgui.cpp ./imgui/imgui.cpp 
link ./contrib/imgui/imgui.h ./imgui/imgui.h 
link ./contrib/imgui/imgui_demo.cpp ./imgui/imgui_demo.cpp 
link ./contrib/imgui/imgui_draw.cpp ./imgui/imgui_draw.cpp 
link ./contrib/imgui/imgui_internal.h ./imgui/imgui_internal.h 
link ./contrib/imgui/imgui_tables.cpp ./imgui/imgui_tables.cpp 
link ./contrib/imgui/imgui_widgets.cpp ./imgui/imgui_widgets.cpp 
link ./contrib/imgui/imstb_rectpack.h ./imgui/imstb_rectpack.h 
link ./contrib/imgui/imstb_textedit.h ./imgui/imstb_textedit.h 
link ./contrib/imgui/imstb_truetype.h ./imgui/imstb_truetype.h 
link ./contrib/imgui_implot/implot.cpp ./implot/implot.cpp 
link ./contrib/imgui_implot/implot.h ./implot/implot.h 
link ./contrib/imgui_implot/implot_internal.h ./implot/implot_internal.h 
link ./contrib/imgui_implot/implot_items.cpp ./implot/implot_items.cpp 
link ./contrib/imgui_implot/implot_demo.cpp ./implot/implot_demo.cpp 
link ./contrib/imgui_ImGuiColorTextEdit/TextEditor.cpp ./imcolortextedit/TextEditor.cpp 
link ./contrib/imgui_ImGuiColorTextEdit/TextEditor.h ./imcolortextedit/TextEditor.h 
