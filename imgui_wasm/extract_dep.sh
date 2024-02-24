#!/bin/bash
#cat "$1" | sed -E 's/[\\]$/ /' | paste -s -d " " | cut -d ":" -f 2
cat "$1" | sed -E 's/[\\]$/ /' | cut -d ":" -f 2 | sed -E 's/ /\n/'
