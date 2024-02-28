#!/bin/bash
set -e
here=$(dirname "$(readlink -f "$BASH_SOURCE")")
cd "$here/.."
go build ./go/imzero.go
./imzero demo --imGuiBinary ./imgui_glfw/imgui_exe
