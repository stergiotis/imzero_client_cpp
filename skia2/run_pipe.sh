#!/bin/bash
set -ev
set -o pipefail
here=$(dirname "$(readlink -f "$BASH_SOURCE")")
cd "$here"

rm -f transfer
mkfifo transfer
font="./martian/MartianMono-StdRg.ttf"
font="./SauceCodeProNerdFontMono-Regular.ttf"
font=$(../scripts/find_ttf_font_file.sh "DejaVu Sans:style=Book")
VSYNC="${VSYNC:-on}"
./main_go --logFormat console demo --mainFontTTF "$font" --mainFontSizeInPixels 13 "$@" < transfer | \
	./bin/imgui_skia_exe -ttfFilePath "$font" -fffiInterpreter on -skiaBackendType gl -vsync $VSYNC -backdropFilter off \
	            -vectorCmd on \
		    -fontManager fontconfig -imguiNavKeyboard on > transfer
rm -f transfer
