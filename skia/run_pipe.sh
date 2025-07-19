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
./main_go --logFormat console demo --mainFontTTF "$font" --mainFontSizeInPixels 15 "$@" < transfer | \
    ./bin/imgui_skia_exe -ttfFilePath "$font" \
                         -coreDump off \
                         -backgroundColorRGBA 8f8f8fff \
                         -fontDyFudge -0.058 \
                         -fontScaleOverride 0.822 \
                         -fffiInterpreter on \
                         -skiaBackendType gl \
                         -vsync $VSYNC \
                         -backdropFilter off \
                         -fontManager fontconfig \
                         -imguiNavKeyboard on \
                         -fullscreen off > transfer
rm -f transfer
