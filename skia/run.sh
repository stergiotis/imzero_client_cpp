#!/bin/bash
set -ev
here=$(dirname "$(readlink -f "$BASH_SOURCE")")
cd "$here"
./build_go.sh
font="./SauceCodeProNerdFontMono-Regular.ttf"
font=$(../scripts/find_ttf_font_file.sh "DejaVu Sans:style=Book")
VSYNC="${VSYNC:-on}"
./main_go --logFormat console --pprofHttpListenAddress localhost:8888 demo --imGuiBinary ./bin/imgui_skia_exe --mainFontTTF "$font" --mainFontSizeInPixels 13 \
	  -clientFontManager fontconfig -clientSkiaBackendType gl -clientVsync $VSYNC -clientTtfFilePath "$font" \
	  -clientBackdropFilter off -clientVectorCmd on -clientImguiNavKeyboard on
