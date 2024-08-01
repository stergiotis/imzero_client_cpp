#!/bin/bash
# a simple, flicker-free, precise terminal clock
c=$(clear)
while true; do
    u=$(date +%Y-%m-%dT%H:%M:%S.%N)
    echo "$c$u"
done
