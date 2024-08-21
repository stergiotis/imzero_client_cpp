#!/bin/bash
set -ev
rm -f transferRawFrames
rm -f transferUserInteractionEvents
mkfifo transferRawFrames
mkfifo transferUserInteractionEvents

resW=1920
resH=1080

./imgui_skia_exe -fffiInterpreter off \
	         -ttfFilePath ./SauceCodeProNerdFontPropo-Regular.ttf \
		 -backdropFilter off \
		 -videoRawFramesFile transferRawFrames \
		 -videoResolutionWidth $resW \
		 -videoResolutionHeight $resH \
		 -videoRawOutputFormat bmp_bgra8888 \
		 -videoUserInteractionEventsInFile transferUserInteractionEvents \
                 -videoExitAfterNFrames 0 &
pid=$!
cat transferRawFrames | ../video_player/imzero_video_play - > transferUserInteractionEvents
# ../video_player/imzero_video_play transferRawFrames > transferUserInteractionEvents
#cat transferRawFrames | tee out.raw | mpv -
kill $pid
