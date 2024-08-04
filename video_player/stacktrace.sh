#!/bin/bash
rm -f core
coredumpctl -1 dump --output core
gdb ./imzero_video_play
