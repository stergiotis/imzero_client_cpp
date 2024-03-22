#!/bin/bash
set -ev
set -ev
here=$(dirname "$(readlink -f "$BASH_SOURCE")")
cd "$here"
rm -f transfer
mkfifo transfer
go build ../go/imzero.go
export IMZERO_ASSERT_BASE_PATH="$here"
./imzero demo < transfer | ./imgui_exe > transfer
rm -f transfer
