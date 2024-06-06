#!/bin/bash
set -ev
here=$(dirname "$(readlink -f "$BASH_SOURCE")")
cd "$here"
./build_go.sh
./main_go --logFormat console --httpServerAddress localhost:8888 demo --imGuiBinary ./imgui_exe --mainFontTTF ./SauceCodeProNerdFontMono-Regular.ttf --mainFontSizeInPixels 18
