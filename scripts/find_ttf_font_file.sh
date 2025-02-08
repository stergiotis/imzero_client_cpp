#!/bin/bash
set -e
set -o pipefail
fc-list | grep ".ttf:" | grep -i "$1" | head -n 1 | cut -d ":" -f 1 | tr -d "\n"
