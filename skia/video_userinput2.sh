#!/bin/bash
set -ev
rm -f transferRawFrames
rm -f transferUserInteractionEvents
mkfifo transferRawFrames
mkfifo transferUserInteractionEvents

resW=1920
resH=1080

./imgui_exe -fffiInterpreter off \
            -ttfFilePath ./SauceCodeProNerdFontPropo-Regular.ttf \
            -backdropFilter off \
            -videoRawFramesFile transferRawFrames \
            -videoResolutionWidth $resW \
            -videoResolutionHeight $resH \
            -videoRawOutputFormat bmp_bgra8888 \
            -videoUserInteractionEventsInFile transferUserInteractionEvents \
            -videoExitAfterNFrames 0

#cat transferRawFrames | ../video_player/imzero_video_play - > transferUserInteractionEvents #&
#pid=$!
#./imgui_skia_exe -fffiInterpreter off \
#	         -ttfFilePath ./SauceCodeProNerdFontPropo-Regular.ttf \
#		 -backdropFilter off \
#		 -videoRawFramesFile transferRawFrames \
#		 -videoResolutionWidth $resW \
#		 -videoResolutionHeight $resH \
#		 -videoRawOutputFormat bmp_bgra8888 \
#		 -videoUserInteractionEventsInFile transferUserInteractionEvents \
#                 -videoExitAfterNFrames 0
#kill $pid
