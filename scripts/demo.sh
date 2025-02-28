#!/bin/bash
set -ev
here=$(dirname "$(readlink -f "$BASH_SOURCE")")
cd "$here/.."
go build -tags boxer_enable_profiling ./go/imzero.go

rm -f transfer
mkfifo transfer
font=$(./scripts/find_ttf_font_file.sh "DejaVu Sans:style=Book")
./imzero --logFormat console demo --mainFontTTF "$font" --mainFontSizeInPixels 13 "$@" < transfer | \
        ./skia/bin/imgui_skia_exe -ttfFilePath "$font" -fffiInterpreter on -skiaBackendType gl -vsync on -backdropFilter off \
                    -vectorCmd on \
                    -fontManager fontconfig -imguiNavKeyboard on > transfer
rm -f transfer
