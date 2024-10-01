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
		 -videoUserInteractionEventsFile transferUserInteractionEvents \
		 -videoUserInteractionEventsAreBinary on \
                 -videoExitAfterNFrames 0 &
pid=$!
#cat transferRawFrames | ../video_player/imzero_video_play - > transferUserInteractionEvents
cat transferRawFrames | ../contrib/FFmpeg/ffplay -imzero_user_interaction_path transferUserInteractionEvents -
# ../video_player/imzero_video_play transferRawFrames > transferUserInteractionEvents
#cat transferRawFrames | tee out.raw | mpv -
kill $pid
