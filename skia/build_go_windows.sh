#!/bin/bash
set -ev
here=$(dirname "$(readlink -f "$BASH_SOURCE")")
cd "$here"
rm -f main_go.exe
rm -f main_go
source tags.sh
export CGO_ENABLED=0 # ensure a cgo-free build
go build -tags "$build_tags" -o main_go.exe ../../boxer/public/imzero/main.go
