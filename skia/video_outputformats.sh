#!/bin/bash
here=$(dirname "$(readlink -f "$BASH_SOURCE")")
cd "$here"

mkdir -p "test"
function run() {
	./imgui_skia_exe -fffiInterpreter off \
		         -ttfFilePath ./SauceCodeProNerdFontPropo-Regular.ttf \
			 -backdropFilter off \
			 -videoResolutionWidth 1920 \
			 -videoResolutionHeight 1080 \
			 -videoExitAfterNFrames 1 \
			 -videoRawOutputFormat "$1" \
			 -videoRawFramesFile "test/${1}${2}"
}
run "bmp_bgra8888" ".bmp"
run "qoi" ".qoi"
run "jpeg" ".jpeg"
run "webp_lossy" ".webp"
run "webp_lossless" ".webp"
run "flatbuffers" ".flatbuffers"
run "png" ".png"
run "svg" ".svg"
run "svg_textaspath" ".svg"
run "skp" ".svg"
