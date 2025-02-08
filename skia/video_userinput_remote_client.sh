#!/bin/bash
set -v
rm -f transferRawFrames
rm -f transferUserInteractionEvents
mkfifo transferRawFrames
mkfifo transferUserInteractionEvents

here=$(dirname "$(readlink -f "$BASH_SOURCE")")
cd "$here"

resW=1920
resH=1080
fps=60

#-vf "drawtext=text='%{localtime\:%S-%6N}':fontsize=144:box=1:boxcolor=black:fontcolor=red:y=(main_h/2)+text_h" \
ffplay -hide_banner \
       -framerate $fps \
       -threads 1 -filter_threads 1 \
       -probesize 32 -sync ext \
       -fpsprobesize 0 -framedrop -fast -infbuf \
       -f nut -fflags '+nobuffer' -flags2 '+fast' -i udp://195.201.119.88:5000
