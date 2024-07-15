#!/bin/bash
rm -f transferRawFrames
mkfifo transferRawFrames

resW=1920
resH=1080
outputFormat="bmp_bgra8888"
#outputFormat="jpeg"
#outputFormat="png"
#outputFormat="webp_lossy"

./imgui_skia_exe -fffiInterpreter off \
	         -ttfFilePath ./SauceCodeProNerdFontPropo-Regular.ttf \
		 -backdropFilter off \
		 -videoRawFramesFile transferRawFrames \
		 -videoResolutionWidth $resW \
		 -videoResolutionHeight $resH \
		 -videoRawOutputFormat $outputFormat &
pid=$!
#cat transferRawFrames | tee out.raw | \
#    mpv --no-cache --untimed --no-demuxer-thread -

mpv --no-cache --untimed --no-demuxer-thread transferRawFrames
#ffplay -hide_banner \
#       -threads 1 -filter_threads 1 \
#       -probesize 32 -sync ext \
#       -fpsprobesize 0 -framedrop -fast -infbuf \
#       -f nut -fflags '+nobuffer' -flags2 '+fast' -i "pipe:0" \
#       -vf "drawtext=text='%{localtime\:%S-%6N}':fontsize=144:box=1:boxcolor=black:fontcolor=red:y=(main_h/2)+text_h"

#mpv --no-cache --untimed --no-demuxer-thread --video-sync=audio \
kill $pid
