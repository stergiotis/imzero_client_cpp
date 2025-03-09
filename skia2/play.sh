#!/bin/bash
resW=1920
resH=1024
#ffmpeg -hide_banner \
#       -re -fflags +genpts  \
#       -f image2pipe -vcodec bmp -i transferRawFrames \
#       -flags +global_header -r 30000/1001 \
#       -an \
#       -vaapi_device /dev/dri/renderD128 \
#       -vf "format=nv12,hwupload,scale_vaapi=w=$resW:h=$resH" \
#       -c:v h264_vaapi -qp:v 5 -bf 0 -async_depth 4  \
#       -f nut "pipe:1" | \
#ffmpeg -hide_banner \
#       -re -fflags +genpts  \
#       -f image2pipe -vcodec bmp -i transferRawFrames \
#       -flags +global_header -r 30000/1001 \
#       -an \
#       -vaapi_device /dev/dri/renderD128 \
#       -vf "format=nv12,hwupload,scale_vaapi=w=$resW:h=$resH" \
#       -c:v mpeg2_vaapi -qp:v 5 -bf 0  \
#       -f nut "pipe:1" | \
#ffmpeg -hide_banner \
#       -re -fflags +genpts  \
#       -f image2pipe -vcodec bmp -i transferRawFrames \
#       -flags +global_header -r 30000/1001 \
#       -an \
#       -pixel_format yuv444 -vcodec vc2 \
#       -f nut "pipe:1" | \
#ffmpeg -hide_banner \
#       -re -fflags +genpts  \
#       -f image2pipe -vcodec bmp -i transferRawFrames \
#       -flags +global_header -r 30000/1001 \
#       -an \
#       -pixel_format rgb -vcodec vc2 \
#       -f nut "pipe:1" | \
#ffmpeg -hide_banner \
#       -re -fflags +genpts  \
#       -f image2pipe -vcodec bmp -i transferRawFrames \
#       -flags +global_header -r 30000/1001 \
#       -an \
#       -pixel_format rgb -vcodec mjpeg -q:v 3 \
#       -f nut "pipe:1" | \
#ffmpeg -hide_banner \
#       -re -fflags +genpts  \
#       -f image2pipe -vcodec bmp -i transferRawFrames \
#       -flags +global_header -r 30000/1001 \
#       -an \
#       -pixel_format rgb -vcodec rawvideo \
#       -f nut "pipe:1" | \
#../video_player/imzero_video_play - > transferUserInteractionEvents
ffmpeg -hide_banner \
       -re -fflags +genpts  \
       -f image2pipe -vcodec bmp -i transferRawFrames \
       -flags +global_header -r 30000/1001 \
       -an \
       -vcodec ffv1 \
       -f nut "pipe:1" | \
#mplayer -benchmark -
../video_player/imzero_video_play - > transferUserInteractionEvents
#ffplay -hide_banner \
#       -threads 1 -filter_threads 1 \
#       -probesize 32 -sync ext \
#       -fpsprobesize 0 -framedrop -fast -infbuf \
#       -f nut -fflags '+nobuffer' -flags2 '+fast' -i "pipe:0"
#-vf "drawtext=text='%{localtime\:%S-%6N}':fontsize=144:box=1:boxcolor=black:fontcolor=red:y=(main_h/2)+text_h"
#../video_player/imzero_video_play - > transferUserInteractionEvents
#~/repo/contrib/sdl/build2/test/testffmpeg --software /dev/fd/0
#ffplay -hide_banner \
#       -threads 1 -filter_threads 1 \
#       -probesize 32 -sync ext \
#       -fpsprobesize 0 -framedrop -fast -infbuf \
#       -f nut -fflags '+nobuffer' -flags2 '+fast' -i transferRawFrames
