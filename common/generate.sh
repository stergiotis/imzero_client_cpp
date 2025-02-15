#!/bin/bash
set -ev
here=$(dirname "$(readlink -f "$BASH_SOURCE")")
cd "$here"
source "env.sh"
./generate_fb_go.sh
"../../boxer/public/imzero/generate.sh"
