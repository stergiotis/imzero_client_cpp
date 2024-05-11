#!/bin/bash
set -ev
rm -f fffiIn
rm -f fffiOut
mkfifo fffiIn
mkfifo fffiOut
font="./SauceCodeProNerdFontMono-Regular.ttf"
cat fffiOut &
./main_go --logFormat console demo --mainFontTTF "$font" --mainFontSizeInPixels 13 "$@" | pv -c > fffiIn
#./main_go --logFormat console demo --mainFontTTF "$font" --mainFontSizeInPixels 13 "$@" < fffiOut > fffiIn
rm -f fffiIn
rm -f fffiOut
