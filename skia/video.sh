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

#./imgui_skia_exe -fffiInterpreter off -ttfFilePath ./SauceCodeProNerdFontPropo-Regular.ttf -backdropFilter off -videoRawFramesFile transferRawFrames -videoResolutionWidth 1024 -videoResolutionHeight 768 &
./run_pipe.sh &
pid=$!
ffmpeg -hide_banner \
       -loglevel debug \
       -re -fflags +genpts  \
       -f image2pipe -i transferRawFrames \
       -flags +global_header -r 30000/1001 \
       -vaapi_device /dev/dri/renderD128 \
       -vf 'format=nv12,hwupload,scale_vaapi=w=1920:h=1080' \
       -c:v h264_vaapi -qp:v 26 -bf 0 -tune zerolatency \
       -c:a aac -ar 48000 -b:a 96k \
       -f nut "pipe:1" | \
mpv --no-cache --untimed --no-demuxer-thread --video-sync=audio \
  --vd-lavc-threads=1 -
kill $pid

#ffplay -hide_banner \
# -analyzeduration 1 -fflags -nobuffer -probesize 32 -sync ext \
# -f nut -i "pipe:0"
