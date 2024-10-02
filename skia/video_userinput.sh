#!/bin/bash
set -ev
rm -f transferRawFrames
rm -f transferUserInteractionEvents
mkfifo transferRawFrames
mkfifo transferUserInteractionEvents

here=$(dirname "$(readlink -f "$BASH_SOURCE")")
cd "$here"

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
ffmpeg -hide_banner \
       -re -fflags +genpts  \
       -framerate 60 \
       -f image2pipe -vcodec bmp -i transferRawFrames \
       -flags +global_header -r 30000/1001 \
       -an \
       -vaapi_device /dev/dri/renderD128 \
       -vf "fps=60,format=nv12,hwupload,scale_vaapi=w=$resW:h=$resH" \
       -c:v h264_vaapi -qp:v 26 -bf 0 -qp:v 26 -bf 0 -async_depth 4  \
       -f nut "pipe:1" | \
../../contrib/FFmpeg/ffplay -hide_banner \
       -threads 1 -filter_threads 1 \
       -probesize 32 -sync ext \
       -fflags nobuffer -flags low_delay \
       -fpsprobesize 0 -framedrop -fast -infbuf \
       -f nut -fflags '+nobuffer' -flags2 '+fast' -i "pipe:0"  \
       -imzero_user_interaction_path transferUserInteractionEvents
kill $pid
