#!/bin/bash
set -ev
rm -f transfer
mkfifo transfer
font="./martian/MartianMono-StdRg.ttf"
font="./SauceCodeProNerdFontMono-Regular.ttf"
VSYNC="${VSYNC:-on}"
./main_go --logFormat console demo --mainFontTTF "$font" --mainFontSizeInPixels 13 "$@" < transfer | \
	./imgui_exe -ttfFilePath "$font" -fffiInterpreter on -skiaBackendType gl -vsync $VSYNC -backdropFilter off \
	            -vectorCmd on \
		    -fontManager fontconfig -imguiNavKeyboard on > transfer
rm -f transfer
