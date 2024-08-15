#!/bin/bash
set -ev

ffplay zmq:tcp://127.0.0.1:5555
#ffplay -hide_banner \
#       -threads 1 -filter_threads 1 \
#       -probesize 32 -sync ext \
#       -fpsprobesize 0 -framedrop -fast -infbuf \
#       -vf "drawtext=text='%{localtime\:%S-%6N}':fontsize=144:box=1:boxcolor=black:fontcolor=red:y=(main_h/2)+text_h" \
#       -f nut -fflags '+nobuffer' -flags2 '+fast' -i zmq:tcp://127.0.0.1:5555
