#!/bin/bash
resW=1920
resH=1080
ffmpeg -hide_banner \
       -re -fflags +genpts  \
       -f image2pipe -vcodec bmp -i transferRawFrames \
       -flags +global_header -r 30000/1001 \
       -an \
       -vaapi_device /dev/dri/renderD128 \
       -vf "format=nv12,hwupload,scale_vaapi=w=$resW:h=$resH" \
       -c:v h264_vaapi -qp:v 26 -bf 0 -qp:v 26 -bf 0 -async_depth 4  \
       -f nut "pipe:1" | ../video_player/imzero_video_play - > transferUserInteractionEvents
