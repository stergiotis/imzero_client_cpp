#!/bin/bash
set -ev
rm -f transferRawFrames
rm -f transferUserInteractionEvents
mkfifo transferRawFrames
mkfifo transferUserInteractionEvents

resW=1920
resH=1080

./imgui_video_exe -fffiInterpreter off \
            -ttfFilePath ./SauceCodeProNerdFontPropo-Regular.ttf \
            -backdropFilter off \
            -videoRawFramesFile transferRawFrames \
            -videoResolutionWidth $resW \
            -videoResolutionHeight $resH \
            -videoRawOutputFormat bmp_bgra8888 \
            -videoUserInteractionEventsInFile transferUserInteractionEvents \
            -videoExitAfterNFrames 0 \
            -fontManager fontconfig &
pid=$!
ffmpeg -hide_banner \
       -probesize 1024 -analyzeduration 1000 \
       -re \
       -f image2pipe -vcodec bmp -i transferRawFrames \
       -an \
       -pixel_format yuvj444p -vcodec mjpeg -q:v 1 \
       -f nut zmq:tcp://127.0.0.1:5555
kill $pid
