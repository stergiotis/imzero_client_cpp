#!/bin/bash
set -ev
here=$(dirname "$(readlink -f "$BASH_SOURCE")")
cd "$here"
./build.sh
font=$(../scripts/find_ttf_font_file.sh "DejaVu Sans:style=Book")
VSYNC="${VSYNC:-on}"
./bin/imgui_skia_exe -ttfFilePath "$font" \
                     -fontManager fontconfig \
		     -skiaBackendType gl \
		     -vsync $VSYNC \
		     -backdropFilter off \
		     -vectorCmd on \
		     -imguiNavKeyboard on
