#!/bin/bash
set -ev
here=$(dirname "$(readlink -f "$BASH_SOURCE")")
cd "$here"
./build_go.sh
font=$(../scripts/find_ttf_font_file.sh "DejaVu Sans:style=Book")
./main_go --logFormat console --httpServerAddress localhost:8888 demo --imGuiBinary ./imgui_skia_exe --mainFontTTF "$font" --mainFontSizeInPixels 18
