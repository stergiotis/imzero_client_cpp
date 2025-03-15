#!/bin/bash
set -e
here=$(dirname "$(readlink -f "$BASH_SOURCE")")
cd "$here"
rm -f core
coredumpctl -1 dump --output core
gdb ./bin/imgui_skia_exe core
