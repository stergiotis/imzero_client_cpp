#!/bin/bash
set -ev
here=$(dirname "$(readlink -f "$BASH_SOURCE")")
cd "$here"
rm -f transfer
mkfifo transfer
font="./martian/MartianMono-StdRg.ttf"
font="./SauceCodeProNerdFontMono-Regular.ttf"
export IMZERO_ASSERT_BASE_PATH="$here"
./main_go --logFormat console demo --mainFontTTF "$font" --mainFontSizeInPixels 13 "$@" < transfer | ./imgui_exe -vsync on > transfer
rm -f transfer
