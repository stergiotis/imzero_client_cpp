#!/bin/bash
set -ev
rm -f transfer
mkfifo transfer
font="./martian/MartianMono-StdRg.ttf"
font="./SauceCodeProNerdFontMono-Regular.ttf"
./main_go --logFormat console demo --mainFontTTF "$font" --mainFontSizeInPixels 13 "$@" < transfer | ./imgui_exe -ttfFilePath "$font" -fffiInterpreter on -skiaBackendType gl -vsync on -backdropFilter on > transfer
rm -f transfer
