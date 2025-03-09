#!/bin/bash
set -ev

../../contrib/FFmpeg/ffplay -hide_banner \
       -threads 1 -filter_threads 1 \
       -probesize 32 -sync ext \
       -fpsprobesize 0 -framedrop -fast -infbuf \
       -imzero_user_interaction_path transferUserInteractionEvents \
       -f nut -fflags '+nobuffer' -flags2 '+fast' -i "srt://195.201.119.88:5000?mode=caller&smoother=live"
