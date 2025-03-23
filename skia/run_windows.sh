#!/bin/bash
set -ev
here=$(dirname "$(readlink -f "$BASH_SOURCE")")
cd "$here"
./build_go_windows.sh
font="./SauceCodeProNerdFontPropo-Regular.ttf"
VSYNC="${VSYNC:-on}"
./main_go.exe --logFormat console --httpServerAddress localhost:8888 demo --imGuiBinary ./bin/imgui_skia.exe --mainFontTTF "$font" --mainFontSizeInPixels 13 \
	  -clientFontManager "directwrite" -clientSkiaBackendType gl -clientVsync $VSYNC -clientTtfFilePath "$font" \
	  -clientBackdropFilter off -clientVectorCmd on -clientImguiNavKeyboard on
