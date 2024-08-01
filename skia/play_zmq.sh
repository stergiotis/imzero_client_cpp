#!/bin/bash
set -ev
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
       -probesize 1024 -analyzeduration 1000 \
       -re \
       -f image2pipe -vcodec bmp -i transferRawFrames \
       -an \
       -pixel_format yuv444p -vcodec mjpeg -pix_fmt yuv444p -q:v 1 \
       -f matroska zmq:tcp://127.0.0.1:5555
#ffmpeg -hide_banner \
#       -probesize 1024 -analyzeduration 1000 \
#       -re \
#       -f image2pipe -vcodec bmp -i transferRawFrames \
#       -an \
#       -pixel_format yuv444p -vcodec ffv1 \
#       -f nut zmq:tcp://127.0.0.1:5555
