#!/bin/bash
rm -f transferRawFrames
mkfifo transferRawFrames

#./imgui_skia_exe -fffiInterpreter off -ttfFilePath ./SauceCodeProNerdFontPropo-Regular.ttf -backdropFilter off | \
#  ffmpeg -readrate 1 \
#         -report \
#         -f image2pipe \
#         -i - \
#	 -c:v libx264 -pix_fmt yuv444p \
#	 -preset fast \
#	 -f nut "pipe:1" | ffplay -hide_banner -f nut -i "pipe:0"
#./imgui_skia_exe -fffiInterpreter off -ttfFilePath ./SauceCodeProNerdFontPropo-Regular.ttf -backdropFilter off | \
#  ffmpeg -readrate 1 \
#         -report -hide_banner \
#         -f image2pipe \
#         -i - \
#	 -c:v libx264 -pix_fmt yuv444p \
#	 -f nut "pipe:1" | \
#  ffplay -hide_banner \
#         -f nut -i "pipe:0"


#./imgui_skia_exe -fffiInterpreter off -ttfFilePath ./SauceCodeProNerdFontPropo-Regular.ttf -backdropFilter off | \
#  ffmpeg -readrate 1 \
#         -report -hide_banner \
#         -f image2pipe \
#         -i - \
#	 -c:v libx264 -pix_fmt yuv444p \
#	 -f mpegts "pipe:1" | \
#  ffplay -hide_banner \
#         -f nut -i "pipe:0"

# see https://moctodemo.akamaized.net/tools/ffbuilder/#
#./imgui_skia_exe -fffiInterpreter off -ttfFilePath ./SauceCodeProNerdFontPropo-Regular.ttf -backdropFilter off | \
#ffmpeg -hide_banner \
#-loglevel debug \
#-re -fflags +genpts  \
#-f image2pipe -i - \
#-flags +global_header -r 30000/1001 \
#-pix_fmt yuv420p \
#-c:v libx264 \
#-b:v:0 3000K -maxrate:v:0 3000K -bufsize:v:0 3000K/2 \
#-g:v 30 -keyint_min:v 30 -sc_threshold:v 0 \
#-color_primaries bt709 -color_trc bt709 -colorspace bt709 \
#-c:a aac -ar 48000 -b:a 96k \
#-preset superfast \
#-tune zerolatency \
#-f nut "pipe:1" | \
#mpv --no-cache --untimed --no-demuxer-thread --video-sync=audio \
#  --vd-lavc-threads=1 -
##ffplay -hide_banner \
## -analyzeduration 1 -fflags -nobuffer -probesize 32 -sync ext \
## -f nut -i "pipe:0"

# needs apt install intel-media-va-driver-non-free
# see https://en.wikipedia.org/wiki/Intel_Quick_Sync_Video#Hardware_decoding_and_encoding

resW=1920
resH=1080

./imgui_skia_exe -fffiInterpreter off -ttfFilePath ./SauceCodeProNerdFontPropo-Regular.ttf -backdropFilter off -videoRawFramesFile transferRawFrames -videoResolutionWidth $resW -videoResolutionHeight $resH &
pid=$!
       #-c:a aac -ar 48000 -b:a 96k \
       #-loglevel debug \
ffmpeg -hide_banner \
       -re -fflags +genpts  \
       -f image2pipe -i transferRawFrames \
       -flags +global_header -r 30000/1001 \
       -an \
       -vaapi_device /dev/dri/renderD128 \
       -vf "format=nv12,hwupload,scale_vaapi=w=$resW:h=$resH" \
       -c:v h264_vaapi -qp:v 26 -bf 0 -qp:v 26 -bf 0 -async_depth 4  \
       -f nut "pipe:1" | tee out.nut | \
ffplay -hide_banner \
       -threads 1 -filter_threads 1 \
       -probesize 32 -sync ext \
       -fpsprobesize 0 -framedrop -fast -infbuf \
       -f nut -fflags '+nobuffer' -flags2 '+fast' -i "pipe:0" \
       -vf "drawtext=text='%{localtime\:%S-%6N}':fontsize=144:box=1:boxcolor=black:fontcolor=red:y=(main_h/2)+text_h"

#mpv --no-cache --untimed --no-demuxer-thread --video-sync=audio \
#  --vd-lavc-threads=1 -
kill $pid

#ffplay -hide_banner \
# -analyzeduration 1 -fflags -nobuffer -probesize 32 -sync ext \
# -f nut -i "pipe:0"
