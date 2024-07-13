#!/bin/bash
# credits: https://www.reddit.com/r/ffmpeg/comments/zqfeam/streaming_desktop_capture_over_the_lan_how_can_i/
#ffmpeg -hide_banner \
#	-f lavfi -i "testsrc=size=hd1080:rate=30,drawtext=text='%{localtime\:%S-%6N}':fontsize=144:box=1:boxcolor=black:fontcolor=yellow:y=(main_h/2)-text_h" \
#	-c:v libx264 -g 1 -preset ultrafast \
#	-f nut "pipe:1" | \
#ffplay -hide_banner \
#        -f nut -i "pipe:0" \
#	-vf "drawtext=text='%{localtime\:%S-%6N}':fontsize=144:box=1:boxcolor=black:fontcolor=red:y=(main_h/2)+text_h"

# ffmpeg -h encoder=h264_nvenc
# ffmpeg -h encoder=hevc_vaapi

#ffmpeg -hide_banner \
#	-threads 1 -filter_threads 1 \
#	-f lavfi \
#	-i "testsrc=size=hd1080:rate=60,drawtext=text='%{localtime\:%S-%6N}':fontsize=144:box=1:boxcolor=black:fontcolor=yellow:y=(main_h/2)-text_h,format=pix_fmts=yuv420p" \
#	-threads 0 -frame_drop_threshold \
#	-1 -g 1 -fps_mode:v vfr -c:v libx264 -tune zerolatency -muxdelay 0 -flags2 '+fast' \
#	-f nut "pipe:1" | \
#ffplay -hide_banner \
#        -threads 1 -filter_threads 1 \
#	-probesize 32 -sync ext \
#	-fpsprobesize 0 -framedrop -fast -infbuf -f nut -fflags '+nobuffer' -flags2 '+fast' -i "pipe:0" -vf "drawtext=text='%{localtime\:%S-%6N}':fontsize=144:box=1:boxcolor=black:fontcolor=red:y=(main_h/2)+text_h"


# see https://gist.github.com/Brainiarc7/4636a162ef7dc2e8c9c4c1d4ae887c0e

ffmpeg -hide_banner \
	-threads 1 -filter_threads 1 \
	-f lavfi \
	-i "testsrc=size=hd1080:rate=60,drawtext=text='%{localtime\:%S-%6N}':fontsize=144:box=1:boxcolor=black:fontcolor=yellow:y=(main_h/2)-text_h,format=pix_fmts=yuv420p" \
	-threads 0 -frame_drop_threshold -1 -g 1 \
	-vaapi_device /dev/dri/renderD128 -vf 'hwmap=derive_device=vaapi,scale_vaapi=w=1920:h=1080:format=nv12' \
	-c:v h264_vaapi -qp:v 19 -bf 4 -threads 4 -aspect 16:9 \
	-loglevel debug \
	-f nut "pipe:1" #| \
#ffplay -hide_banner \
#        -threads 1 -filter_threads 1 \
#	-probesize 32 -sync ext \
#	-fpsprobesize 0 -framedrop -fast -infbuf -f nut -fflags '+nobuffer' -flags2 '+fast' -i "pipe:0" -vf "drawtext=text='%{localtime\:%S-%6N}':fontsize=144:box=1:boxcolor=black:fontcolor=red:y=(main_h/2)+text_h"
